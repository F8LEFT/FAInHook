//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
// Instruction helper for arm. Base on GodInHook
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

    private:
        bool repairCallOriginIns(HookInfo* info, uint8_t repair[], uint32_t& repairLen);

        int getRepairInstruction(uint32_t instruction);

        enum RepairIns{
            BLX_ARM,      /*!< BLX <label>*/
            BL_ARM,       /*!< BL <label>*/
            B_ARM,        /*!< B <label>*/
            BX_ARM,       /*!< BX PC*/
            ADD_ARM,      /*!< ADD Rd, PC, Rm (Rd != PC, Rm != PC)*/
            ADR1_ARM,     /*!< ADR Rd, <label>*/
            ADR2_ARM,     /*!< ADR Rd, <label>*/
            MOV_ARM,      /*!< MOV Rd, PC*/
            LDR_ARM,      /*!<LDR Rt, <label>*/

            UNDEFINE,
        };
    };

}

#endif //FAINHOOK_ARMINSTRUCTION_H
