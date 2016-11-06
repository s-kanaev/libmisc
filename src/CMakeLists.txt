
include_directories(../include)

add_library(containers SHARED containers.c
                              avl-tree.c
                              hash-map.c
                              hash-functions.c
                              set.c)
