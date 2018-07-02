append_list(PROJECT_SRC
    ${_SRC_ROOT}/mem/dlmalloc.c
)

add_definitions(-DUSE_DLMALLOC)
