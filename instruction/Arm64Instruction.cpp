//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "Arm64Instruction.h"

bool FAHook::Arm64Instruction::createStub(FAHook::HookInfo *info) {
    return false;
}

bool FAHook::Arm64Instruction::createCallOriginalStub(FAHook::HookInfo *info) {
    return Instruction::createCallOriginalStub(info);
}
