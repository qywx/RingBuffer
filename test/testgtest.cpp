
#include <string>
#include <iostream>
#include <hstring.h>
#include "gtest/gtest.h"
#include "RingBuffer.hpp"


using namespace std;
using namespace Sledge;


namespace Sledge {

	// It's important that the << operator is defined in the SAME
	// namespace that defines Bar.  C++'s look-up rules rely on that.
	::std::ostream& operator<<(::std::ostream& os, const RingIndex& ri)
	{
		//return os << bar.toString();  // whatever needed to print bar to os
		return os << "idx_max: " << ri.get_max() << ", idx:" << ri.get();
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
	TEST( RingIndexTest,  Simple )
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

	// Tests that the Foo::Bar() method does Abc.
	TEST( RingIndexTest, RingIndex1)
	{
		RingIndex ri(22, 23);
		EXPECT_EQ(++ri, 0);
		EXPECT_EQ(ri + 23, 0);
		EXPECT_EQ(ri + 24, 1);
		ri += 5;
		EXPECT_EQ(ri, 5);
		EXPECT_EQ(ri + RingIndex::LIMIT_MAX, ri + (RingIndex::LIMIT_MAX % ri.get_max()));
	}
	
	/// Tests increment and decrement operations
	TEST( RingIndexTest, RingIndex2)
	{
		RingIndex ri2;
		EXPECT_EQ( ri2++, 0 );
		EXPECT_EQ( ++ri2, 2 );
		EXPECT_EQ( ri2-2, 0 );
		EXPECT_EQ( ri2-=2, 0 );
		EXPECT_EQ( --ri2, ri2.get_max()-1 );
		EXPECT_EQ( ri2+1, 0 );
		EXPECT_EQ( ri2-5, ri2.get_max()-6 );
	}
	
	TEST( RingIndexTest, RingIndexF)
	{
		RingIndex ri(0,256);
		ASSERT_EQ( ri + ri.get_max(), ri);
		
		auto max = RingIndex::LIMIT_MAX / 2;
		ri.set_max(max);
		ri.set( max / 200 );  // value much less than max 
		ASSERT_EQ( ri + (max + 22), ri.get() + 22 );
		ASSERT_EQ( ri + (max + 25662), ri.get() + 25662 );
		ASSERT_EQ( ri + 453, ri.get() + 453 );  // no overflow
		
		ri.set_max( RingIndex::LIMIT_MAX - 211 );
		ri = 0; 
		ri += RingIndex::LIMIT_MAX;
		ASSERT_EQ( ri, RingIndex::LIMIT_MAX % ri.get_max() );
		
		ri.set( ri.get_max() );
		ASSERT_EQ( ri, 0 );
	}
	
	
	/// When result overflows ( value + increment ) > LIMIT_MAX 
	TEST( RingIndexTest, RingIndex_OverflowSum)
	{
		RingIndex ri;
		ri.set_max( RingIndex::LIMIT_MAX - 7 );
		ri = RingIndex::LIMIT_MAX - 9;
		ri += 15;
		ASSERT_EQ( ri, 13 );  // 15 - (9-7)
		
		ri.set_max( RingIndex::LIMIT_MAX - 777 );
		ri = RingIndex::LIMIT_MAX - 9999;
		ri += 55555;
		ASSERT_EQ( ri, 55555 - (9999-777) );  // 
	}
	
	//todo
	TEST( RingIndexTest, RingIndex_UnderflowSub)
	{
		
	}
	
	
	
	int main(int argc, char *argv[])
	{
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	
}