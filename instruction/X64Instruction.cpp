//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "X64Instruction.h"

bool FAHook::X64Instruction::createStub(FAHook::HookInfo *info) {
    return false;
}

bool FAHook::X64Instruction::createCallOriginalStub(FAHook::HookInfo *info) {
    return Instruction::createCallOriginalStub(info);
}
