//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// Inline hook engine. Base on GodInHook.
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_FAINHOOK_H
#define FAINHOOK_FAINHOOK_H

#include "elf.h"

#if defined(__arm__)
#define S32
#elif defined(__aarch64__)
#define S64
#elif defined(__i386__)
#define S32
#elif defined(__x86_64__)
#define S64
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define S64
#elif defined(__mips__)
#define S32
#endif

#ifndef S64
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Sym Elf_Sym;
typedef Elf32_Rel Elf_Rel;
typedef Elf32_Rela Elf_Rela;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Dyn Elf_Dyn;
typedef Elf32_Word Elf_Word;
#else
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Rel Elf_Rel;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Dyn Elf_Dyn;
typedef Elf64_Word Elf_Word;
#endif

class FAInHook {
public:
    enum HOOK_STATUS {
        FERROR_UNKNOWN = -1,
        FERROR_SUCCESS = 0,
        FERROR_NOT_REGISTERED,
    };

    FAInHook* instance();

    /* you must register a func before hook.
     * callOrigin pointer to call origin funcation, not support yet!!!*/
    HOOK_STATUS registerHook(Elf_Addr orginalFunAddr, Elf_Addr newFunAddr,
                             Elf_Addr* callOrigin);

    /* do make hook*/
    HOOK_STATUS hook(Elf_Addr originalFunAddr);
    /* do unhook func*/
    HOOK_STATUS unhook(Elf_Addr originalFuncAddr);

    void hookAll();
    void unhookAll();

    /* check whether func is hooked.*/
    HOOK_STATUS isAlreadyHooked(Elf_Addr originalFunAddr);
    /* if func is hooked, return call original address.*/
    Elf_Ehdr getCallOriginFuncAddr(Elf_Addr originalFunAddr);
    /* if func is hooked, return new func address*/
    Elf_Addr getNewFunAddr(Elf_Addr originalFunAddr);


private:
    FAInHook();
    ~FAInHook();
};


#endif //FAINHOOK_FAINHOOK_H
