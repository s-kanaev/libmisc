
file(GLOB headers *.h)

install(FILES ${headers}
        DESTINATION include/misc
        PERMISSIONS WORLD_READ)
