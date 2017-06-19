set(ProjectName FAInHook)

set(SOURCE FAInHook.cpp)

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
