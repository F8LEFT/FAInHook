//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "Arm64Instruction.h"

/*
.plt:00000000000101E0 F0 01 00 B0                       ADRP            X16, #strlen_ptr@PAGE
.plt:00000000000101E4 11 06 46 F9                       LDR             X17, [X16,#strlen_ptr@PAGEOFF]
.plt:00000000000101E8 10 22 30 91                       ADD             X16, X16, #strlen_ptr@PAGEOFF
.plt:00000000000101EC 20 02 1F D6                       BR              X17 ; strlen
*/

bool FAHook::Arm64Instruction::createStub(FAHook::HookInfo *info) {
    return false;
}
