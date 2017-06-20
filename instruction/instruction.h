//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// Instruction base.
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_INSTRUCTION_H
#define FAINHOOK_INSTRUCTION_H

#include <string.h>
#include "../HookInfo.h"
#include "../elfex.h"

#include <assert.h>

namespace FAHook {
    class Instruction {
    public:
        virtual ~Instruction(){}

        static void* getOriginalAddr(HookInfo* info);

        /*create jump stub to jump to new func*/
        virtual bool createStub(HookInfo* info) = 0;
        /*create original jump stub to jump to old func*/
        virtual bool createCallOriginalStub(HookInfo* info) {
            info->setCallOriginalIns(nullptr);
            assert(false && "not support yet");
            return false;
        };
        /*create back stub*/
        bool createBackStub(HookInfo* info);


        /*make jump stub enable(patch func stub))*/
        static bool enableJumpStub(HookInfo* info);
        /*make jump stub disable(restore old func)*/
        static bool disableJumpStub(HookInfo* info);

        static FunctionType getFunctionType(Elf_Addr functionAddr) {
#if defined(__arm__)
            if(0 == functionAddr) {
                return ERRTYPE;
            } else if((functionAddr & 3) == 0) {
                return ARM;
            } else {
                return THUMB;
            }
#elif defined(__aarch64__)
            return ARM64;
#elif defined(__i386__)
            return X86;
#elif defined(__x86_64__)
            return X64;
#elif defined(__mips64__)  /* mips64el-* toolchain defines __mips__ too */
            return MIPS64;
#elif defined(__mips__)
            return MIPS;
#endif
        }

        static bool patchMemory(void* dest, void* src, uint32_t len);

    };

}

#endif //FAINHOOK_INSTRUCTION_H
