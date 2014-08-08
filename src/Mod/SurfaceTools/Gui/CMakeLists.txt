if(MSVC)
    add_definitions(-DHAVE_ACOSH -DHAVE_ATANH -DHAVE_ASINH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${COIN3D_INCLUDE_DIR}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${SOQT_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
)

set(SurfaceToolsGui_LIBS
    SurfaceTools
    FreeCADGui
)

qt4_add_resources(SurfaceTools_QRC_SRCS Resources/SurfaceTools.qrc)

SET(SurfaceToolsGui_SRCS
    ${SurfaceTools_QRC_SRCS}
    AppSurfaceToolsGui.cpp
    AppSurfaceToolsGuiPy.cpp
    Command.cpp
    PreCompiled.cpp
    PreCompiled.h
    Workbench.cpp
    Workbench.h
#    ViewProviderCut.cpp
#    ViewProviderCut.h
)

link_directories(${OCC_LIBRARY_DIR})
add_library(SurfaceToolsGui SHARED ${SurfaceToolsGui_SRCS})
target_link_libraries(SurfaceToolsGui ${SurfaceToolsGui_LIBS})

fc_target_copy_resource(SurfaceToolsGui 
    ${CMAKE_SOURCE_DIR}/src/Mod/SurfaceTools
    ${CMAKE_BINARY_DIR}/Mod/SurfaceTools
    InitGui.py)

SET_BIN_DIR(SurfaceToolsGui SurfaceToolsGui /Mod/SurfaceTools)
SET_PYTHON_PREFIX_SUFFIX(SurfaceToolsGui)
    
install(TARGETS SurfaceToolsGui DESTINATION ${CMAKE_INSTALL_LIBDIR})
