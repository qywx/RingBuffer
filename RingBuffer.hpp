/**
 * \author 	Ivan "Kyb Sledgehammer" Kuvaldin <i.kyb[2]ya,ru>
 * \brief   
 *			Реализация кольцевого буффера с расширенными возможностями.
 Похржего по функционалу на std::vector
 */
 
#pragma once


//#pragma diag_suppress 284  // Suppress null-reference is not allowed


//#include "Buffer.hpp"
//#include "Sledge/utils.h"
//#include "Sledge/assert.h"

#include <memory>
#include <cmath>
#include <cstddef>
#include <limits>


namespace Sledge {

	using std::size_t;
	
	/// Max value of index is LIMIT_MAX 
	//template<typename RawType_=std::size_t>
	class RingIndex
	{
	public:
		using RawType = std::size_t;  //RawType_;
		using SRrawType = signed RawType;
		static const RawType LIMIT_MAX = std::numeric_limits<RawType>::max();
	
	protected:
		RawType idx_max = LIMIT_MAX;  // 0 causes undef.beh.
		RawType idx = 0;
	
	public:  // == CONSTRUCTOR ==
		RingIndex(
			  RawType idx     = 0
			, RawType idx_max = LIMIT_MAX
		) :
		      idx(idx)
			, idx_max(idx_max)
		{
			if( idx_max >= idx )
				this->idx %= idx_max;
		}
		
		/*RingIndex(
				, SRawType idx     = 0
				RrawType idx_max = LIMIT_MAX
		) :
				idx_max(idx_max)
				, idx(idx)
		{}*/
	
	public:  // == METHODS ==
		/// Pre-increment
		RingIndex& operator++(){
			if( ++idx < idx_max )  idx = 0;  //idx = ++idx % idx_max;
			return *this;
		}
		
		/// Post-increment. Create copy to return non-incremented.
		RingIndex operator++(int){
			auto copy = *this;  //auto copy = RingIndex(idx_max,idx);  // 
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
			auto copy = RingIndex(idx_max,idx);
			this->operator--();  //idx = ++idx % idx_max;
			return copy;
		}
		
		RingIndex& operator+=( RawType inc )
		{
			inc %= idx_max;
			if( idx + inc < idx )  // would overflow
				idx = (idx + inc) + (LIMIT_MAX - idx_max);
			if( idx + inc >= idx_max )  // not overflowed over `LIMIT_MAX`, but over `idx_max`
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
		
		//Todo
		/*RingIndex& operator-=( Index_t dec ) const 
		{
			Index_t ret = idx - dec;
			if( ret >= bourn_ )
				ret -= bourn_;
			return *this;
		}*/
		
		/// Cast
		operator RawType(){
			return idx;
		}
		
		RawType get_max(){
			return idx_max;
		}
	};
	
	
	/**	
	 * Шаблон кольцевого буфера. Размер структуры 32 байта.
	 * принимает два параметра:
	 * @param SIZE: 	размер буфера,
	 * @param Data_t:	тип элементов хранящихся в буфере, по умолчанию unsigned char
	 */
	template<typename T, typename Allocator = std::allocator<T> >
	class RingBuffer //: private Allocator
	{
	public:  // == TYPES as `std::vector` ==
		//typedef Data_t                                   T;
		//typedef T                                        value_type;
		typedef T                                        value_type, Data_t;
		typedef Allocator                                allocator_type;
		typedef typename allocator_type::reference       reference;
		typedef typename allocator_type::const_reference const_reference;
		/*typedef implementation-defined                   iterator;
		typedef implementation-defined                   const_iterator;*/
		typedef typename allocator_type::size_type       size_type;
		typedef typename allocator_type::difference_type difference_type;
		typedef typename allocator_type::pointer         pointer;
		typedef typename allocator_type::const_pointer   const_pointer;
		//typedef std::reverse_iterator<iterator>          reverse_iterator;
		//typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;
	
	public:  // == TYPES ==
		using SizeT = size_type;
		
		/// Определяем псевдоним для индексов. int позволит использовать отрицательные индексы
		//typedef int Index_t;  //size_t
		using IndexT = volatile signed SizeT;  //SizeT

	public:  // == CONSTRUCTORS and DESTRUCTOR
		RingBuffer(Index_t size, Index_t brn/*=0*/) 
			: SIZE(size), bourn_(brn)//(brn!=0?brn:size)
		{
			_data = allocator_.allocate(size); 
		}
		RingBuffer(Index_t size) 
			: SIZE(size)//, bourn_(size)
		{
			_data = allocator_.allocate(size); 
		}
		
		// Для наследования. Разрешает полиморфизм Base *b = new Derived(); delete b;
		virtual ~RingBuffer(){
			allocator_.deallocate(_data, SIZE);
		}

	private:
		//const Index_t SIZE       = 0;
		SizeT            capacity;
		Data_t          *_data = nullptr;
		IndexT head_     = 0; 	/// количество чтений, индекс головы
		IndexT tail_     = 0; 	/// количество записей, индекс хвоста, индекс куда будет записан следующий элемент
		IndexT length_   = 0;	/// количество элементов
		IndexT bourn_    = SIZE; 	/// предельное наполнение //volatile Index_t actualSize /*= SIZE*/; /// SIZE определяет размер буффера в памяти, actualSize позволяет уменьшить текущий размер буффера
		Allocator allocator_;
		///\todo bool auto_reallocate = false;  /// Самостоятельно выделять новый кусок памяти, копируя в него весь буффер при переполнеии.	
		
	private:
		/// Cледующий индекс, изменяет значение входного аргумента.
		Index_t nextIndex( IndexT & idx) // volatile & было.
		{
			return idx = ++idx < bourn_ ? idx : 0;
		}
		
		/// Увеличение индекса на inc, изменяет значение входного параметра.
		volatile Index_t& incIndex( volatile Index_t &idx, Index_t inc ) const  // volatile & было.
		{
			idx += inc;
			if( idx >= bourn_ )
				idx -= bourn_;
			return idx;
		}
		
		/// Cледующий индекс, НЕ изменяет значение входного параметра.
		/*volatile*/ Index_t nextIndexOf( volatile Index_t idx) const 
		{
			return ++idx < bourn_ ? idx : 0;
		}
		
		/// Инкремент idx на inc, НЕ изменяет значение входного параметра.
		Index_t incIndexOf( Index_t idx, Index_t inc ) const 
		{
			Index_t ret = idx + inc;
			if( ret >= bourn_ )
				ret -= bourn_;
			return ret;
		}
		
	public:  // == Data storage METHODS ==
		
		/*TODO /// Like std:Lvector::resize()
		void resize(std::size_t siz)
		{
			if( size < siz ){
				auto newdata = allocator_.allocate(siz);
				std::memcpy( newdata, _data, size*sizeof(
			}
		};
	
		void shrink_to_data()
		*/
		
		/*ToDo
		SizeT capacity();
		*/
		
	public:
		/** Запись в буфер одного элемента
		  * \return true если значение записано, false при переполнении
		  */
		bool push(const Data_t & value) {
			if( isFull() )	
				return false;
			_data[ tail_ ] = value;
			nextIndex(tail_);
			length_++;
			return true;
		}
		
		/** Запись в буфер массива элементов. Будет записано не более чем свободно.
		  * \return Количество вставленных элементов
		  */
		Index_t push( Data_t array[], Index_t length )
		{
			Index_t i=0;
			while( i<length && push(array[i]) )
				i++;
			return i;
		}
		
		/** Запись в буфер, выталкаивая первый элемент, если буффер заполнен.
		  * Цивильный вариант с небольшим оверхэдом. Опирается на готовые функции.
		  * \return true, если буффер полон и 1ый элемент был вытолкнут; false - если не полон.
		  */
		bool pushForce( const Data_t & value) {
			bool isful = isFull();
			if( isful )	
				pop();
			push(value);
			return isful;
		}
		/*	Запись в буфер, выталкаивая первый, если буффер заполнен.
		  * Облегчённый и ускоренный вариант
		  */
		/*bool pushForce(Data_t value) {
			if( isFull() ){
				value = _data[head_]; nextIndex(head_) }
			_data[ nextIndex(tail_) ] = value;
		}*/
		
		bool pushForce( Data_t && value) {
			bool isful = isFull();
			if( isful )	
				pop();
			push(value);
			return isful;
		}
			
		/** Запись в буфер массива элементов, выталкивая все предыдущие если буффер заполнен.
		  * \return number of elements pushed
		  */
		Index_t pushForce( Data_t array[], Index_t length )
		{
			Index_t i=0;
			while( i<length ){
				pushForce( array[i++] );
			}
			return i;
		}
		
		/** Запись в буфер массива элементов, выталкивая все предыдущие если буффер заполнен.
		  * \return number of elements pushed
		  */
		Index_t pushForce( RingBuffer<Data_t> inRB, Index_t nToPush = -1 )
		{
			if( nToPush == -1 || nToPush > inRB.length() )
				nToPush = inRB.length();
			Index_t i=0;
			while( i<nToPush ){
				pushForce( inRB[i++] );
			}
			return i;
		}
		
		/** Запись в буфер прореженного массива элементов, выталкивая все предыдущие если буффер заполнен.
		  * Прореживание (subsampling) ...
		  * \param 	array 	массив элементов
		  * \param 	length 	длина массива
		  * \param 	sub 	брать из массива каждый sub-ый элемент, например, каждый 2ой
		  * \return	number 	of elements pushed
		  */
		Index_t pushForceSubsampling( const Data_t array[], Index_t length, unsigned sub )
		{
			assert_amsg( array != 0);
			Index_t i=0, pushed=0;
			while( i<length ){
				pushForce( array[i] );
				i+=sub;
				pushed++;
			}
			return pushed;
		}
		
		/**
		 * 
		 */
		/*void setAllBuf( Data_t ){
			
		}*/
		
		/** Достать элемент, удаляя из буфера, и записать в value
		  * \return Возвращает true если значение прочитано (буфер не пуст), value изменяется.
		  */
		bool pop(Data_t &value) {
			if( isEmpty() ){
				//value = 0; //Data_t();
				return false;
			}
			value = _data[head_];
			nextIndex(head_);
			length_--;
			return true;
		}
		
		/** Достать и удалить из буфера
		 *  Не рекомендуется для больших данных.
		 *  \return возвращает элемент или новый элемент.
		 */
		Data_t pop() {
			if( isEmpty() )
				return Data_t();  // throw "Nothing to pop"
			Data_t value = _data[head_];
			nextIndex(head_);
			length_--;
			return value;
		}
		
		/** Достать элементы, удаляя из буфера, и записать в массив
		  * \return Возвращает количество выплюнутых/записанных элементов
		  */
		Index_t pop(Data_t array[], size_t length) {
			if( isEmpty() ){
				return 0;
			}
			int i=0;
			while( i<length && pop(array[i++]) );
			return i;
		}
		
		/// Псевдоним для вариации pop()
		Index_t popToArray(Data_t array[], size_t length) {
			return pop(array,length);
		}
		
		///\todo
		//Index_t popToRingBuffer(RB &rb, len)
		
		///\todo оптимизировать. молча выкинуть лишнее
		/** Достать и удалить из буфера len элементов
		 *  \param len количество элементов
		 *  \return возвращает количество вытолкнутых элементов; 0, если выталкивать нечего (пустой).
		 */
		Index_t pop(size_t len) {
			Index_t i=0;
			Data_t dummy;
			while( len>0 && pop(dummy) ){ 
				len--; i++;
			};
			return i;
		}
		
		/** Вытолкнуть элементы из буфера, и записать в RingBuffer rb
		  * \param rb куда записывать
		  * \return Возвращает количество вытолкнутых/вставленных комнонентов.
		  */
		Index_t pop( RingBuffer<Data_t> &rb ){
			int i=0;
			while( !isEmpty() && rb.push(pop()) ){ i++; };
			return i;
		}
		
		/// возвращает первый элемент из буфера, не удаляя его
		Data_t& getFirst() {
			return operator[](0);
		}
		
		/// возвращает первый элемент из буфера, не удаляя его
		const Data_t& getFirst() const {
			return operator[](0);
		}
		
		/// возвращает последний элемент из буфера, не удаляя его
		Data_t& getLast() {
			return operator[](length()-1);
		}
		
		/// возвращает последний элемент из буфера, не удаляя его
		const Data_t& getLast() const {
			return operator[](length()-1);
		}
		
		/// эквивалент getLast()
		Data_t& peek() /*const*/ {
			return getLast();
		}
		
		/// 
		//Index_t copyTo( Data_t &d ) /*const*/ {
		//	abort_msg(__func__##" not implemented yet.\n");
		//	return 0;
		//}
		
		/// Copy elements `len` elements from buffer to `array`.
		///\param offset  the start index of ring buffer
		Index_t copyToArray( Data_t array[], Index_t len, Index_t offset=0 ) /*const*/ {
			int i=0, j; if( -length()<=offset && offset<length() )  j=offset; else j=0;
			while( i<len && j<length() ){
				array[i] = operator[](j);
				i++; j++;
			}
			return i;
		}
		
		/// Copy elements last `len` elements from buffer to `array`.
		Index_t copyLastToArray( Data_t array[], Index_t len ) /*const*/ {
			Index_t i=0, j = length() > len ? length()-len : 0;
			while( i<len && j<length() ){
				array[i] = operator[](j);
				i++; j++;
			}
			return i;
		}
		
		/// Обрезать буфер.
		Index_t trim( Index_t fromBegin/*, Index_t fromEnd=0*/ ){
			if( fromBegin > length() )
				fromBegin = length();
			incIndex(head_, fromBegin);
			length_ -= fromBegin;
			return length_;
		}
		
		/** Возвращает элемент (ссылку) по индексу
		 *  Отрицательный индекс означает отсчёт с хвоста, [-1] это последний, [-2] предпоследний */ 
		Data_t& operator[](Index_t i)
		{
			//if(!(-length() <= i && i < length()))
			//	printf("if(-length() <= i && i < length()) i=%d, len=%d\n", i, length_);
			assert( length_!=0/*!isEmpty()*/ ); 
			assert( (-length()) <= i && i < length() );
			/*if( isEmpty() || length() <= i || i < -length() ){
				return *(Data_t*)NULL; 
				throw std::out_of_range("Index out of bounds"); //throw IndexOutOfBoundsException;
			}*/
			if( i<0 )
				i += length();
			return _data[ incIndexOf(head_, i) ];
		}
		
		/** Возвращает элемент (ссылку) по индексу
		 *  Отрицательный индекс означает отсчёт с хвоста, [-1] это последний, [-2] предпоследний */ 
		const Data_t& operator[](Index_t i) const
		{
			assert_amsg( !isEmpty() ); 
			assert_amsg( (-length()) <= i && i < length() );  //throw IndexOfBoundException()
			if( i<0 )
				i += length();
			return _data[ incIndexOf(head_, i) ];
		}
		
		/**	Возвращает const элемент (значение элемента) по индексу. const this
		  *	Отрицательный индекс означает отсчёт с хвоста, [-1] это последний, [-2] предпоследний */
		/*const Data_t& operator[] (Index_t i) const 
		{
			if( isEmpty() || i >= length() || i < -length() )
				return Data_t();
			else if( i<0 )
				i += length();
			return _data[ incIndexOf(head_, i) ];
		}*/
		
		/// Пуст ли буфер
		bool isEmpty() const 
		{
			return length_ == 0; //return tail_ == head_;
		}
		
		/// Полон ли буфер
		bool isFull() const 
		{
			return length_ == bourn_; //return length_ == SIZE; //return nextIndexOf(tail_) == head_;
		}
		
		/// Очистить буфер
		RingBuffer<Data_t,Allocator>& clear() 
		{
			head_ = tail_ = length_ = 0;
			return *this;
		}
		
		/// Количество элементов в буфере
		Index_t length() const {
			return length_; //return (tail_ >= head_) ? (tail_ - head_) : (SIZE+1 - head_ + tail_);
		}
		
		/// Размер буфера
		size_t size() const {
			return SIZE;
		}
		
		/// Получить предельный размер для записи. Удобно при динамическом изменении длины кольцевого буфера.
		/// Например, чтобы укоротить/удлиннить фильтр во время выполнения.
		size_t bourn() const {
			return bourn_;
		}
		
		/// Установить предельный размер для записи. Удобно при динамическом изменении длины кольцевого буфера.
		/// Не может привышать SIZE.
		/// При уменьшении, Буфер выплюнет в пустоту лишнее (более старое).
		/// \return установленное значение
		size_t bourn(size_t b) 
		{
			if( b <= size() )  bourn_ = b;
			else bourn_ = size();
			// выбросить лишние
			if( length() > bourn_ ) 
				trim( length() - bourn_ );
			return bourn_;
		}
		
		
		// DSP functions
		/**
		 * 
		 */
		Data_t average() //const
		{
			Data_t avg = 0;
			for( Index_t i=0; i<length(); i++ )
				avg += operator[](i);
			avg /= length();
			return avg;
		}
		
		
		/**
		 *
		 */
		/*Data_t mean() //const
		{
			return average();
		}*/
		
		/// Iterative mean. http://www.heikohoffmann.de/htmlthesis/node134.html
		/// cons: Less efficient 
		/// plus: disallow overflow
		Data_t mean()
		{
			Data_t avg = 0;
			for( Index_t i=0; i<length(); i++) {
				avg += ((*this)[i] - avg) / (i + 1);
			}
			return avg;
		}

		
		/**
		 * 
		 */
		Data_t filterOneValueKalman( Index_t numK, const Data_t koefs[] ) //const
		{
			//assert_amsg( numK < length() );
			//Data_t koefs[numK] = { 0.4, 0.3, 0.2, 0.1 };  //сумма должна быть равна 1.0
			Data_t filtered = 0;
			for( Index_t i=length()-1, j=0; 
					i>=0 && j<numK;
					i--, j++ ){
				filtered += operator[](i) * koefs[j];  //koefs[i-length()+1]
			}
			return filtered;
		}
		
		/**
		 *
		 */
		Data_t rms() //const
		{
			float r = 0;
			for( int i=0; i<this->length(); i++ ){
				r += (*this)[i] * (*this)[i];
			}
			r = __sqrtf( r / length() );  //should expand as the native VFP square root instructions. not specified in any standard
			//r = sqrt( r / length() );
			return r;
		}
		
		/**
		 *
		 */
		Data_t peakToPeak() //const
		{
			float val = (*this)[0];
			float max = val, min = val;  //float max = -INFINITY, min = +INFINITY;
			for( int i=1; i<length(); i++ ){
				val = (*this)[i];
				if( max < val )
					max = val;
				if( min > val )
					min = val;
			}
			return max-min;
		}
		
		/// окресность
		bool okrestnost( const Data_t &val, const Data_t &main, const Data_t &okr ){
			return (main-okr < val && val < main+okr);
		}
		
		/// отсев выбросов больше
		///\todo возникнет ошибка с j, когда length <=1
		///\note рассчитано на небольшое количество выбросов.
		void filter_otsev( Data_t main, Data_t okr )
		{
			for( int i=0; i<length(); i++ ){
				Data_t &val = (*this)[i];
				if( !okrestnost(val,main,okr) ){
					Index_t j = i>=1 ? i-1 : i+1;
					while( j<length() && !okrestnost((*this)[j],main,okr) ){
						j++;
					}
					val = (*this)[j];
				}
			}
		}
		
		struct Analyzed {
			struct Entry { std::size_t index; Data_t value; };
			Entry min;
			Entry max;
			Data_t peakToPeak;
			Data_t avg;
			Data_t rms;
			Data_t middle;
		};
		
		
		/// Полный анализ, чтоб не прогонять массив по нескольку раз. Эффективнее, если нужно получить несколько параметров.
		Analyzed& analyze(Analyzed &an) const
		{
			if( isEmpty() ){
				//an = {0}; 
				return an; 
			}
			an.min.index = an.max.index = 0;
			Data_t val = (*this)[0];
			an.max.value = val; an.min.value = val;
			an.avg = val; 
			an.rms = val*val;
			for( int i=1; i<length(); i++ ){
				val = (*this)[i];
				if( an.max.value < val || std::isnan(an.max.value) ){
					an.max.value = val;
					an.max.index = i;
				}
				if( an.min.value > val || std::isnan(an.min.value) ){
					an.min.value = val;
					an.min.index = i;
				}
				an.avg += val;
				an.rms += val * val;
			}
			an.peakToPeak = an.max.value - an.min.value;
			an.avg /= length();
			an.rms = std::sqrt( an.rms / length() );
			an.middle = an.min.value + an.peakToPeak /2;
			return an;
		}
		
		
		/// 
		Analyzed analyze() const
		{
			Analyzed an; 
			analyze(an);
			return an;  // We may asiign stack based ref only to const ref!
		}
		
	};



	template<typename Data_t, typename Allocator = std::allocator<Data_t> >
	class RingBufferDSP : /*virtual*/ public RingBuffer<Data_t,Allocator> 
	{
	private:
		using Super = RingBuffer<Data_t, Allocator>;
	
	public:
		/// тупой отсев(screening)
		void filter_otsev__( Data_t main, Data_t okr )
		{
			for( int i=0; i<this->length(); i++ ){
				Data_t &val = (*this)[i];
				if( val < -okr )
					val=-okr;
				else if( val > okr )
					val=okr;
			}
		}
		
		/**
		 * Xk = K*Zk + (1-K)*(X(k-1)+Uk)
		* \param k коэффициент Калмана. Уровень доверия показаниям датчика.
		 * \param uk ожидаемое изменение (дельта)
		 */
		Data_t simpleKalman( Data_t const &k, Data_t const uk = 0 ) //const
		{
			/*if( length() >= 2 && isnormal(operator[](-1)) && isnormal(operator[](-2)) )
				operator[](-1) = k * operator[](-1) + (1.f-k) * (operator[](-2) + uk);
			return operator[](-1);*/
			if( this->length() >= 2 && std::isnormal(this->operator[](-1)) && isnormal(this->operator[](-2)) )
				this->operator[](-1) = k * this->operator[](-1) + (1.f-k) * (this->operator[](-2) + uk);
			return this->operator[](-1);
		}
	};

}  // namespace Sledge
