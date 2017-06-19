//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "MipsInstruction.h"

bool FAHook::MipsInstruction::createStub(FAHook::HookInfo *info) {
    return false;
}

bool FAHook::MipsInstruction::createCallOriginalStub(FAHook::HookInfo *info) {
    return Instruction::createCallOriginalStub(info);
}
