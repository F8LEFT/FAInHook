//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "FAInHook.h"
#include "MemHelper.h"
#include "HookInfo.h"
#include "instruction/Instruction.h"

#if defined(__arm__)
#include "instruction/ArmInstruction.h"
#include "instruction/ThumbInstruction.h"
#elif defined(__aarch64__)
#include "instruction/Arm64Instruction.h"
#elif defined(__i386__)
#include "instruction/X86Instruction.h"
#elif defined(__x86_64__)
#include "instruction/X64Instruction.h"
#elif defined(__mips64__)  /* mips64el-* toolchain defines __mips__ too */
#include "instruction/Mips64Instruction.h"
#elif defined(__mips__)
#include "instruction/MipsInstruction.h"
#endif


FAInHook *FAInHook::instance() {
    static FAInHook* mIns = nullptr;
    if(mIns == nullptr) {
        mIns = new FAInHook();
    }
    return mIns;
}

FAInHook::FAInHook() {

}

FAInHook::~FAInHook() {

}



FAInHook::HOOK_STATUS FAInHook::registerHook(
        Elf_Addr orginalFunAddr, Elf_Addr newFunAddr, Elf_Addr *callOrigin) {
    // register hook information, calc hook stub at the same time.
    if(!FAHook::MemHelper::isFunctionAddr((void *) orginalFunAddr)
       || !FAHook::MemHelper::isFunctionAddr((void *) newFunAddr)) {
        return FERROR_NOT_EXECUTABLE;
    }

    auto info = getHookInfo(orginalFunAddr);
    if(nullptr != info) {
        auto hookStatus = info->getHookStatus();
        if(FAHook::HOOKED == hookStatus) {
            return FERROR_ALREADY_HOOKED;
        } else if(FAHook::REGISTERED == hookStatus) {
            delHookInfo(info);
        }
    }

    // check for FunctionType
    auto type = FAHook::Instruction::getFunctionType(orginalFunAddr);
    if(FAHook::ERRTYPE == type) {
        return FERROR_UNKNOWN;
    }

    info = new FAHook::HookInfo((void *) orginalFunAddr, (void *) newFunAddr);
    info->setOriginalFunctionType(type);

    FAHook::Instruction* instruction = nullptr;
    switch(type) {
#if defined(__arm__)
        case FAHook::ARM:
            instruction = new FAHook::ArmInstruction();
            break;
        case FAHook::THUMB:
            instruction = new FAHook::ThumbInstruction();
            break;
#elif defined(__aarch64__)
        case FAHook::ARM64:
            instruction = new FAHook::Arm64Instruction();
            break;
#elif defined(__i386__)
        case FAHook::X86:
            instruction = new FAHook::X86Instruction();
            break;
#elif defined(__x86_64__)
        case FAHook::X64:
            instruction = new FAHook::X64Instruction();
            break;
#elif defined(__mips64__)
            case FAHook::MIPS64:
            instruction = new FAHook::Mips64Instruction();
            break;
#elif defined(__mips__)
        case FAHook::MIPS:
            instruction = new FAHook::MipsInstruction();
            break;
#endif
        default:
            assert(false && "not support abi");
            break;
    }

    if(!instruction->createStub(info)
       || !instruction->createBackStub(info)
       || (callOrigin != nullptr) ?
            !instruction->createCallOriginalStub(info) : false  // want a callback
       ) {
        delete instruction;
        delete info;
        return FERROR_MEMORY;
    }

    addHookInfo(info);
    info->setHookStatus(FAHook::REGISTERED);

    if(callOrigin != nullptr) {
        *callOrigin = (Elf_Addr) info->getCallOriginalIns();
    }

    delete instruction;
    return FERROR_SUCCESS;
}

bool FAInHook::hook(Elf_Addr originalFunAddr) {
    auto info = getHookInfo(originalFunAddr);
    if(info == nullptr) {
        return false;
    }
    if(info->getHookStatus() == FAHook::HOOKED) {
        return true;
    } else if(info->getHookStatus() != FAHook::REGISTERED) {
        return false;
    }
    return Hook(info);
}

bool FAInHook::unhook(Elf_Addr originalFuncAddr) {
    auto info = getHookInfo(originalFuncAddr);
    if(info == nullptr) {
        return false;
    }
    if(info->getHookStatus() == FAHook::REGISTERED) {
        return true;
    } else if(info->getHookStatus() != FAHook::HOOKED) {
        return false;
    }
    return UnHook(info);
}

void FAInHook::hookAll() {
    for(auto it: hook_map) {
        if(it.second->getHookStatus() == FAHook::REGISTERED) {
            Hook(it.second);
        }
    }
}

void FAInHook::unhookAll() {
    for(auto it: hook_map) {
        if(it.second->getHookStatus() == FAHook::HOOKED) {
            UnHook(it.second);
        }
    }
}

bool FAInHook::isAlreadyHooked(Elf_Addr originalFunAddr) {
    auto it = hook_map.find(originalFunAddr);
    if(it == hook_map.end()) {
        return false;
    }
    auto info = it->second;
    return info->getHookStatus() == FAHook::HOOKED;
}

Elf_Addr FAInHook::getCallOriginFuncAddr(Elf_Addr originalFunAddr) {
    auto info = getHookInfo(originalFunAddr);
    if(info == nullptr) {
        return 0;
    }
    return (Elf_Addr) info->getCallOriginalIns();
}

Elf_Addr FAInHook::getNewFunAddr(Elf_Addr originalFunAddr) {
    auto info = getHookInfo(originalFunAddr);
    if(info == nullptr) {
        return 0;
    }
    return (Elf_Addr) info->getHookAddr();
}

int FAInHook::getHookedCount() {
    return hook_map.size();
}

FAHook::HookStatus FAInHook::getFunctionStatus(Elf_Addr functionAddr) {
    auto it = hook_map.find(functionAddr);
    if(it == hook_map.end()) {
        return FAHook::ERRSTATUS;
    }
    return it->second->getHookStatus();
}

FAHook::HookInfo *FAInHook::getHookInfo(Elf_Addr origFunAddr) {
    auto it = hook_map.find(origFunAddr);
    if(it == hook_map.end()) {
        return nullptr;
    }
    return it->second;
}

void FAInHook::addHookInfo(FAHook::HookInfo *info) {
    hook_map[(Elf_Addr)info->getOriginalAddr()] = info;
}

bool FAInHook::delHookInfo(FAHook::HookInfo *info) {
    auto it = hook_map.find((Elf_Addr)info->getOriginalAddr());
    if(it != hook_map.end()) {
        delete it->second;
        hook_map.erase(it);
        return true;
    }
    return false;
}

bool FAInHook::Hook(FAHook::HookInfo *info) {
    if(!FAHook::Instruction::enableJumpStub(info)) {
        return false;
    }
    info->setHookStatus(FAHook::HOOKED);
    return true;
}

bool FAInHook::UnHook(FAHook::HookInfo *info) {
    if(!FAHook::Instruction::disableJumpStub(info)) {
        return false;
    }
    info->setHookStatus(FAHook::REGISTERED);
    return true;
}


