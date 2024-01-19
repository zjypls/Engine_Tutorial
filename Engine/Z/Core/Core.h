//
// Created by 32725 on 2023/3/11.
//


#ifndef ENGINE_TUTORIAL_CORE_H
#define ENGINE_TUTORIAL_CORE_H

#include <memory>
#include <string>
#include <cstring>
#include <cassert>

#ifdef Z_PLATFORM_WIN32
#ifdef Z_DYNAMIC_LINK
#ifdef Z_BUILD_DLL
#define Z_API __declspec(dllexport)
#else
#define Z_API __declspec(dllimport)
#endif
#else
#define Z_API
#endif
#endif
#ifdef Z_PLATFORM_LINUX
#ifdef Z_DYNAMIC_LINK
#error "not finished yet"
#ifdef Z_BUILD_DLL
#define Z_API __declspec(dllexport)
#else
#define Z_API __declspec(dllimport)
#endif
#else
#define Z_API
#endif
#endif

#ifndef Z_ENABLE_ASSERT

#ifdef __GNUC__
#define DEBUG_BREAK __builtin_trap
#else
#define DEBUG_BREAK __debugbreak
#endif

#define Z_ASSERT(x, ...) do{if(!(x)){Z_ERROR("Assertion Failed!:{0}",__VA_ARGS__);DEBUG_BREAK();}}while(0)
#define Z_CORE_ASSERT(x, ...) do{if(!(x)){Z_CORE_ERROR("Assertion Failed!:{0}",__VA_ARGS__);DEBUG_BREAK();}}while(0)
#else
#define Z_ASSERT(x,...)
#define Z_CORE_ASSERT(x,...)
#endif

#define BIT(x) (1<<x)
#define Z_BIND_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

namespace Z {
    using byte = unsigned char;
    using uint8 = byte;
    using int8 = char;
    using uint16 = unsigned short;
    using int16 = short;
    using uint32 = unsigned int;
    using int32 = int;
    using int64 = long int;
    using uint64 = unsigned long int;
    static_assert(sizeof(uint8) == 1);
    static_assert(sizeof(uint16) == 2 * sizeof(uint8));
    static_assert(sizeof(int16) == 2 * sizeof(uint8));
    static_assert(sizeof(uint32) == 2 * sizeof(uint16));
    static_assert(sizeof(uint64) == 4 * sizeof(uint16));
    static_assert(sizeof(int32) == 2 * sizeof(int16));
    static_assert(sizeof(int64) == 4 * sizeof(int16));
#ifdef Z_PLATFORM_LINUX
    static constexpr char Z_SEPARATOR = '/';
#endif
#ifdef Z_PLATFORM_WIN32
    static constexpr char Z_SEPARATOR='\\';
#endif
    // Z_SOURCE_DIR means #define Z_SOURCE_DIR "{CMAKE_SOURCE_DIR}" provide with cmake
    static const std::string ROOT_PATH = std::string(Z_SOURCE_DIR) + Z_SEPARATOR;
    static const char *BUILD_VERSION =
#ifdef Z_DEBUG
            "Debug"
#else
    "Release"
#endif
    ;
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args &&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args &&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}


#endif //ENGINE_TUTORIAL_CORE_H