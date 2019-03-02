cmake_minimum_required(VERSION 3.4.3)
project(OgreCrowd)
find_package(OGRE 1.11 REQUIRED)

option(BUILD_C_DEMO_FOR_PY "build c demo for python" TRUE)

include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/recastnaviation/DebugUtils/Include/
    ${PROJECT_SOURCE_DIR}/recastnaviation/Detour/Include/
    ${PROJECT_SOURCE_DIR}/recastnaviation/DetourCrowd/Include/
    ${PROJECT_SOURCE_DIR}/recastnaviation/DetourTileCache/Include/
    ${PROJECT_SOURCE_DIR}/recastnaviation/Recast/Include/
    ${PROJECT_SOURCE_DIR}/recastnaviation/RecastDemo/Contrib/fastlz/
    ${OGRE_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/demo/
)
link_directories(${OGRE_LIBRARY_DIRS})

file(GLOB ogrecrowdcpp "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB debugutilscpp "${PROJECT_SOURCE_DIR}/recastnaviation/DebugUtils/Source/*.cpp")
file(GLOB detourcpp "${PROJECT_SOURCE_DIR}/recastnaviation/Detour/Source/*.cpp")
file(GLOB detourcrowdcpp "${PROJECT_SOURCE_DIR}/recastnaviation/DetourCrowd/Source/*.cpp")
file(GLOB detourtilecachecpp "${PROJECT_SOURCE_DIR}/recastnaviation/DetourTileCache/Source/*.cpp")
file(GLOB recastcpp "${PROJECT_SOURCE_DIR}/recastnaviation/Recast/Source/*.cpp")
file(GLOB fastlzcpp "${PROJECT_SOURCE_DIR}/recastnaviation/RecastDemo/Contrib/fastlz/*.c")


add_library(OgreCrowdlib ${ogrecrowdcpp} ${debugutils} ${detourcpp} ${detourcrowdcpp} ${detourtilecachecpp} ${recastcpp} ${fastlzcpp})
target_link_libraries(OgreCrowdlib ${OGRE_LIBRARIES})

INSTALL(TARGETS OgreCrowdlib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)


if(BUILD_C_DEMO_FOR_PY)
    add_executable(OgreCrowdTest1 pydemo/c/main.cpp)
    target_link_libraries(OgreCrowdTest1 OgreCrowdlib)
    INSTALL(TARGETS OgreCrowdTest1
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib)
endif()

