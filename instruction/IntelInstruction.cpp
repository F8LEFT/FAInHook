//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/26.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "IntelInstruction.h"
#include "../MemHelper.h"
#include "IntelDisasm.h"

bool FAHook::IntelInstruction::createStub(FAHook::HookInfo *info) {

    uintptr_t source(reinterpret_cast<uintptr_t >(info->getOriginalAddr()));
    uintptr_t target(reinterpret_cast<uintptr_t >(info->getHookAddr()));

    auto required = MSSizeOfJump(target, source);
    auto current = new uint8_t[required];
    auto stub = current;
    MSWriteJump(current, source, target);

    info->setJumpStubLen(required);
    info->setJumpStubBack(stub);
    return true;
}

bool FAHook::IntelInstruction::createCallOriginalStub(FAHook::HookInfo *info) {
    uintptr_t source(reinterpret_cast<uintptr_t >(info->getOriginalAddr()));
    uintptr_t target(reinterpret_cast<uintptr_t >(info->getHookAddr()));

    uint8_t * area(reinterpret_cast<uint8_t *>(source));

    auto required = info->getJumpStubLen();
    auto used(0);
    while(used < required) {
        auto width = hde64_getInsWidth(area + used);
        if(width == 0) {
            return false;
        }
        used += width;
    }

    auto blank(used - required);
    uint8_t backup[used];
    memcpy(backup, area, used);

    if(backup[0] == 0xe9) {   // call, just ret distination addr
        auto dest = reinterpret_cast<void*>(source + 5 + *reinterpret_cast<uint32_t *>(backup + 1));
        info->setCallOriginalIns((uint8_t *) dest);
        return true;
    }

    if(!ia32 && backup[0] == 0xff && backup[1] == 0x25) {   // jmp, just get distination addr
        auto dest = reinterpret_cast<void*> (source + 6 + *reinterpret_cast<uint32_t *>(backup + 2));
        info->setCallOriginalIns((uint8_t*)dest);
        return true;
    }

    size_t length(used + MSSizeOfJump(source + used));

    for (size_t offset(0), width; offset != used; offset += width) {
        hde64s decode;
        hde64_disasm(backup + offset, &decode);
        width = decode.len;
        //_assert(width != 0 && offset + width <= used);

#ifdef __LP64__
        if ((decode.modrm & 0xc7) == 0x05) {
            if (decode.opcode == 0x8b) {
                void *destiny(area + offset + width + int32_t(decode.disp.disp32));
                uint8_t reg(decode.rex_r << 3 | decode.modrm_reg);
                length -= decode.len;
                length += MSSizeOfPushPointer(destiny);
                length += MSSizeOfPop(reg);
                length += MSSizeOfMove64();
            } else {
                continue;
            }
        } else
#endif

        if (backup[offset] == 0xe8) {
            int32_t relative(*reinterpret_cast<int32_t *>(backup + offset + 1));
            void *destiny(area + offset + decode.len + relative);

            if (relative == 0) {
                length -= decode.len;
                length += MSSizeOfPushPointer(destiny);
            } else {
                length += MSSizeOfSkip();
                length += MSSizeOfJump(destiny);
            }
        } else if (backup[offset] == 0xeb) {
            length -= decode.len;
            length += MSSizeOfJump(area + offset + decode.len + *reinterpret_cast<int8_t *>(backup + offset + 1));
        } else if (backup[offset] == 0xe9) {
            length -= decode.len;
            length += MSSizeOfJump(area + offset + decode.len + *reinterpret_cast<int32_t *>(backup + offset + 1));
        } else if (
                backup[offset] == 0xe3 ||
                (backup[offset] & 0xf0) == 0x70
            // XXX: opcode2 & 0xf0 is 0x80?
                ) {
            length += decode.len;
            length += MSSizeOfJump(area + offset + decode.len + *reinterpret_cast<int8_t *>(backup + offset + 1));
        }
    }

    uint8_t * buffer = (uint8_t *) MemHelper::createExecMemory(length);

    if (buffer == nullptr) {
        return false;
    }

    uint8_t *current(buffer);

    for (size_t offset(0), width; offset != used; offset += width) {
        hde64s decode;
        hde64_disasm(backup + offset, &decode);
        width = decode.len;
        //_assert(width != 0 && offset + width <= used);

#ifdef __LP64__
        if ((decode.modrm & 0xc7) == 0x05) {
            if (decode.opcode == 0x8b) {
                void *destiny(area + offset + width + int32_t(decode.disp.disp32));
                uint8_t reg(decode.rex_r << 3 | decode.modrm_reg);
                MSPushPointer(current, destiny);
                MSWritePop(current, reg);
                MSWriteMove64(current, reg, reg);
            } else {
                goto copy;
            }
        } else
#endif

        if (backup[offset] == 0xe8) {
            int32_t relative(*reinterpret_cast<int32_t *>(backup + offset + 1));
            if (relative == 0)
                MSPushPointer(current, area + offset + decode.len);
            else {
                MSWrite<uint8_t>(current, 0xe8);
                MSWrite<int32_t>(current, MSSizeOfSkip());
                void *destiny(area + offset + decode.len + relative);
                MSWriteSkip(current, MSSizeOfJump(destiny, current + MSSizeOfSkip()));
                MSWriteJump(current, (uintptr_t) current, destiny);
            }
        } else if (backup[offset] == 0xeb)
            MSWriteJump(current, (uintptr_t) current, area + offset + decode.len + *reinterpret_cast<int8_t *>(backup + offset + 1));
        else if (backup[offset] == 0xe9)
            MSWriteJump(current, (uintptr_t) current, area + offset + decode.len + *reinterpret_cast<int32_t *>(backup + offset + 1));
        else if (
                backup[offset] == 0xe3 ||
                (backup[offset] & 0xf0) == 0x70
                ) {
            MSWrite<uint8_t>(current, backup[offset]);
            MSWrite<uint8_t>(current, 2);
            MSWrite<uint8_t>(current, 0xeb);
            void *destiny(area + offset + decode.len + *reinterpret_cast<int8_t *>(backup + offset + 1));
            MSWrite<uint8_t>(current, MSSizeOfJump(destiny, current + 1));
            MSWriteJump(current, (uintptr_t) current, destiny);
        } else
#ifdef __LP64__
            copy:
#endif
            {
                MSWrite(current, backup + offset, width);
            }
    }

    MSWriteJump(current, (uintptr_t) current, area + used);

    info->setCallOriginalIns(buffer);

    return true;
}
