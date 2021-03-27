/**
 * Author: caoge@strivemycodelife@163.com
 * Date: 2021-03-12
 */
#pragma once

#include <functional>
#include <iostream>

#undef LIKELY
#undef UNLIKELY

#if defined(__GNUC__)
#define LIKELY(x) (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

#define FATAL 1
#undef LOG
#define LOG(x) std::cout
#define LOG_FATAL(x) LOG(X)

class LibInitializer {
    LibInitializer(std::function<void(void)> func) {
        func();
    }
};

#define LIB_INITIALIZER(name, f) \
    namespace { \
        LibInitializer lib_initializer_##name(f); \
    }  
