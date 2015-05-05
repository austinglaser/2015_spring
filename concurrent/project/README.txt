Here's my current progress on the project. I've spent a lot of time learning about C's stdatomic types and methods, and some more time in devising a good test suite and in attempting to use abstraction efficiently.

My hashtable is almost complete, but there is an outstanding issue that I'm combatting:

- Resizing has some race conditions that can occur if a new sentinel node is inserted and then removed while the resizing is taking place, but before the hash width has been updated

A note on C's atomics:

The implementation of any atomic type is not guaranteed to be lock free, since it is architecture dependent. On my particular computer (uname -a yields Linux BES-E6540-esd 3.13.0-49-generic #83-Ubuntu SMP Fri Apr 10 20:11:33 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux), all types are lock free (which can be determined by a macro in C and a public class member in C++. Using this library is preferable to me, because it should give portability to just about any architecture which provides stdatomic.h and gcc, with more or less efficiency.

Compiling and testing:

compile:                                make
test (and compile if necessary):        make test
benchmark (and compile if necessary):   make benchmark
clean:                                  make clean

Compiling requires gcc 4.9, which for me was available through apt-get on testing. gcc 4.8 does not provide stdatomic.h. Running the test target 'make test' requires valgrind, though simply running the test executables directly (hashtable_node and hashtable) can be done without valgrind
