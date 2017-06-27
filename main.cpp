

#ifndef FA_STATIC

#include <jni.h>
#include "MinAndroidDef.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

void test();
void test2();

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{

    JNIEnv *env = nullptr;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        FLOGE(This jni version is not supported);
        return -1;
    }


    FLOGD(FAInHook.so load success);
    FLOGD(current JNI Version %d, JNI_VERSION_1_6);
    test();
    test2();
    return JNI_VERSION_1_6;
}

#include <dlfcn.h>
#include <stdio.h>
#include "FAInHook.h"
#define TESTCB

#ifdef TESTCB
int (*gB2)(const char* r1, const char* r2);
#endif

int b1(const char * r1, const char * r2) {
    FLOGE(b1 has been invoked %s %s, r1, r2);
#ifdef TESTCB
    gB2("This is invoke from b1", "is it in b1??");
#endif

    return 0;
}

int b2(const char* r1, const char * r2) {
    FLOGE(b2 has been invoked %s %s, r1, r2);
    return -1;
}

void test() {
    auto hook = FAInHook::instance();
#ifdef TESTCB
    hook->registerHook((Elf_Addr) b2, (Elf_Addr) b1, (Elf_Addr *) &gB2);
#else
    hook->registerHook((Elf_Addr) b2, (Elf_Addr) b1, nullptr);
#endif
    hook->hookAll();

    b2("This should be in b1", "is it true??");
    hook->unhookAll();
    b2("This should be in b2", "is it true??");
}

namespace FArt {
    int f_open(const char* name, int flags, ...);
    ssize_t f_read(int fd, void *dest, size_t request);
    int f_fstat(int fd, struct ::stat* st);
    void* f_mmap(void* start, size_t len, int prot, int flags, int fd, off_t offset);
    int f_munmap(void* des, size_t size);
    int f_mprotect(const void* mem, size_t size, int flag);
    ssize_t f_write(int fd, const void *dec, size_t request);

    int (*p_open)(const char* name, int flags, ...) = nullptr;
    ssize_t (*p_read)(int fd, void *dest, size_t request) = nullptr;
    int (*p_fstat)(int fd, struct ::stat* st) = nullptr;
    void* (*p_mmap)(void* start, size_t len, int prot, int flags, int fd, off_t offset) = nullptr;
    int (*p_munmap)(void* des, size_t size) = nullptr;
    int (*p_mprotect)(const void* mem, size_t size, int flag) = nullptr;
    ssize_t (*p_write)(int fd, const void *dec, size_t request) = nullptr;


}

void test2() {
    auto fHook = FAInHook::instance();
    auto libc = dlopen("libc.so", RTLD_NOW);
//    fHook->registerHook((Elf_Addr) dlsym(libc, "open"), (Elf_Addr)FArt::f_open, (Elf_Addr*)&FArt::p_open);
//    fHook->registerHook((Elf_Addr) dlsym(libc, "read"), (Elf_Addr)FArt::f_read, (Elf_Addr*)&FArt::p_read);
//    fHook->registerHook((Elf_Addr) dlsym(libc, "fstat"), (Elf_Addr)FArt::f_fstat, (Elf_Addr*)&FArt::p_fstat);
//    fHook->registerHook((Elf_Addr) dlsym(libc, "mmap"), (Elf_Addr)FArt::f_mmap, (Elf_Addr*)&FArt::p_mmap);
    fHook->registerHook((Elf_Addr) dlsym(libc, "munmap"), (Elf_Addr)FArt::f_munmap, (Elf_Addr*)&FArt::p_munmap);
    fHook->registerHook((Elf_Addr) dlsym(libc, "mprotect"), (Elf_Addr)FArt::f_mprotect, (Elf_Addr*)&FArt::p_mprotect);
//    fHook->registerHook((Elf_Addr) dlsym(libc, "write"), (Elf_Addr)FArt::f_write, (Elf_Addr*)&FArt::p_write);
    dlclose(libc);

    char buf[256];
//    read(-1, buf, 3);
//    fstat(-1, nullptr);
//    mmap(0, 1024, 0, 0, 0, 0);
    munmap(0, 1024);
    mprotect(0, 1024, 0);
//    write(-1, buf, 3);

    fHook->hookAll();

    open("/data/local/tmp/android_server", 'r');

}


int ::FArt::f_open(const char *name, int flags, ...) {
    mode_t mode = 0;
    if((flags & O_CREAT) != 0) {
        va_list args;
        va_start(args, flags);
        mode = static_cast<mode_t >(va_arg(args, int));
        va_end(args);
    }
    auto rel = p_open(name, flags, mode);
    FLOGE(%d = open %s %d %d, rel, name, flags, mode);
    return rel;
}

ssize_t FArt::f_read(int fd, void *dest, size_t request) {
    FLOGD(read data %d at %p:%d, fd, dest, request);
    return p_read(fd, dest, request);
}

int ::FArt::f_fstat(int fd, struct ::stat *st) {
    FLOGD(fstat file %d, fd);
    return p_fstat(fd, (struct stat *)st);
}

void *::FArt::f_mmap(void *start, size_t len, int prot, int flags, int fd,
                     off_t offset) {
    FLOGD(mmap at %p:%d for %d, start, len, fd);
    return p_mmap(start, len, prot, flags, fd, offset);
}

int ::FArt::f_munmap(void *des, size_t size) {
    FLOGD(munmap at %p:%d, des, size);
    return p_munmap(des, size);
}

int ::FArt::f_mprotect(const void *mem, size_t size, int flag) {
    FLOGD(mprotect %p:%d %d, mem, size, flag);
    return p_mprotect(mem, size, flag);
}

ssize_t FArt::f_write(int fd, const void *dec, size_t request) {
    FLOGD(write %d at %p:%d, fd, dec, request);
    return p_write(fd, dec, request);
}


#endif