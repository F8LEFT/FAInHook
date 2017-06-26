//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// instruction helper for thumb. Base on GodInHook
//===----------------------------------------------------------------------===//


#ifndef FAINHOOK_THUMBINSTRUCTION_H
#define FAINHOOK_THUMBINSTRUCTION_H

#include "Instruction.h"

namespace FAHook {
    class ThumbInstruction: public Instruction {
    public:
        ~ThumbInstruction() {}
        bool createStub(HookInfo* info);

        bool createCallOriginalStub(HookInfo* info);

    private:
        bool repairCallOriginIns(HookInfo* info, uint8_t repair[], uint32_t& repairLen);

        bool isThumb2Instruction(uint16_t ins);

        int repairThumb32Instruction(uint32_t pc, uint16_t high_instruction, uint16_t low_instruction, uint16_t *respair);

        int repairThumb16Instruction(uint32_t pc, uint16_t instruction, uint16_t *respair);

        int getRepairInstruction(size_t instruction);

        uint32_t valueToMem(uint32_t addr)
        {
            return addr &(~0x1L);
        }

        uint32_t valueToPc(uint32_t addr)
        {
            return valueToMem(addr)+1;
        }

        enum RepairIns{
            // B <label>
                    B1_THUMB16 = 0,
            // B <label>
                    B2_THUMB16,
            // BX PC
                    BX_THUMB16,
            // ADD <Rdn>, PC (Rd != PC, Rn != PC) 在对ADD进行修正时，
            //采用了替换PC为Rr的方法，当Rd也为PC时，由于之前更改了Rr的值，
            //可能会影响跳转后的正常功能。
                    ADD_THUMB16,
            // MOV Rd, PC
                    MOV_THUMB16,
            // ADR Rd, <label>
                    ADR_THUMB16,
            // LDR Rt, <label>
                    LDR_THUMB16,

            // BLX <label>
                    BLX_THUMB32,
            // BL <label>
                    BL_THUMB32,
            // B.W <label>
                    B1_THUMB32,
            // B.W <label>
                    B2_THUMB32,
            // ADR.W Rd, <label>
                    ADR1_THUMB32,
            // ADR.W Rd, <label>
                    ADR2_THUMB32,
            // LDR.W Rt, <label>
                    LDR_THUMB32,
            // TBB [PC, Rm]
                    TBB_THUMB32,
            // TBH [PC, Rm, LSL #1]
                    TBH_THUMB32,

            UNDEFINE,
        };
    };

}


#endif //FAINHOOK_THUMBINSTRUCTION_H
