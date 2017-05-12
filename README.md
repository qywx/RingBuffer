
[![Build Status](https://travis-ci.org/sledgeh/RingBuffer.png?branch=master)](https://travis-ci.org/sledgeh/RingBuffer)


RingBuffer
==========
This is a header-only implementation of ring (or circular) buffer in C++11. It depends only on few stdlib definitions.
Read more about [Circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) on wikipedia.

See usage examples in folder [`test`](test/)


To do
-----
- Better compability with `std::` containers
- Iterators
- Memory reallocation
- Some functional extensions


Build and run
-------------
After project was cloned
```
$ mkdir ./build  &&  cd ./build
$ cmake -G"MinGW Makefiles" ../
$ cmake --build ./
$ cd ./test  &&  ctest
```
