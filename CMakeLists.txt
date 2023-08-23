cmake_minimum_required(VERSION 3.10)
project(ECC C)

set(CMAKE_C_STANDARD 11)

# Set source and output directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(OUT_DIR ${CMAKE_SOURCE_DIR}/out)

# Collect source files
file(GLOB SRCS "${SRC_DIR}/*.c")
file(GLOB PARSER_SRCS "${SRC_DIR}/parser/*.c")
file(GLOB LIB_SRCS "${SRC_DIR}/lib/*.c")

# Build the executable
add_executable(ECC ${SRCS} ${LIB_SRCS} ${PARSER_SRCS})

# Set output directory for the executable
set_target_properties(ECC PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUT_DIR}
)

# Add a custom target to run tests
add_custom_target(run-tests
        COMMAND ${CMAKE_SOURCE_DIR}/test.sh
        DEPENDS ECC
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)