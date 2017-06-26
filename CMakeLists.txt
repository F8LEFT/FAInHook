set(ProjectName FAInHook)

if(ANDROID_ABI MATCHES "armeabi(-v7a)?")
    set(INSTRUCTION instruction/Instruction.cpp
                    instruction/ThumbInstruction.cpp
                    instruction/ArmInstruction.cpp)
elseif(ANDROID_ABI STREQUAL "arm64-v8a")
    set(INSTRUCTION instruction/Instruction.cpp
                    instruction/Arm64Instruction.cpp)
elseif(ANDROID_ABI MATCHES "x86(_64)?")
    set(INSTRUCTION instruction/Instruction.cpp
                    instruction/IntelInstruction.cpp
                    instruction/IntelDisasm.cpp)
elseif(ANDROID_ABI STREQUAL "mips")
    set(INSTRUCTION instruction/Instruction.cpp
                        instruction/MipsInstruction.cpp)
elseif(ANDROID_ABI STREQUAL "mips64")
    set(INSTRUCTION instruction/Instruction.cpp
                            instruction/Mips64Instruction.cpp)
endif()

set(SOURCE FAInHook.cpp MemHelper.cpp ${INSTRUCTION})

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(FA_EXTLIB log)
endif()

if(FA_STATIC)
    add_definitions(-DFA_STATIC)
    add_library( ${ProjectName}
             STATIC
             ${SOURCE})
else()
    add_library( ${ProjectName}
             SHARED
             ${SOURCE} main.cpp)
endif()

target_link_libraries( ${ProjectName}
                       ${FA_EXTLIB} dl)
