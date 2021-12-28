project(visual-sort)
cmake_minimum_required(VERSION 3.13)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

if(NOT OPENGL_FOUND OR NOT OPENGL_GLU_FOUND)
  message(FATAL_ERROR "OpenGL or GLU not found")
endif()

add_executable(visual_sort)

target_compile_features(visual_sort PRIVATE cxx_std_14)

target_include_directories(visual_sort PRIVATE src)

target_sources(
  visual_sort
  PRIVATE src/rendering/callbacks.cpp
          src/args.cpp
          src/globals.cpp
          src/main.cpp
          src/sorting/sort_handler.cpp
          src/sorting/sorts.cpp
          src/glut/window.cpp)

target_link_libraries(
  visual_sort PRIVATE Threads::Threads GLUT::GLUT OpenGL::GL OpenGL::GLU
                      Boost::boost Boost::program_options)
