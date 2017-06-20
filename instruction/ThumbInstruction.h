//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// instruction helper for thumb
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_THUMBINSTRUCTION_H
#define FAINHOOK_THUMBINSTRUCTION_H

#include "Instruction.h"

namespace FAHook {
    class ThumbInstruction: public Instruction {
    public:
        ~ThumbInstruction() {}
        bool createStub(HookInfo* info);
    };

}


#endif //FAINHOOK_THUMBINSTRUCTION_H
