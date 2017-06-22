//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "Arm64Instruction.h"

bool FAHook::Arm64Instruction::createStub(FAHook::HookInfo *info) {
    auto stubsize = 16;
    uint8_t* stub = new uint8_t[16];
    *(uint32_t *)&stub[0] = 0x58000051; // ldr x17, =stub[8]
    *(uint32_t *)&stub[4] = 0xd61f0220; // BR X17
    *(uint64_t *)&stub[8] = (uint64_t) info->getHookAddr();

    info->setJumpStubLen(stubsize);
    info->setJumpStubBack(stub);
    return true;
}
