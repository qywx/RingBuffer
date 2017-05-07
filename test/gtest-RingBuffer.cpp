
#include <string>
#include <iostream>
#include <hstring.h>
#include "gtest/gtest.h"
#include "RingBuffer.hpp"


using namespace std;
using namespace Sledge;


namespace Sledge {

	//TODO
	// It's important that the << operator is defined in the SAME
	// namespace that defines Bar.  C++'s look-up rules rely on that.
	template< typename T, typename A = std::allocator<T>, typename IndexT_ = signed int > 
	::std::ostream& operator<<(::std::ostream& os, const RingBuffer<T,A,IndexT_>& rb)
	{
		return os;  //<< "idx_max: " << ri.get_max() << ", idx:" << ri.get();
	}
	
}


namespace {
	
	/*// The fixture for testing class Foo.
	class RingIndexTest : public ::testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is empty.
		
		RingIndexTest()
		{
			// You can do set-up work for each test here.
		}
		
		virtual ~RingIndexTest()
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
	 
	Use TEST_F( RingIndexTest, test_name ){}
	*/


	// Tests that the Foo::Bar() method does Abc.
	TEST( RingBufferTest,  Simple )
	{
		constexpr unsigned TEST_SIZE = 256;
		RingBuffer<uint16_t> rb(TEST_SIZE);
		for( int i=0; i<TEST_SIZE ; ++i )
			EXPECT_EQ( rb.push(i), true );
		EXPECT_EQ( rb.length(), TEST_SIZE);
		EXPECT_EQ( rb.push(0), false );  // buffer overflow. failed to push
		for( int i=0; i<TEST_SIZE ; ++i )
			EXPECT_EQ( rb[i], i );
		for( int i=0; i<TEST_SIZE ; ++i ) {
			auto v = rb.pop(); 
			EXPECT_EQ( v, i);
		}
		EXPECT_TRUE( rb.isEmpty() );
	}
	
	
	int main(int argc, char *argv[])
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	
}