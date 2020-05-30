# C++ signals and slots

**vdk-signals** is a type-safe and thread-safe signals-slots system for standard C++ designed with performance and simplicity in mind. It follows the main philosophy of the C++ language avoiding unnecessary overheads and superfluous functionality that can slow down your program.

High performance is achieved through the use of modern C++ features and atomic variables. Specialized synchronization mechanism used internally makes signal emissions lock-free and ensures the fastest possible execution. The library supports synchronous and asynchronous slot invocations with automatic detection of target threads, as well as automatic object lifetime tracking.

`vdk-signals` has no external dependencies and relies on standard C++17 only. It is organized as the amalgamation and can be easily integrated into any existing project.

## Usage:

**Note!** `vdk-signals` requires a compiler that supports **C++17** standard.

The library is not meant to be built and linked as a standalone package. Instead, it is organized as "the amalgamation" and contains everything you need in just two files. This allows you to easily integrate `vdk-signals` into any target project. Just copy `signals.h` and `signals.cpp` into your project and compile them together with your other source files.

Please note, that in order to provide maximum flexibility and independence from target project's structure, `signals.cpp` includes `signals.h` as a standard header (`#include<signals.h>`), so make sure that `signals.h` resides in a folder that is searched for header files by the compiler.

GoogleTest is required to build and run tests. CMake files are provided to simplify the process. If you already have a copy of GoogleTest framework, just run CMake and set `GTEST_SOURCE_DIR` cache variable to point to the directory that contains your GoogleTest sources. If you don't have GoogleTest sources, CMake will download and compile them automatically.

`demo` directory contains code examples that can serve as a tutorial for learning how to use `vdk::signals`.

## [API documentation](docs/signals.md)

## License:

This software is licensed under the Apache License version 2.0.