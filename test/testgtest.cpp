
#include <string>
#include <iostream>
#include <hstring.h>
#include "gtest/gtest.h"
#include "RingBuffer.hpp"


using namespace std;
using namespace Sledge;

namespace {
	
	// The fixture for testing class Foo.
	class ParameterTest : public ::testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is empty.
		
		ParameterTest()
		{
			// You can do set-up work for each test here.
		}
		
		virtual ~ParameterTest()
		{
			// You can do clean-up work that doesn't throw exceptions here.
		}
		
		// If the constructor and destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:
		
		virtual void SetUp()
		{
			// Code here will be called immediately after the constructor (right
			// before each test).
		}
		
		virtual void TearDown()
		{
			// Code here will be called immediately after each test (right
			// before the destructor).
		}
		
		// Objects declared here can be used by all tests in the test case for Foo.
	};


	// Tests that the Foo::Bar() method does Abc.
	TEST_F(ParameterTest, Simple)
	{
		RingBuffer<uint16_t> rb(256);
		for( int i=0; i<256 ; ++i )
			rb.push()
	}

	// Tests that the Foo::Bar() method does Abc.
	TEST_F(ParameterTest, Simple2)
	{
		
	}
	
	
	
	int main(int argc, char **argv)
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	
}