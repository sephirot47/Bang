#=================================================================
# CMake options ==================================================
#=================================================================
cmake_minimum_required(VERSION 2.8)
#=================================================================
#=================================================================
#=================================================================

#=================================================================
# Include guard ==================================================
#=================================================================
if (DEFINED BANG_DEPENDENCIES_BUILT)
    return()
endif()
set(BANG_DEPENDENCIES_BUILT ON)
#===============================s==================================
#=================================================================
#=================================================================

#=================================================================
# Includes =======================================================
#=================================================================
include ("${CMAKE_CURRENT_LIST_DIR}/CMakeMacros.cmake")
#=================================================================
#=================================================================
#=================================================================

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

#=================================================================
# Dependencies ===================================================
#=================================================================

# Common shared dependencies ===================
    # DL Dependency =============
    message("Looking for dependency dl...")
    find_library(DL_LIBRARY "dl")

    # GL Dependency ============================
    message("Looking for dependency OpenGL...")
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})

    # ZLIB Dependency ============================
    find_package(ZLIB REQUIRED "zlib")

    # SNDFILE Dependency ============================
    message("Looking for dependency sndfile...")
    find_library(SNDFILE_LIBRARY "sndfile")
# =================================================

if (${BUILD_SHARED_LIBS})
    # Shared dependencies ===================

    # GLEW Dependency ============================
    message("Looking for dependency GLEW...")
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})

    # OpenAL Dependency ============================
    find_package(OpenAL REQUIRED)
    include_directories(${OPENAL_INCLUDE_DIRS})

    # PNG Dependency ============================
    message("Looking for dependency PNG...")
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIRS})

    # JPEG Dependency ============================
    message("Looking for dependency JPEG...")
    find_package(JPEG REQUIRED)
    include_directories(${JPEG_INCLUDE_DIRS})

    # ASSIMP Dependency ============================
    message("Looking for dependency assimp...")
    find_library(ASSIMP_LIBRARY "assimp")

    # SDL2 Dependency ============================
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2_INCLUDE_DIRS})
    string(STRIP "${SDL2_LIBRARY}" SDL2_LIBRARY)
    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
    string(REPLACE "-lpthread" "" SDL2_LIBRARY "${SDL2_LIBRARY}")

    # SDL2_TTF Dependency ============================
    find_package(SDL2_ttf REQUIRED)
    include_directories(${SDL2_TTF_INCLUDE_DIR})
    include_directories(${SDL2_TTF_INCLUDE_DIRS})
    string(STRIP "${SDL2_TTF_LIBRARY}" SDL2_TTF_LIBRARY)
    string(STRIP "${SDL2_TTF_LIBRARIES}" SDL2_TTF_LIBRARIES)

    # Set output variables
    set(ALL_DEPENDENCIES_LIB_FILES
        ${ALL_DEPENDENCIES_LIB_FILES}
        ${PNG_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${JPEG_LIBRARIES}
        ${SDL2_LIBRARY}
        ${GLEW_LIBRARIES}
        ${SDL2_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${ASSIMP_LIBRARY}
    )

else ()
    # Static dependencies ===================
    include(${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/CMakeLists.txt)
endif()

# Common libs
set(ALL_DEPENDENCIES_LIB_FILES
    ${ALL_DEPENDENCIES_LIB_FILES}
    ${DL_LIBRARY}
    ${ZLIB_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${SNDFILE_LIBRARY}
)

#=================================================================
#=================================================================
#=================================================================
