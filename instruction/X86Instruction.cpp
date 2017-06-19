//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "X86Instruction.h"

bool FAHook::X86Instruction::createStub(FAHook::HookInfo *info) {
    auto stubsize = 6;
    uint8_t* stub = new uint8_t[6];
    stub[0] = 0x68;         // push imme
    *(uint32_t *)&stub[1] = (uint32_t)info->getHookAddr();  // imme
    stub[5] = 0xc3;         // retn

    info->setJumpStubLen(stubsize);
    info->setJumpStubBack(stub);
    return true;
}

bool FAHook::X86Instruction::createCallOriginalStub(FAHook::HookInfo *info) {
    info->setCallOriginalIns(nullptr);
    return true;
}
