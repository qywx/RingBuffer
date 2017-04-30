
################################
# GTest
################################
add_subdirectory( googletest )
enable_testing()
include_directories( ${googletest_SOURCE_DIR}/include
                     ${googletest_SOURCE_DIR} 
                     )

################################
# Unit Tests
################################
# Add test cpp file
set( TEST_FILES testgtest.cpp )
add_executable( runUnitTests ${TEST_FILES} )
## Link test executable against gtest & gtest_main
target_link_libraries( runUnitTests gtest gtest_main )
#add_test( NAME runUnitTests COMMAND runUnitTests )



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
