if(MSVC)
    add_definitions(-DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${ZLIB_INCLUDE_DIR}
    ${XERCESC_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
)

set(SurfaceTools_LIBS
    FreeCADApp
    Part
)

SET(SurfaceTools_SRCS
    AppSurfaceTools.cpp
    AppSurfaceToolsPy.cpp
    PreCompiled.cpp
    PreCompiled.h
    FeatureFilling.h
    FeatureFilling.cpp
    FeatureSewing.h
    FeatureSewing.cpp
    FeatureCut.h
    FeatureCut.cpp
    FeatureBezSurf.h
    FeatureBezSurf.cpp
    FeatureBSplineSurf.h
    FeatureBSplineSurf.cpp
)

link_directories(${OCC_LIBRARY_DIR})
add_library(SurfaceTools SHARED ${SurfaceTools_SRCS})
target_link_libraries(SurfaceTools ${SurfaceTools_LIBS})


fc_target_copy_resource(SurfaceTools 
    ${CMAKE_SOURCE_DIR}/src/Mod/SurfaceTools
    ${CMAKE_BINARY_DIR}/Mod/SurfaceTools
    Init.py)

SET_BIN_DIR(SurfaceTools SurfaceTools /Mod/SurfaceTools)
SET_PYTHON_PREFIX_SUFFIX(SurfaceTools)

install(TARGETS SurfaceTools DESTINATION ${CMAKE_INSTALL_LIBDIR})
