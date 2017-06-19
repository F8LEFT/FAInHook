//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// instruction helper for x64
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_X64INSTRUCTION_H
#define FAINHOOK_X64INSTRUCTION_H

#include "Instruction.h"

namespace FAHook {
    class X64Instruction: public Instruction {
    public:
        ~X64Instruction() {}
        bool createStub(HookInfo* info);
        bool createCallOriginalStub(HookInfo* info);
    };

}


#endif //FAINHOOK_X64INSTRUCTION_H
