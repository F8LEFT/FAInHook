//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/9.
//                   Copyright (c) 2017. All rights reserved.
//===--------------------------------------------------------------------------
//
//===----------------------------------------------------------------------===//

#include "MemHelper.h"
#include "MinAndroidDef.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>

#define maps "/proc/self/maps"
#define MAX_BUF 128



namespace FAHook {
    uint32_t MemHelper::page_size;
    MemHelper gMemHelper;
}

bool FAHook::MemHelper::isFunctionAddr(void *addr) {
    char buf[MAX_BUF];
    auto fp = fopen(maps, "r");
    if(nullptr == fp) {
        return false;
    }

    while(fgets(buf, MAX_BUF, fp)) {
        if(strstr(buf, "r-xp") != nullptr) {
            void* startAddr = (void*)strtoul(strtok(buf, "-"), nullptr, 16);
            void* endAddr = (void*)strtoul(strtok(nullptr, " "), nullptr, 16);
            if(addr >= startAddr && addr <= endAddr) {
                fclose(fp);
                return true;
            }
        }
    }
    fclose(fp);
    FLOGE(this functionAddr is not a function!);
    return false;
}

bool FAHook::MemHelper::unProtectMemory(void *addr, uint32_t size) {
    auto align = ((size_t)addr) % page_size;
    return mprotect((uint8_t*)addr - align, size + align, PROT_READ|PROT_WRITE|PROT_EXEC) != -1;
}

bool FAHook::MemHelper::protectMemory(void *addr, uint32_t size) {
    auto align = ((size_t)addr) % page_size;
    return mprotect((uint8_t*)addr - align, size + align, PROT_READ|PROT_EXEC) != -1;
}

void *FAHook::MemHelper::createExecMemory(uint32_t size) {
    if(size & 1) {
        size ++;
    }
    if(size > page_size) {
        return nullptr;
    }
    if(gMemHelper.current_page != nullptr && page_size - gMemHelper.page_ptr_ >= size) {
        auto funPtr = (void*)((size_t)gMemHelper.current_page + gMemHelper.page_ptr_);
        gMemHelper.page_ptr_ += size;
        // Align 4
        while(gMemHelper.page_ptr_ & 0x3) {
            gMemHelper.page_ptr_ ++;
        }
        return funPtr;
    }
    // scroll to next page
    auto newPage = mmap(nullptr, page_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    if(newPage != MAP_FAILED) {
        gMemHelper.alloc_memory_page_.push_back(newPage);

        gMemHelper.current_page = newPage;
        gMemHelper.page_ptr_ = 0;
        return createExecMemory(size);
    }
    return nullptr;
}

FAHook::MemHelper::MemHelper() {
    page_size = sysconf(_SC_PAGESIZE);
}

FAHook::MemHelper::~MemHelper() {
    for(auto *page : alloc_memory_page_) {
        munmap(page, page_size);
    }
}



