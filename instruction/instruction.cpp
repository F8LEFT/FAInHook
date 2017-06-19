//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//

#include "Instruction.h"
#include "../MemHelper.h"
#include <unistd.h>

bool FAHook::Instruction::enableJumpStub(FAHook::HookInfo *info) {
    auto origAddr = getOriginalAddr(info);
    auto len = info->getJumpStubLen();
    auto stubAddr = info->getJumpStubBack();
    return patchMemory(origAddr, stubAddr, len);
}

bool FAHook::Instruction::disableJumpStub(FAHook::HookInfo *info) {
    auto origAddr = getOriginalAddr(info);
    auto len = info->getBackLen();
    auto stubAddr = info->getOriginalStubBack();
    return patchMemory(origAddr, stubAddr, len);
}

bool FAHook::Instruction::patchMemory(void *dest, void *src, uint32_t len) {
    if(dest == nullptr || src == nullptr || len == 0) {
        return false;
    }
    if(!MemHelper::unProtectMemory(dest, len)) {
        return false;
    }

    memcpy(dest, src, len);
    MemHelper::protectMemory(dest, len);
#ifdef __arm__
    cacheflush((Elf_Addr)dest, (Elf_Addr)dest + len, 0);
#endif
    return true;
}

bool FAHook::Instruction::createBackStub(FAHook::HookInfo *info) {
    auto len = info->getJumpStubLen();
    if(len == 0) {
        return false;
    }
    auto stub = new uint8_t[len];
    info->setOriginalStubBack(stub);
    info->setBackLen(len);
    memcpy(stub, getOriginalAddr(info), len);
    return true;
}

void *FAHook::Instruction::getOriginalAddr(FAHook::HookInfo *info) {
    auto addr = info->getOriginalAddr();
    if(info->getOriginalFunctionType() == FAHook::THUMB) {
        addr =  (void *) ((Elf_Addr)addr & (~0x1L));
    }
    return addr;
}
