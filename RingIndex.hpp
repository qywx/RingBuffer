/**
 * \author 	Ivan "Kyb Sledgehammer" Kuvaldin <i.kyb[2]ya,ru>
 * \brief   
 *			Реализация кольцевого буффера с расширенными возможностями.
 *          Похожего по функционалу на std::vector
 */

/// TODO  Перевести RingBuffer::Index_t на RingIndex
/// TODO  How to move to std::vector?
/// TODO  Iterators
 

#pragma once


#include <cstddef>
/*#include <memory>
#include <cmath>
#include <limits>
#include <stdexcept>*/


namespace Sledge {

	using std::size_t;
	
	/// RingIndex is a wrapped unsigned integer used to convert 
	/// relative index of RingBuffer to absolute index of underlaying raw data array.
	/// Max value of index is LIMIT_MAX 
	///@note read more about optimizations <http://stackoverflow.com/questions/43731203/is-the-if-statement-redundant-before-modulo-and-before-assign-operations>
	//template<typename RawType_=std::size_t>
	class RingIndex
	{
	public:
		using RawType = std::size_t;  //RawType_;
		using RawT = RawType;
		using SRrawType = signed RawType;
		
		static const RawType LIMIT_MAX = std::numeric_limits<RawType>::max();
		//static const RawType INDEX_MAX = LIMIT_MAX -1;
	
	protected:
		RawType idx_max = LIMIT_MAX;  // 0 causes undef.beh.
		RawType idx = 0;
	
	public:  // == CONSTRUCTOR ==
		RingIndex( RawType idx     = 0
				 , RawType idx_max = LIMIT_MAX )
		    : idx_max(idx_max)    // idx_max == 0 ? LIMIT_MAX : idx_max OR better throw an exception
		    , idx(idx % idx_max)  //(idx % this->idx_max)
		{
			// if( idx_max == 0 )  throw std::runtime_error("Illegal argument");  // but we won't spend resources
			/*//if( idx >= idx_max )
				this->idx %= idx_max;*/
		}
		
		RingIndex( const RingIndex& ri )
			: idx(ri.idx)
			, idx_max(ri.idx_max)
		{
			/* Do not spend time for this sanity check. We believe input `ri` is normalized. There is no legal way to set idx_max >= idx
			if( idx_max >= idx )
				this->idx %= idx_max;*/
		}
	
	public:  // == GETERS and SETTERS and CAST operators and ASSIGN OPERATORS ==
		
		/// Cast
		explicit operator RawType() const {
			return idx;
		}
		
		RawType get() const {
			return idx;
		}
		
		RingIndex& set(RawT i){
			idx = i % idx_max;
			return *this;
		}
		
		RawType get_max() const {
			return idx_max;
		}
		
		RingIndex& set_max(RawT max){
			idx_max = max;
			idx %= idx_max;
			return *this;
		}
		
		RingIndex & operator=(RawT r){
			return set(r);
		}
		
		RingIndex & operator=(RingIndex const & ri){
			idx = ri.idx;
			idx_max = ri.idx_max;
			return *this;
		}
		
	public:  // == ARITHMETIC OPERATORS ==
		/// Pre-increment
		RingIndex& operator++(){
			if( ++idx >= idx_max )  
				idx = 0;  //idx = ++idx % idx_max;
			return *this;
		}
		
		/// Post-increment. Create copy to return non-incremented.
		RingIndex operator++(int){
			auto copy = *this;   //auto copy = RingIndex(idx_max,idx);  // 
			this->operator++();  //ToDo check vs simple void function without `return statement`
			return copy;
		}
		
		/// Pre-decrement
		RingIndex& operator--(){
			/*idx = idx == 0
			      ? idx_max -1
			      : idx-1;*/
			if( idx == 0 )
				idx = idx_max -1;
			else
				--idx;
			return *this;
		}
		
		/// Post-decrement. Create copy to return non-Increment.
		RingIndex operator--(int){
			auto copy = *this;  //auto copy = RingIndex(idx_max,idx);
			this->operator--();  //idx = ++idx % idx_max;
			return copy;
		}
		
		RingIndex& operator+=( RawType inc )
		{
			//inc %= idx_max;
			idx += inc;
			if( idx < inc )  // overflow
				idx += LIMIT_MAX - idx_max +1;
			//if( idx >= idx_max )  // not overflowed over `LIMIT_MAX`, but over `idx_max`
				idx %= idx_max;
			/*if( inc > (LIMIT_MAX - idx) )  // would overflow
				idx += inc + (LIMIT_MAX - idx_max);
			else{
				idx += inc;
				idx %= idx_max;
			}*/
			/*if( inc <= 
				idx += inc;
			if( asm_check_overflow_flag() )
				idx -= LIMIT_MAX - idx_max;*/
			return *this;
		}
		
		
		RingIndex & operator-=( RawType dec ) 
		{
			if( idx < dec )  // would underflow
				idx = idx_max - (dec-idx);
			else
				idx -= dec;
			return *this;
		}
		
		RingIndex operator+( RawType inc ) const
		{
			RingIndex ret = *this;
			ret += inc;
			return ret;
		}
		
		RingIndex operator-( RawType dec ) const
		{
			RingIndex ret = *this;
			ret -= dec;
			return ret;
		}
		
	public:  // == COMPARISON OPERATORS ==
		
		bool operator==(RingIndex const &ri) const{
			return idx == ri.idx;
		}
		
		bool operator!=(RingIndex const &ri) const{
			return idx != ri.idx;
		}
		
		bool operator<(RingIndex const &ri) const {
			return idx < ri.idx;
		}
		
		bool operator>(RingIndex const &ri) const {
			return idx > ri.idx;
		}
		
		bool operator<=(RingIndex const &ri) const {
			return idx <= ri.idx;
		}
		
		bool operator>=(RingIndex const &ri) const {
			return idx >= ri.idx;
		}
		
		bool operator==(RawT i) const {
			return idx == i;
		}
		
		bool operator!=(RawT i) const {
			return idx != i;
		}
		
		bool operator<(RawT i) const {
			return idx < i;
		}
		
		bool operator>(RawT i) const {
			return idx > i;
		}
		
		bool operator<=(RawT i) const {
			return idx <= i;
		}
		
		bool operator>=(RawT i) const {
			return idx >= i;
		}
	};
		

}  // namespace Sledge
