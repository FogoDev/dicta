cmake_minimum_required(VERSION 3.5)
project(Dicta)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

# List of Header files (.h, .hh, .hpp)
set(HEADER_FILES
    include/audio/AudioHandler.h
    include/audio/SoundIoException.h
    include/preprocessor/Frame.hpp
    include/preprocessor/DFTHandler.h
    include/preprocessor/PreProcessor.h
    include/preprocessor/MFCC.hpp
    )

# List of Source files (.c, .cc, .cpp)
set(SOURCE_FILES
    src/main.cpp
    src/audio/AudioHandler.cpp
    src/preprocessor/DFTHandler.cpp
    src/preprocessor/PreProcessor.cpp
    )

# Include Projet cmake scripts (Mostly used to find dependencies libraries on the system)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake
    )

# Execute each dependency find_cmake script
find_package(SoundIo REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)
find_package(FFTW REQUIRED)

# Include the dependencies header files to be compiled
if (SOUNDIO_FOUND AND Threads_FOUND AND Boost_FOUND AND FFTW_FOUND)
    include_directories(
            ${SOUNDIO_INCLUDE_DIR}
            ${Boost_INCLUDE_DIR}
            ${FFTW_INCLUDE_DIR}
            )

    # Specify which project's files will be compiled
    add_executable(${PROJECT_NAME}
                   ${HEADER_FILES}
                   ${SOURCE_FILES}
                   )

    # Link the dependencies libs
    target_link_libraries(${PROJECT_NAME}
                          ${SOUNDIO_LIBRARY}
                          Threads::Threads
                          ${Boost_LIBRARIES}
                          ${FFTW_LIBRARIES}
                          )

endif (SOUNDIO_FOUND AND Threads_FOUND AND Boost_FOUND AND FFTW_FOUND)

