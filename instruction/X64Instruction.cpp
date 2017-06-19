//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "X64Instruction.h"

bool FAHook::X64Instruction::createStub(FAHook::HookInfo *info) {
    auto addr = (uint64_t)info->getOriginalAddr();

    uint8_t * stub = nullptr;
    uint32_t stubsize = 0;
    if(addr <= 0xffffffff) {    // same as x86
        stubsize = 6;
        stub = new uint8_t[6];
        stub[0] = 0x68;         // push imme
        *(uint32_t *)&stub[1] = (uint32_t)((uint64_t)info->getHookAddr() & 0xffffffff);  // imme
        stub[5] = 0xc3;         // retn

        info->setJumpStubLen(stubsize);
        info->setJumpStubBack(stub);
    } else {
        stubsize = 14;
        stub = new uint8_t[14];
        *(uint16_t *)&stub[0] = 0x25ff;     // jmp cs:qword imm
        *(uint32_t *)&stub[2] = 0x00000000;
        *(uint64_t *)&stub[6] = (uint64_t)info->getHookAddr();  //qw: imm
    }
    info->setJumpStubLen(stubsize);
    info->setJumpStubBack(stub);
    return true;
}

bool FAHook::X64Instruction::createCallOriginalStub(FAHook::HookInfo *info) {
    info->setCallOriginalIns(nullptr);
    return true;
}
