append_list(PROJECT_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(${_CMAKE_ROOT}/core_sources.cmake)
include(${_CMAKE_ROOT}/drivers_sources.cmake)

if(MEMORY_ALLOCATOR MATCHES "dlmalloc")
    include(${_CMAKE_ROOT}/dlmalloc.cmake)
endif()
