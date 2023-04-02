//
// Created by 32725 on 2023/3/11.
//


#ifndef ENGINE_TUTORIAL_CORE_H
#define ENGINE_TUTORIAL_CORE_H
#include <memory>
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
#else
	#error Supports Windows Only!
#endif

#ifndef Z_ENABLE_ASSERT
	#define Z_ASSERT(x, ...) {if(!(x)){Z_ERROR("Assertion Failed!:{0}",__VA_ARGS__);__debugbreak();}}
	#define Z_CORE_ASSERT(x, ...) {if(!(x)){Z_CORE_ERROR("Assertion Failed!:{0}",__VA_ARGS__);__debugbreak();}}
#else
	#define Z_ASSERT(x,...)
	#define Z_CORE_ASSERT(x,...)
#endif

#define BIT(x) (1<<x)
#define Z_BIND_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

namespace Z{
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