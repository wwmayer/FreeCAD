macro(SetupPython)
# -------------------------------- Python --------------------------------

    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

    if (${Python3_VERSION} VERSION_LESS "3.8")
         message(FATAL_ERROR "To build FreeCAD you need at least Python 3.8\n")
    endif()

endmacro(SetupPython)
