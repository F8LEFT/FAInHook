

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
#include "FAInHook.h"

int (*gB2)(const char* r1, const char* r2);

int b1(const char * r1, const char * r2) {
    FLOGD(b1 has been invoked %s %s, r1, r2);
    gB2("This is invoke from r1", "is it in b1??");
    return 0;
}

int b2(const char* r1, const char * r2) {
    FLOGE(b2 has been invoked %s %s, r1, r2);
    return -1;
}

void test() {
    auto hook = FAInHook::instance();
    hook->registerHook((Elf_Addr) b2, (Elf_Addr) b1, (Elf_Addr*)&gB2);
    hook->hookAll();

    b2("This should be in b1", "is it true??");
    hook->unhookAll();
    b2("This should be in b2", "is it true??");
}


#endif