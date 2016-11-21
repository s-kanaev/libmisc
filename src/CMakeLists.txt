
include_directories(../include)

add_library(containers SHARED containers.c
                              avl-tree.c
                              hash-map.c
                              hash-functions.c
                              set.c)

add_library(io-service SHARED io-service.c)
target_link_libraries(io-service containers)

add_library(coroutine SHARED coroutine.c)
target_link_libraries(coroutine containers)

set_target_properties(containers PROPERTIES
                      VERSION 0.0.1
                      SOVERSION 0)
set_target_properties(io-service PROPERTIES
                      VERSION 0.0.1
                      SOVERSION 0)
set_target_properties(coroutine PROPERTIES
                      VERSION 0.0.1
                      SOVERSION 0)

install(TARGETS containers io-service coroutine
        LIBRARY DESTINATION lib/misc)
