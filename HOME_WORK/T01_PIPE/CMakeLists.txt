
cmake_minimum_required( VERSION 3.11 )
project( Pipe VERSION 0.1 LANGUAGES C )

set( EXEC_NAME "pipe" )
add_executable( ${EXEC_NAME} )

target_include_directories( ${EXEC_NAME} PRIVATE "${CMAKE_HOME_DIRECTORY}/headers" )
target_sources( ${EXEC_NAME} PRIVATE
"${CMAKE_HOME_DIRECTORY}/source/errors.c"
"${CMAKE_HOME_DIRECTORY}/source/pipe.c"
)

target_compile_options( ${EXEC_NAME} PRIVATE
-O1 -g -Wall -Werror -Wextra -Wpedantic -Wunused -Wunreachable-code
)

target_compile_features( ${EXEC_NAME} PRIVATE c_std_11 )