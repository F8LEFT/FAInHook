//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "ArmInstruction.h"
#include "../MemHelper.h"


bool FAHook::ArmInstruction::createStub(FAHook::HookInfo *info) {
    auto stubsize = 8;
    uint8_t* stub = new uint8_t[8];
    *(uint32_t *)&stub[0] = 0xe51ff004; // ldr pc, addr
    *(uint32_t *)&stub[4] = (uint32_t)info->getHookAddr();

    info->setJumpStubLen(stubsize);
    info->setJumpStubBack(stub);
    return true;
}

bool FAHook::ArmInstruction::createCallOriginalStub(FAHook::HookInfo *info) {
    uint8_t ins[128];
    uint32_t insLen;
    if(!repairCallOriginIns(info, ins, insLen)) {
        return false;
    }
    auto ism = MemHelper::createExecMemory(insLen);
    if(ism == nullptr) {
        return false;
    }
    memcpy(ism, ins, insLen);
    info->setCallOriginalIns((uint8_t*)ism);
    return true;
}

bool FAHook::ArmInstruction::repairCallOriginIns(FAHook::HookInfo *info,
                                                 uint8_t *repairIns,
                                                 uint32_t &repairLen) {
    auto originalAddress = (uint8_t*)info->getOriginalAddr();

    auto ins = (uint32_t*)originalAddress;
    uint32_t * repair = (uint32_t *) repairIns;
    if(NULL == repair)
        return false;
    int pos = 0;
    int jumpLen = info->getJumpStubLen();

    // 得到原始指令起始处的pc值
    auto originalPc = (uint32_t)originalAddress + 8;
    auto originalLr = (uint32_t)originalAddress + jumpLen;


    /**
     * 需要修正的是那些机器指令内部存储的是要操作数据基于当前PC的偏移值；
     * 修正思路，计算出绝对地址，构造跳转指令。
     */
    for(int i=0;i<jumpLen/(sizeof(uint32_t));i++){
        /// 为了线程安全
        int type = getRepairInstruction(ins[i]);
        size_t x = 0;
        int top_bit;
        size_t imm32;
        size_t value;
        switch(type){
            case BLX_ARM:
            case BL_ARM:
                repair[pos++] = 0xE28FE004;	// ADD LR, PC, #4
            case B_ARM:
            case BX_ARM:
            {
                repair[pos++] = 0xE51FF004;  	// LDR PC, [PC, #-4]
                if(BLX_ARM == type)
                    x = (((ins[i]) & 0xFFFFFF) << 2) | (((ins[i]) & 0x1000000) >> 23);
                else if (type == BL_ARM || type == B_ARM){
                    x = ((ins[i]) & 0xFFFFFF) << 2;
                }
                else {
                    x = 0;
                }
                top_bit = x >> 25;
                imm32 = top_bit ? (x | (0xFFFFFFFF << 26)) : x;
                if (type == BLX_ARM) {
                    value = originalPc + imm32 + 1;
                }
                else {
                    value = originalPc + imm32;
                }
                repair[pos++] = value;
                break;
            }
            case ADD_ARM:
            {
                int rd;
                int rm;
                int r;
                rd = ((ins[i]) & 0xF000) >> 12;
                rm = (ins[i]) & 0xF;
                for (r = 12; ; --r) {
                    if (r != rd && r != rm) {
                        break;
                    }
                }
                repair[pos++] = 0xE52D0004 | (r << 12);	// PUSH {Rr}
                repair[pos++] = 0xE59F0008 | (r << 12);	// LDR Rr, [PC, #8]
                repair[pos++] = ((ins[i]) & 0xFFF0FFFF) | (r << 16);
                repair[pos++] = 0xE49D0004 | (r << 12);	// POP {Rr}
                repair[pos++] = 0xE28FF000;	// ADD PC, PC
                repair[pos++] = originalPc;
                break;
            }
            case ADR1_ARM:
            case ADR2_ARM:
            case LDR_ARM:
            case MOV_ARM:
            {
                int r;
                uint32_t value;

                r = ((ins[i]) & 0xF000) >> 12;

                if (type == ADR1_ARM || type == ADR2_ARM || type == LDR_ARM) {
                    uint32_t imm32;

                    imm32 = (ins[i]) & 0xFFF;
                    if (type == ADR1_ARM) {
                        value = originalPc + imm32;
                    }
                    else if (type == ADR2_ARM) {
                        value = originalPc - imm32;
                    }
                    else if (type == LDR_ARM) {
                        int is_add;

                        is_add = ((ins[i]) & 0x800000) >> 23;
                        if (is_add) {
                            value = ((size_t *) (originalPc + imm32))[0];
                        }
                        else {
                            value = ((size_t *) (originalPc - imm32))[0];
                        }
                    }
                }
                else {
                    value = originalPc;
                }
                repair[pos++] = 0xE51F0000 | (r << 12);	// LDR Rr, [PC]
                repair[pos++] = 0xE28FF000;	// ADD PC, PC
                repair[pos++] = value;
                break;
            }
            default:
            {
                ///无需修正
                repair[pos++] = ins[i];
            }
        }
        originalPc +=sizeof(uint32_t);
        //ins++;
    }
    repair[pos++] = 0xe51ff004;	// LDR PC, [PC, #-4]
    repair[pos++] = originalLr;

    repairLen = pos * 4;
    return true;
}

int FAHook::ArmInstruction::getRepairInstruction(uint32_t instruction) {
    if ((instruction & 0xFE000000) == 0xFA000000) {
        return BLX_ARM;
    }
    if ((instruction & 0xF000000) == 0xB000000) {
        return BL_ARM;
    }
    if ((instruction & 0xF000000) == 0xA000000) {
        return B_ARM;
    }
    if ((instruction & 0xFF000FF) == 0x120001F) {
        return BX_ARM;
    }
    if ((instruction & 0xFEF0010) == 0x8F0000) {
        return ADD_ARM;
    }
    if ((instruction & 0xFFF0000) == 0x28F0000) {
        return ADR1_ARM;
    }
    if ((instruction & 0xFFF0000) == 0x24F0000) {
        return ADR2_ARM;
    }
    if ((instruction & 0xE5F0000) == 0x41F0000) {
        return LDR_ARM;
    }
    if ((instruction & 0xFE00FFF) == 0x1A0000F) {
        return MOV_ARM;
    }
    return UNDEFINE;
}

