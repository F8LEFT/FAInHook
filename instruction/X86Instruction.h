//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// Instruction helper for x86
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_X86INSTRUCTION_H
#define FAINHOOK_X86INSTRUCTION_H

#include "Instruction.h"

namespace FAHook {
    class X86Instruction: public Instruction {
    public:
        ~X86Instruction() {}
        bool createStub(HookInfo* info);
    };

}



#endif //FAINHOOK_X86INSTRUCTION_H
