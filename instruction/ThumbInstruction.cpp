//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "ThumbInstruction.h"

bool FAHook::ThumbInstruction::createStub(FAHook::HookInfo *info) {
    auto stubSize = 0;
    uint8_t *stub = nullptr;

    uint32_t addr = (uint32_t)info->getOriginalAddr();
    auto clearBit0 = addr & 0xfffffffe;

    if (clearBit0 % 4 != 0) {                       // need to align 4, just patch with nop
        stub = new uint8_t[10];
        ((uint16_t*)stub)[stubSize++] = 0xbf00;     //NOP
    } else {
        stub = new uint8_t[8];
    }
    ((uint16_t*)stub)[stubSize++] = 0xf8df;
    ((uint16_t*)stub)[stubSize++] = 0xf000; // LDR.W PC, [PC]
    ((uint16_t*)stub)[stubSize++] = (uint32_t)info->getHookAddr() & 0xffff;
    ((uint16_t*)stub)[stubSize++] = (uint32_t)info->getHookAddr() >> 16;

    info->setJumpStubLen(stubSize * 2);
    info->setJumpStubBack(stub);
    return true;
}
