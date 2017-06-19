//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// Inline hook engine. Base on GodInHook.
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_FAINHOOK_H
#define FAINHOOK_FAINHOOK_H

#include "elfex.h"
#include "HookInfo.h"
#include <map>

class FAInHook {
public:
    enum HOOK_STATUS {
        FERROR_UNKNOWN = -1,
        FERROR_SUCCESS = 0,
        FERROR_NOT_REGISTERED,
        FERROR_NOT_EXECUTABLE,
        FERROR_ALREADY_HOOKED,
        FERROR_MEMORY,
    };

    static FAInHook* instance();

    /* you must register a func before hook.
     * callOrigin pointer to call origin funcation, not support yet!!!*/
    HOOK_STATUS registerHook(Elf_Addr orginalFunAddr, Elf_Addr newFunAddr,
                             Elf_Addr* callOrigin);

    /* do make hook*/
    bool hook(Elf_Addr originalFunAddr);
    /* do unhook func*/
    bool unhook(Elf_Addr originalFuncAddr);

    void hookAll();
    void unhookAll();

    /* check whether func is hooked.*/
    bool isAlreadyHooked(Elf_Addr originalFunAddr);
    /* if func is hooked, return call original address.*/
    Elf_Addr getCallOriginFuncAddr(Elf_Addr originalFunAddr);
    /* if func is hooked, return hook func address*/
    Elf_Addr getNewFunAddr(Elf_Addr originalFunAddr);
    int getHookedCount();

private:
    FAInHook();
    ~FAInHook();

    FAHook::HookStatus getFunctionStatus(Elf_Addr functionAddr);

    FAHook::HookInfo* getHookInfo(Elf_Addr origFunAddr);
    void addHookInfo(FAHook::HookInfo *info);
    bool delHookInfo(FAHook::HookInfo *info);

    /* hook function and flash memory*/
    bool Hook(FAHook::HookInfo* info);
    /* unhook funciton and flash memory*/
    bool UnHook(FAHook::HookInfo* info);

    // map to record originalFun and HookInfo.
    std::map<Elf_Addr , FAHook::HookInfo*> hook_map;
};


#endif //FAINHOOK_FAINHOOK_H
