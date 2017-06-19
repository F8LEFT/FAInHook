//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/19.
//                   Copyright (c) 2017. All rights reserved.
//===----------------------------------------------------------------------===//
//
//===----------------------------------------------------------------------===//


#include "FAInHook.h"

FAInHook *FAInHook::instance() {
    static FAInHook* mIns = nullptr;
    if(mIns == nullptr) {
        mIns = new FAInHook();
    }
    return mIns;
}

FAInHook::~FAInHook() {

}
