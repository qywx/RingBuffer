
################################
# GTest
################################
add_subdirectory( googletest )
include_directories( ${googletest_SOURCE_DIR}/include
                     ${googletest_SOURCE_DIR} 
                     )

enable_testing()

################################
###         Unit Tests       ###
################################
## Add test cpp file
add_executable( gtest-RingIndex 
                gtest-RingIndex.cpp )
add_executable( gtest-RingBuffer
                gtest-RingBuffer.cpp )
## Link test executable against gtest & gtest_main
target_link_libraries(  gtest-RingIndex
                        gtest gtest_main )
target_link_libraries(  gtest-RingBuffer
                        gtest gtest_main )
add_test( NAME gtest-RingIndex 
          COMMAND gtest-RingIndex )
add_test( NAME gtest-RingBuffer
          COMMAND gtest-RingBuffer )

########################################
# Boost::Test
###
#find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
#include_directories (${TEST_SOURCE_DIR}/src
#                     ${Boost_INCLUDE_DIRS}
#                     )
#add_definitions (-DBOOST_TEST_DYN_LINK)
#add_executable (Test test.cpp)
#target_link_libraries (Test
#                       Sqr
#                       ${Boost_FILESYSTEM_LIBRARY}
#                       ${Boost_SYSTEM_LIBRARY}
#                       ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
#                       )
