#pragma once

#include <iostream>

#define ___QF_EMPTY_STRING ""
#define __QF_DONT_CARE -1
#define __QF_UNDEFINED -333
#define __QF_DEBUG_LEVEL 1

#define __QF_SHUTDOWN_MSG() \
    std::cout << "\033[31m" << \
    "+------------------------------------------------------+" << std::endl << \
    "|  retard alert                                 - [] x |" << std::endl << \
    "+------------------------------------------------------+" << std::endl << \
    "|                & SHUTTING DOWN QF &                  |" << std::endl << \
    "|                                                      |" << std::endl << \
    "|             || SAVE FROM CRASH-STATE ||              |" << std::endl << \
    "|          *dumb retarded fuck revise ur code          |" << std::endl << \
    "+------------------------------------------------------+" << std::endl << \
    "\033[0m" << std::endl;

#define __QF_SHUTDOWN() \
  do {                  \
    __QF_SHUTDOWN_MSG(); \
    std::exit(EXIT_FAILURE); \
  } while (false)

/* Assertion for qf, simple yet effective  */
#define __QF_ASSERT(condition, message)   \
    do {                              \
        if (!(condition)) {           \
            std::cerr << "\033[1;33m Assertion failed: " << #condition << "\n" \
                      << "\033[1;33m Why? -> " << message << "\033[0m\n"; \
            __QF_SHUTDOWN();  \
        }                             \
    } while (false)
