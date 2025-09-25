set(SRC_DIRS src)
set(INCLUDE_DIRS inc)
set(REQUIRES driver)

idf_component_register(SRC_DIRS ${SRC_DIRS}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    REQUIRES ${REQUIRES})
