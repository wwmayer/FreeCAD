set(occ_gordon_dir
    ${CMAKE_SOURCE_DIR}/src/3rdParty/occ_gordon/src
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(occ_gordon_internal STATIC
    ${occ_gordon_dir}/internal/ApproxResult.h
    ${occ_gordon_dir}/internal/BSplineAlgorithms.cpp
    ${occ_gordon_dir}/internal/BSplineAlgorithms.h
    ${occ_gordon_dir}/internal/BSplineApproxInterp.cpp
    ${occ_gordon_dir}/internal/BSplineApproxInterp.h
    ${occ_gordon_dir}/internal/CurveNetworkSorter.cpp
    ${occ_gordon_dir}/internal/CurveNetworkSorter.h
    ${occ_gordon_dir}/internal/CurvesToSurface.cpp
    ${occ_gordon_dir}/internal/CurvesToSurface.h
    ${occ_gordon_dir}/internal/GordonSurfaceBuilder.cpp
    ${occ_gordon_dir}/internal/GordonSurfaceBuilder.h
    ${occ_gordon_dir}/internal/InterpolateCurveNetwork.cpp
    ${occ_gordon_dir}/internal/InterpolateCurveNetwork.h
    ${occ_gordon_dir}/internal/IntersectBSplines.cpp
    ${occ_gordon_dir}/internal/IntersectBSplines.h
    ${occ_gordon_dir}/internal/IntersectionPoint.h
    ${occ_gordon_dir}/internal/PointsToBSplineInterpolation.cpp
    ${occ_gordon_dir}/internal/PointsToBSplineInterpolation.h
    ${occ_gordon_dir}/internal/occ_gordon_internal.h
    ${occ_gordon_dir}/internal/occ_std_adapters.h
    ${occ_gordon_dir}/internal/Error.cpp
    ${occ_gordon_dir}/internal/Error.h
    ${occ_gordon_dir}/occ_gordon/occ_gordon.cpp
    ${occ_gordon_dir}/occ_gordon/occ_gordon.h
)

target_include_directories(occ_gordon_internal
    PRIVATE
        ${occ_gordon_dir}/internal
        ${CMAKE_CURRENT_BINARY_DIR}
        ${occ_gordon_dir}
)

target_include_directories(occ_gordon_internal
    SYSTEM
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src/3rdParty/occ
        ${OCC_INCLUDE_DIR}
)

include(GenerateExportHeader)
generate_export_header(occ_gordon_internal
    EXPORT_MACRO_NAME OCC_GORDON_EXPORT
    EXPORT_FILE_NAME occ_gordon/exports.h
)

### -----------------------------------------------------------------------------

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Blending)

generate_from_xml(Blending/BlendPointPy)
generate_from_xml(Blending/BlendCurvePy)

set(Surface_LIBS
  FreeCADApp
  Part
)

# BlendPoint Wrapper
SET(BlendingPy_SRCS
  Blending/BlendPointPy.xml
  Blending/BlendPointPyImp.cpp
  Blending/BlendCurvePy.xml
  Blending/BlendCurvePyImp.cpp
)
SOURCE_GROUP("Blending" FILES ${BlendingPy_SRCS})

SET(Blending_SRCS
  Blending/FeatureBlendCurve.cpp
  Blending/FeatureBlendCurve.h
  Blending/BlendPoint.cpp
  Blending/BlendPoint.h
  Blending/BlendCurve.cpp
  Blending/BlendCurve.h
)

SET(Gordon_SRCS
  Gordon/FeatureGordonSurface.cpp
  Gordon/FeatureGordonSurface.h
)

SET(Surface_SRCS
  ${Blending_SRCS}
  ${BlendingPy_SRCS}
  ${Gordon_SRCS}
  AppSurface.cpp
  PreCompiled.cpp
  PreCompiled.h
  FeatureExtend.cpp
  FeatureExtend.h
  FeatureGeomFillSurface.cpp
  FeatureGeomFillSurface.h
  FeatureFilling.cpp
  FeatureFilling.h
  FeatureSections.cpp
  FeatureSections.h
  FeatureSewing.cpp
  FeatureSewing.h
  FeatureCut.cpp
  FeatureCut.h
  Measure.cpp
  Measure.h
)

if(FREECAD_USE_PCH)
    add_definitions(-D_PreComp_)
    GET_MSVC_PRECOMPILED_SOURCE("PreCompiled.cpp" PCH_SRCS ${Surface_SRCS})
    ADD_MSVC_PRECOMPILED_HEADER(Surface PreCompiled.h PreCompiled.cpp PCH_SRCS)
endif(FREECAD_USE_PCH)

add_library(Surface SHARED ${Surface_SRCS})
target_include_directories(
    Surface
    PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(
    Surface
    SYSTEM
    PUBLIC
    ${FREETYPE_INCLUDE_DIRS}
)

target_link_directories(Surface PUBLIC ${OCC_LIBRARY_DIR})
target_link_libraries(Surface ${Surface_LIBS} occ_gordon_internal)
if (FREECAD_WARN_ERROR)
    target_compile_warn_error(Surface)
endif()

SET_BIN_DIR(Surface Surface /Mod/Surface)
SET_PYTHON_PREFIX_SUFFIX(Surface)

install(TARGETS Surface DESTINATION ${CMAKE_INSTALL_LIBDIR})
