
file(GLOB headers *.h)

install(FILES ${headers}
        DESTINATION include/${thelibname}
        PERMISSIONS OWNER_READ
                    OWNER_WRITE
                    GROUP_READ
                    WORLD_READ)
