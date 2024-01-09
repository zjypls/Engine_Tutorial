//
// Created by 32725 on 2023/3/11.
//


#ifndef ENGINE_TUTORIAL_CORE_H
#define ENGINE_TUTORIAL_CORE_H
#include <memory>
#include <string>
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
	//Todo:improve
	#define Z_ASSERT(x, ...) do{if(!(x)){Z_ERROR("Assertion Failed!:{0}",__VA_ARGS__);assert(false);}}while(0)
	#define Z_CORE_ASSERT(x, ...) do{if(!(x)){Z_CORE_ERROR("Assertion Failed!:{0}",__VA_ARGS__);assert(false);}}while(0)
#else
	#define Z_ASSERT(x,...)
	#define Z_CORE_ASSERT(x,...)
#endif

#define BIT(x) (1<<x)
#define Z_BIND_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

namespace Z{
    // Z_SOURCE_DIR means #define Z_SOURCE_DIR "{CMAKE_SOURCE_DIR}" provide with cmake
    static const std::string ROOT_PATH=Z_SOURCE_DIR"/";
#ifdef Z_PLATFORM_LINUX
	static constexpr char Z_SEPEARATOR='/';
#endif
#ifdef Z_PLATFORM_WIN32
	static constexpr char Z_SEPEARATOR='\\';
#endif



	template<typename T>
	using Scope=std::unique_ptr<T>;
	template<typename T,typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args){
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
	template<typename T>
	using Ref=std::shared_ptr<T>;
	template<typename T,typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args){
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}


#endif //ENGINE_TUTORIAL_CORE_H