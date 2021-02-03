# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

# project name
project(create-dictionary-cmake)

# set (CMAKE_C_COMPILER             "/usr/bin/clang")
set (CMAKE_C_COMPILER             "/usr/bin/gcc")
set (CMAKE_C_FLAGS                "-Wall -std=c11")
set (CMAKE_C_FLAGS_DEBUG          "-g")
set (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

# set (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
set (CMAKE_CXX_COMPILER             "/usr/bin/c++")
set (CMAKE_CXX_FLAGS                "-Wall -pthread")
# set (CMAKE_CXX_FLAGS_DEBUG          "-g -fsanitize=address")
set (CMAKE_CXX_FLAGS_DEBUG          "-g")
set (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_LINKER_FLAGS             "")
set (CMAKE_LINKER_FLAGS_DEBUG       "-fsanitize=address")

set (CMAKE_AR      "/usr/bin/llvm-ar")
set (CMAKE_LINKER  "/usr/bin/llvm-ld")
set (CMAKE_NM      "/usr/bin/llvm-nm")
set (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
set (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")

set (LLVM_ENABLE_THREADS "yes")

# Enable threads
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

# Enable unicode
find_package(ICU COMPONENTS io i18n uc REQUIRED)
include_directories(${ICU_INCLUDE_DIRS})

# creates the variable EXEC and sets it to hello
set(EXEC create-dictionary)

# set the C++14 standard
set(CMAKE_CXX_STANDARD 14)

# I../includes
include_directories(includes)

# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES src/*.cpp)

# compiles the files defined by SOURCES to generante the executable defined by EXEC
add_executable(${EXEC} ${SOURCES})
target_link_libraries(${EXEC} ${CMAKE_THREAD_LIBS_INIT} ${ICU_LIBRARIES})
