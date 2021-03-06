#=================================================================
# CMake options ==================================================
#=================================================================
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)
project(BangThirdParty C CXX)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Variables ======================================================
#=================================================================
# General
set(NUM_COMPILE_THREADS 4)
add_definitions(-DBUILD_STATIC)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMakeIncludes")

# Assimp
set(ASSIMP_DIR "${CMAKE_CURRENT_LIST_DIR}/assimp")
set(ASSIMP_INCLUDE_DIR_0 "${ASSIMP_DIR}/include")
set(ASSIMP_INCLUDE_DIR_1 "${ASSIMP_DIR}/build/include")
set(ASSIMP_LIB_OUTPUT_DIR  "${ASSIMP_DIR}/build/code")
set(ASSIMP_LIB_OUTPUT_NAME "libassimp.a")
set(ASSIMP_LIB_OUTPUT "${ASSIMP_LIB_OUTPUT_DIR}/${ASSIMP_LIB_OUTPUT_NAME}")

# IRRXML (used by Assimp)
set(IRRXML_DIR "${ASSIMP_DIR}/build/contrib/irrXML")
set(IRRXML_LIB_OUTPUT_DIR  "${IRRXML_DIR}")
set(IRRXML_LIB_OUTPUT_NAME "libIrrXML.a")
set(IRRXML_LIB_OUTPUT "${IRRXML_LIB_OUTPUT_DIR}/${IRRXML_LIB_OUTPUT_NAME}")

# Freetype2
set(FREETYPE_DIR "${CMAKE_CURRENT_LIST_DIR}/freetype-2.4.0")
set(FREETYPE_INSTALL_DIR_NAME "freetype-install")
set(FREETYPE_INSTALL_DIR "${FREETYPE_DIR}/${FREETYPE_INSTALL_DIR_NAME}")
set(FREETYPE_LIB_OUTPUT_DIR  "${FREETYPE_INSTALL_DIR}/lib")
set(FREETYPE_LIB_OUTPUT_NAME "libfreetype.a")
set(FREETYPE_LIB_OUTPUT  "${FREETYPE_LIB_OUTPUT_DIR}/${FREETYPE_LIB_OUTPUT_NAME}")

# GLEW
set(GLEW_DIR "${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0")
set(GLEW_INCLUDE_DIR "${GLEW_DIR}/include")
set(GLEW_LIB_OUTPUT_DIR  "${GLEW_DIR}/lib")
set(GLEW_LIB_OUTPUT_NAME "libGLEW.a")
set(GLEW_LIB_OUTPUT  "${GLEW_LIB_OUTPUT_DIR}/${GLEW_LIB_OUTPUT_NAME}")

# JPEG
set(JPEG_DIR "${CMAKE_CURRENT_LIST_DIR}/libjpeg-turbo")
set(JPEG_INCLUDE_DIR_0 "${JPEG_DIR}")
set(JPEG_INCLUDE_DIR_1 "${JPEG_DIR}/build")
set(JPEG_LIB_OUTPUT_DIR  "${JPEG_DIR}/build")
set(JPEG_LIB_OUTPUT_NAME "libturbojpeg.a")
set(JPEG_LIB_OUTPUT  "${JPEG_LIB_OUTPUT_DIR}/${JPEG_LIB_OUTPUT_NAME}")

# PNG
set(PNG_DIR "${CMAKE_CURRENT_LIST_DIR}/libpng-1.6.34")
set(PNG_INCLUDE_DIR_0 "${PNG_DIR}")
set(PNG_INCLUDE_DIR_1 "${PNG_DIR}/build")
set(PNG_LIB_OUTPUT_DIR  "${PNG_DIR}/build")
set(PNG_LIB_OUTPUT_NAME "libpng.a")
set(PNG_LIB_OUTPUT  "${PNG_LIB_OUTPUT_DIR}/${PNG_LIB_OUTPUT_NAME}")

# OPENAL
set(OPENAL_DIR "${CMAKE_CURRENT_LIST_DIR}/openal-soft")
set(OPENAL_INCLUDE_DIR "${OPENAL_DIR}/include")
set(OPENAL_LIB_OUTPUT_DIR  "${OPENAL_DIR}/build")
set(OPENAL_LIB_OUTPUT_NAME "libopenal.a")
set(OPENAL_LIB_OUTPUT  "${OPENAL_LIB_OUTPUT_DIR}/${OPENAL_LIB_OUTPUT_NAME}")

# SDL2
set(SDL2_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2-2.0.8")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
set(SDL2_LIB_OUTPUT_DIR  "${SDL2_DIR}/build")
set(SDL2_LIB_OUTPUT_NAME "libSDL2.a")
set(SDL2_LIB_OUTPUT  "${SDL2_LIB_OUTPUT_DIR}/${SDL2_LIB_OUTPUT_NAME}")

# SDL2_ttf
set(SDL2TTF_DIR "${CMAKE_CURRENT_LIST_DIR}/SDL2_ttf-2.0.14")
set(SDL2TTF_INSTALL_DIR "${SDL2TTF_DIR}/sdl2-ttf-install")
set(SDL2TTF_INCLUDE_DIR "${SDL2TTF_DIR}")
set(SDL2TTF_LIB_OUTPUT_DIR  "${SDL2TTF_INSTALL_DIR}/lib")
set(SDL2TTF_LIB_OUTPUT_NAME "libSDL2_ttf.a")
set(SDL2TTF_LIB_OUTPUT  "${SDL2TTF_LIB_OUTPUT_DIR}/${SDL2TTF_LIB_OUTPUT_NAME}")
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Assimp Compile =================================================
#=================================================================
add_custom_command(
  OUTPUT ${ASSIMP_LIB_OUTPUT}
  COMMAND
      cd ${ASSIMP_DIR} && mkdir -p build && cd build &&
      cmake -DBUILD_SHARED_LIBS=OFF
            -DSYSTEM_IRRXML=OFF
            -DASSIMP_BUILD_ZLIB=OFF
            -DASSIMP_BUILD_TESTS=OFF
            -DASSIMP_BUILD_SAMPLES=OFF
            -DASSIMP_NO_EXPORT=OFF
            -BUILD_FRAMEWORK=OFF
            -DASSIMP_BUILD_ASSIMP_TOOLS=OFF .. &&
      make -j${NUM_COMPILE_THREADS}
      VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Freetype Compile ===============================================
#=================================================================
add_custom_command(
  OUTPUT ${FREETYPE_LIB_OUTPUT}
  COMMAND
      cd ${FREETYPE_DIR} &&
      sh autogen.sh &&
      mkdir -p ${FREETYPE_INSTALL_DIR} &&
      mkdir -p ${FREETYPE_INSTALL_DIR}/include/freetype2/freetype/internal/ &&
      ./configure --prefix=${FREETYPE_INSTALL_DIR} --with-pic && 
      make -j${NUM_COMPILE_THREADS} &&
      make install
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# GLEW Compile ===================================================
#=================================================================
add_custom_command(
  OUTPUT ${GLEW_LIB_OUTPUT}
  COMMAND
      cd ${GLEW_DIR} &&
      make -j${NUM_COMPILE_THREADS}
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# JPEG Compile ===================================================
#=================================================================
add_custom_command(
  OUTPUT ${JPEG_LIB_OUTPUT}
  COMMAND
      cd ${JPEG_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake -DBUILD_SHARED_LIBS=OFF -DWITH_SIMD=OFF -DREQUIRE_SIMD=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE .. &&
      make -j${NUM_COMPILE_THREADS}
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# PNG Compile ====================================================
#=================================================================
add_custom_command(
  OUTPUT ${PNG_LIB_OUTPUT}
  COMMAND
      cd ${PNG_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_C_FLAGS="-fPIC" .. &&
      make -j${NUM_COMPILE_THREADS}
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# OPENAL Compile =================================================
#=================================================================
add_custom_command(
  OUTPUT ${OPENAL_LIB_OUTPUT}
  COMMAND
      cd ${OPENAL_DIR} &&
      mkdir -p build &&
      cd build &&
      cmake -DBUILD_SHARED_LIBS=OFF
            -DLIBTYPE=STATIC 
            -DCMAKE_CXX_FLAGS=-fPIC
            -DCMAKE_C_FLAGS=-fPIC
            .. &&
      make -j${NUM_COMPILE_THREADS}
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# SDL2 Compile ===================================================
#=================================================================
add_custom_command(
  OUTPUT ${SDL2_LIB_OUTPUT}
  COMMAND
      cd ${SDL2_DIR} &&
      mkdir -p sdl2-install &&
      mkdir -p build &&
      cd build &&
      cmake -DSDL_STATIC_PIC=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${SDL2_DIR}/sdl2-install -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_C_FLAGS=-fPIC .. &&
      make -j${NUM_COMPILE_THREADS}
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# SDL2TTF Compile ================================================
#=================================================================
add_custom_command(
  OUTPUT ${SDL2TTF_LIB_OUTPUT}
  COMMAND
      export PATH=$ENV{PATH}:${SDL2TTF_INSTALL_DIR} &&
      cd ${SDL2TTF_DIR} &&
      mkdir -p ${SDL2TTF_INSTALL_DIR} &&
      sh autogen.sh &&
      ./configure --with-freetype-prefix=${FREETYPE_INSTALL_DIR} --prefix=${SDL2TTF_INSTALL_DIR} --enable-static=yes --with-pic &&
      make -j${NUM_COMPILE_THREADS} && 
      make install
  DEPENDS ${SDL2_LIB_OUTPUT} ${FREETYPE_LIB_OUTPUT}
  VERBATIM
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Variables ======================================================
#=================================================================
set(ALL_DEPENDENCIES_STATIC_TARGETS
    ${ASSIMP_LIB_OUTPUT}
    ${JPEG_LIB_OUTPUT}
    ${PNG_LIB_OUTPUT}
    ${OPENAL_LIB_OUTPUT}
    ${SDL2_LIB_OUTPUT}
    ${SDL2TTF_LIB_OUTPUT}
    ${SNDIO_LIB_OUTPUT}
    ${GLEW_LIB_OUTPUT}
    ${FREETYPE_LIB_OUTPUT} # Must go after sdl2ttf

)

set(ALL_DEPENDENCIES_STATIC_LIB_FILES
    ${ALL_DEPENDENCIES_STATIC_TARGETS}
    ${IRRXML_LIB_OUTPUT}
)

set(ALL_DEPENDENCIES_LIB_FILES
    ${ALL_DEPENDENCIES_LIB_FILES}
    ${ALL_DEPENDENCIES_STATIC_LIB_FILES}
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Include dirs ===================================================
#=================================================================
include_directories(
    ${GLEW_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR_0}
    ${ASSIMP_INCLUDE_DIR_1}
    ${OPENAL_INCLUDE_DIR}
    ${JPEG_INCLUDE_DIR_0}
    ${JPEG_INCLUDE_DIR_1}
    ${PNG_INCLUDE_DIR_0}
    ${PNG_INCLUDE_DIR_1}
    ${SDL2_INCLUDE_DIR}
    ${SDL2TTF_INCLUDE_DIR}
)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Add BuildThirdParty target =====================================
#=================================================================
add_custom_target(BuildThirdParty ALL DEPENDS ${ALL_DEPENDENCIES_STATIC_TARGETS})
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Check Force rebuild ============================================
#=================================================================
if (${FORCE_REBUILD}) # If force rebuild, remove all outputlibs
    foreach(OUTPUT_LIB_FILE ${ALL_DEPENDENCIES_STATIC_LIB_FILES})
        file(REMOVE ${OUTPUT_LIB_FILE})
    endforeach()
endif()
#=================================================================
#=================================================================
#=================================================================

