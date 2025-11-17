# Attest

A single header, lightweight unit testing library for C and C++. I made this because I'm too lazy to install a proper heavyweight unit testing framework for my projects. I intended for this to be super simple to drop in to projects, as well as for use in CI/CD pipelines or automated test runs.

# Features

- Around 500 lines of code, header only implementation
- Unified interface for both C and C++
- The only C dependencies are stdio and stdargs. C++ only adds type_traits, nothing else
- No dynamic allocation
- Optional JSON output - could be used to integrate with other applications
- Nice colours in the terminal
- Supports testing STL types, structs, and probably more 
- CLI arguments 
- macros based assertions

# How to use

1. Copy the header file, `attest.h`, into your project

2. Create a file to hold your unit tests, e.g. `test.c`.

3. In `test.c`, put this at the top:
```
#define ATTEST_IMPLEMENTATION

#include "attest.h"

```

4. Now write some tests. The first thing to do is register the test, then write the test logic. An example is:
```
REGISTER_TEST(string)
{
    ATTEST_EQUAL("hello", "hello");
}
```

5. Compile: Can use something like `gcc test1.c -o TestSuite`.

6. Run `./TestSuite`. You can choose to run with command line arguments as well:

![alt text](/Assets/image.png)

# Documentation

Macros:
- `REGISTER_TEST()`
- `ATTEST_TRUE()`
- `ATTEST_FALSE()`
- `ATTEST_EQUAL()`
- `ATTEST_NOT_EQUAL()`
- `ATTEST_LESS_THAN()`
- `ATTEST_GREATER_THAN()`
- `ATTEST_EQUAL_WITHIN_TOLERANCE()` - used for float comparisons

Command line arguments:
- `--list`: lists out all registered tests without running them
- `--filter="something"`: filter what tests to run based on if they contain a string
- `--json`: outputs the results in JSON. This goes to stdout, so you can pipe this however you want
- `--quiet`: only outputs failures to terminal