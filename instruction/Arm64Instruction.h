//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// instruction helper for arm64
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_ARM64INSTRUCTION_H
#define FAINHOOK_ARM64INSTRUCTION_H

#include "Instruction.h"

namespace FAHook {
    class Arm64Instruction: public Instruction {
    public:
        ~Arm64Instruction() {}
        bool createStub(HookInfo* info);
        bool createCallOriginalStub(HookInfo* info);

    };

}


#endif //FAINHOOK_ARM64INSTRUCTION_H
