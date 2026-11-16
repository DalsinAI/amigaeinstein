/* Put before every Einstein source file (-include): the C++ streams first,
 * so the stove's pthread.h (under <ios>) is read before <unistd.h>; read
 * the other way round, its cancellation macros for read() and close() land
 * in <fstream>. And <ctime>, which upstream gets from glibc's headers.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like Einstein. */
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
