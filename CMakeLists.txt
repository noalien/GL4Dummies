cmake_minimum_required(VERSION 2.6)
PROJECT(GL4dummies)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMakeModules")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(GL4DUMMIES_INCLUDE_DIRS lib_src)

if(UNIX)

  set(CMAKE_C_FLAGS "-W -Wall")
  find_package(Math REQUIRED)

elseif(APPLE)

  set(CMAKE_C_FLAGS "-W -Wall")
  find_package(Math REQUIRED)

elseif(WIN32)

  if(MINGW)
  
    set(CMAKE_C_FLAGS "-W -Wall")
    find_package(Math REQUIRED)
	
  elseif(MSVC)
  
    set(CMAKE_C_FLAGS "/W3 /EHsc /wd4996")
	
  endif()
  
endif()

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${GL4DUMMIES_INCLUDE_DIRS}
  ${MATH_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
  )

# GL4Dummies lib
file(GLOB_RECURSE gl4dummies_header_files ${GL4DUMMIES_INCLUDE_DIRS}/GL4D/*.h)
file(GLOB_RECURSE gl4dummies_source_files ${GL4DUMMIES_INCLUDE_DIRS}/GL4D/*.c)
file(GLOB_RECURSE gl4dummies_inline_files ${GL4DUMMIES_INCLUDE_DIRS}/GL4D/*.inl)
add_library(GL4Dummies SHARED 
  ${gl4dummies_header_files} ${gl4dummies_source_files} ${gl4dummies_inline_files})
set_target_properties(GL4Dummies
  PROPERTIES
  VERSION 1.0.0
  SOVERSION 1
  )
target_link_libraries(GL4Dummies
  ${MATH_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${SDL2_LIBRARIES}
  )
  
# Install
install(TARGETS GL4Dummies
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
install(FILES ${gl4dummies_header_files} ${gl4dummies_inline_files} DESTINATION include/GL4D)

# Example01
file(GLOB_RECURSE example01_source_files 
  samples/example01/*.h samples/example01/*.c)
add_executable(gl4dWalkOnPolygons ${example01_source_files})
target_link_libraries(gl4dWalkOnPolygons GL4Dummies)

# Example02
file(GLOB_RECURSE example02_source_files 
  samples/example02/*.h samples/example02/*.c)
add_executable(gl4dLandscape ${example02_source_files})
target_link_libraries(gl4dLandscape GL4Dummies)

# Example03
file(GLOB_RECURSE example03_source_files 
  samples/example03/*.h samples/example03/*.c
  samples/example03/glm/*.h samples/example03/glm/*.c)
add_executable(gl4dObjLoader ${example03_source_files})
target_link_libraries(gl4dObjLoader GL4Dummies)

# Example04
file(GLOB_RECURSE example04_source_files 
  samples/example04/*.h samples/example04/*.c)
add_executable(gl4dContainer ${example04_source_files})
target_link_libraries(gl4dContainer GL4Dummies)

# Example05
#file(GLOB_RECURSE example05_source_files 
#  samples/example05/*.h samples/example05/*.c)
#add_executable(example5 ${example05_source_files})
#target_link_libraries(example5 GL4Dummies)

# Landscape Multitexturing
file(GLOB_RECURSE landscape_multitex_source_files 
  samples/landscape_multitex/*.h samples/landscape_mutlitex/*.c)
add_executable(landscape_multitex ${landscape_multitex_source_files})
target_link_libraries(landscape_multitex GL4Dummies)

if(UNIX OR APPLE OR MINGW)
  set_target_properties(gl4dContainer PROPERTIES COMPILE_FLAGS "-std=c99")
elseif(WIN32)
  set_target_properties(gl4dWalkOnPolygons PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  set_target_properties(gl4dLandscape PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  set_target_properties(gl4dObjLoader PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  set_target_properties(gl4dContainer PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
  #set_target_properties(example5 PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
endif()