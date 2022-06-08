#pragma once
#include <cassert>
//#include <execinfo.h>
#include <stdio.h>
#include "format.h"
namespace reload {

    static inline void print_backtrace()
    {
/*
        char **strings;
        size_t i, size;
        enum Constexpr { MAX_SIZE = 1024 };
        void *array[MAX_SIZE];
        size = backtrace(array, MAX_SIZE);
        strings = backtrace_symbols(array, size);
        for (i = 0; i < size; i++)
            printf("%s\n", strings[i]);
        puts("");
        free(strings);
*/
    }

    static inline void assert_with_callstack(bool condition, const std::string_view message)
    {
        if (condition) return;
        //print_backtrace();
        std::cout << "*** Assertion Failed: " << message << std::endl;
        exit(1);
    }

    #ifndef RELOAD_GAME_NO_ASSERT
    #define reload_assert(condition, message) assert_with_callstack(condition, message);
    #else
    #define reload_assert(condition, message)
    #endif
}

