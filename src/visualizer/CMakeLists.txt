set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(SFML COMPONENTS audio)

if(NOT OPENGL_FOUND OR NOT OPENGL_GLU_FOUND)
    message(FATAL_ERROR "OpenGL or GLU not found")
endif()

add_executable(visual_sort)

target_compile_features(visual_sort PRIVATE cxx_std_17)

target_include_directories(visual_sort PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_sources(
    visual_sort
    PRIVATE main.cpp
            args.cpp
            sort_visualizer.cpp
            rendering/tone.cpp
            rendering/utils.cpp
            glut/window.cpp
)

target_link_libraries(
    visual_sort
    PRIVATE sort_algorithms
            Threads::Threads
            GLUT::GLUT
            OpenGL::GL
            OpenGL::GLU
            Boost::boost
            Boost::program_options
)

if(SFML_AUDIO_FOUND)
    target_link_libraries(visual_sort PRIVATE sfml-audio)
    target_compile_definitions(visual_sort PRIVATE WITH_SFML)
endif()
