//
// Created by 32725 on 2023/4/4.
//

#ifndef ENGINE_TUTORIAL_ZGUID_H
#define ENGINE_TUTORIAL_ZGUID_H

#include <functional>
#include <random>

namespace Z {

	class zGUID {
	public:
		using Type = uint64_t;

		zGUID();

		zGUID(Type value) : value(value) {}

		zGUID(const zGUID &other) = default;

		operator uint64_t() const { return value; }

	private:
		Type value;
	};

}
namespace std {
	//Add a custom hash for zGUID class to use it as key in std::unordered_map
	template<>
	struct hash<Z::zGUID> {
		size_t operator()(const Z::zGUID &guid) const {
			return std::hash<Z::zGUID::Type>()(guid);
		}
	};
}


#endif //ENGINE_TUTORIAL_ZGUID_H