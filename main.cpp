

#ifndef FA_STATIC

#include <jni.h>
#include "MinAndroidDef.h"

void test();

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{

    JNIEnv *env = nullptr;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        FLOGE(This jni version is not supported);
        return -1;
    }


    FLOGD(FAGotHook.so load success);
    FLOGD(current JNI Version %d, JNI_VERSION_1_6);
    test();
    return JNI_VERSION_1_6;
}

#include <dlfcn.h>
#include <stdio.h>
#include "FAGotHook.h"

void myFopen(const char * __restrict r1, const char * __restrict r2) {
    FLOGD(My Fopen has been invoked %s %s, r1, r2);
}

void test() {
    auto libc = dlopen("libc.so", RTLD_NOW);
    auto pfopen = dlsym(libc, "fopen");

    FAGotHook::Config cfg;
    cfg.check_ehdr = false;
    cfg.unprotect_got_memory = true;
    cfg.with_local_func = true;

    // so name from /proc/self/maps
    // FAGotHook will try to parse so data from memoy.
    FAGotHook faGotHook("libFAGotHook.so", &cfg);
    if(faGotHook.is_valid()) {
        faGotHook.rebindFunc((Elf_Addr) pfopen, (Elf_Addr) myFopen);
    }

    fopen("/data/data/f8left.fagothook/cachefile", "r");

}


#endif