//===------------------------------------------------------------*- C++ -*-===//
//
//                     Created by F8LEFT on 2017/6/9.
//                   Copyright (c) 2017. All rights reserved.
//===--------------------------------------------------------------------------
// help to calc memory info.
//===----------------------------------------------------------------------===//

#ifndef FAINHOOK_MEMHELPER_H
#define FAINHOOK_MEMHELPER_H

#include <cstdint>
#include <vector>
namespace FAHook {
    class MemHelper {
    public:
        static bool isFunctionAddr(void* addr);
        /* remove write protect*/
        static bool unProtectMemory(void* addr, uint32_t size);
        /* add write protect*/
        static bool protectMemory(void* addr, uint32_t size);
        /* get a executable memory*/
        static void* createExecMemory(uint32_t size);

        MemHelper();
        ~MemHelper();
    private:
        std::vector<void* > alloc_memory_page_;
        void* current_page = nullptr;
        uint32_t page_ptr_ = 0;

        static uint32_t page_size;
    };
}



#endif //FAINHOOK_MEMHELPER_H
