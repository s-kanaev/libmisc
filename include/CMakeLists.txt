
file(GLOB headers *.h)

install(FILES ${headers}
        DESTINATION include/lib
        PERMISSIONS WORLD_READ)
