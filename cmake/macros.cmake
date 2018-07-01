macro(append_list arg)
    foreach(f ${ARGN})
        set(${arg} ${${arg}} ${f} CACHE INTERNAL "" FORCE)
    endforeach()
endmacro(append_list)

macro(append arg value)
    set(${arg} "${${arg}} ${value}")
endmacro(append)
