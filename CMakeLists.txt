cmake_minimum_required( VERSION 3.6 )
project( RingBuffer )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

include_directories( ./ )
#add_subdirectory( src )
add_subdirectory( test )
