<h1> Kaurea - Thousand Golden </h1>

<p>Kaurea is a hashing method with multiple solutions and a size support of unlimited¹.</p>
<p>This hashing method was build for the large hash length,  and as an unique feature, being part of a bigger project, but at the same time it is open-source, letting you modify² and execute freely².</p>

- Support: This method is purely in C (**version: C99 or superior**), requiring usage of adaptation for other languages through FFI methods, function adaptation and other methods. *Easier adaption can be made with C++, RUST, GO, Haskell.*

- Usage: To use all you need to do is include all .C files that are located in src into your script, CMake or Makefile (do not include main.c/testvector.c). Make usage of the hash function and always allocate 257 len.

<foot>_¹ The hashing method has a maximum size of 4,294,967,295 bytes for 32-bit systems and 18,446,744,073,709,551,615 for 64-bit systems. ² When utilizing a modified version, make sure to notify that the version being used is modified._</foot>