# Include the directory itself as a path to include directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# The file(GLOB...) allows for wildcard additions:
FILE(GLOB_RECURSE app_sources *.cpp)
LIST(SORT app_sources)

# Create an executable file called helloworld from sources:
#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
ADD_EXECUTABLE(main ${app_sources})

IF(UNIX)
  TARGET_LINK_LIBRARIES(main m)
ENDIF(UNIX)
