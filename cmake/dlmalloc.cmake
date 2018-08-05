append_list(PROJECT_SRC
    ${_SRC_ROOT}/mem/dlmalloc.c
    ${_SRC_ROOT}/mem/sbrk.c
)

add_definitions(-DUSE_DLMALLOC)
