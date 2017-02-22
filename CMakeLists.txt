cmake_minimum_required(VERSION 3.5)
project(Dicta)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

# List of Header files (.h, .hh, .hpp)
set(HEADER_FILES
        include/audio/AudioHandler.h
        include/audio/SoundIoException.h)

# List of Source files (.c, .cc, .cpp)
set(SOURCE_FILES
        src/main.cpp
        src/audio/AudioHandler.cpp
        )

# Include Projet cmake scripts (Mostly used to find dependencies libraries on the system)
set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_SOURCE_DIR}/cmake
        )

# Execute each dependency find_cmake script
find_package(SoundIo REQUIRED)

# Include the dependencies header files to be compiled
if (SOUNDIO_FOUND)
    include_directories(
            ${SOUNDIO_INCLUDE_DIR}
    )

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    if(Threads_FOUND)
        # Specify which project's files will be compiled
        add_executable(${PROJECT_NAME}
                ${HEADER_FILES}
                ${SOURCE_FILES}
                )

        # Link the dependencies libs
        target_link_libraries(
                ${PROJECT_NAME}
                ${SOUNDIO_LIBRARY}
                Threads::Threads
        )
    endif(Threads_FOUND)

endif (SOUNDIO_FOUND)

