//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// Instruction helper for arm
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_ARMINSTRUCTION_H
#define FAINHOOK_ARMINSTRUCTION_H

#include "Instruction.h"

namespace FAHook {
    class ArmInstruction: public Instruction {
    public:
        ~ArmInstruction() {}
        bool createStub(HookInfo* info);
        bool createCallOriginalStub(HookInfo* info);
    };

}

#endif //FAINHOOK_ARMINSTRUCTION_H
