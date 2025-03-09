
file(GLOB_RECURSE sources      src/*.c)

add_executable(${PROJECT_NAME}_client ${sources})

# target_compile_options(${PROJECT_NAME}_client PUBLIC -Wall -Wextra -o3)
target_include_directories(${PROJECT_NAME}_client PUBLIC inc)