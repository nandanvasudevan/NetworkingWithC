
file(GLOB_RECURSE sources      src/*.c)

add_executable(${PROJECT_NAME}_server ${sources})

include_directories(inc)
include_directories(../)

SET(CMAKE_CC_FLAGS  "${CMAKE_CC_FLAGS} -Wall -Wextra -o0 -g")

# target_compile_options(${PROJECT_NAME}_client PUBLIC -Wall -Wextra -o3)
target_include_directories(${PROJECT_NAME}_client PUBLIC inc)