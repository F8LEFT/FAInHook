//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "ArmInstruction.h"

bool FAHook::ArmInstruction::createStub(FAHook::HookInfo *info) {
    auto stubsize = 8;
    uint8_t* stub = new uint8_t[8];
    *(uint32_t *)&stub[0] = 0xe51ff004; // ldr pc, addr
    *(uint32_t *)&stub[4] = (uint32_t)info->getHookAddr();

    info->setJumpStubLen(stubsize);
    info->setJumpStubBack(stub);
    return true;
}

bool FAHook::ArmInstruction::createCallOriginalStub(FAHook::HookInfo *info) {
    info->setCallOriginalIns(nullptr);
    return true;
}
