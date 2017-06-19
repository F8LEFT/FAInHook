set(ProjectName FAInHook)

aux_source_directory(instruction INSTRUCTION)

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
