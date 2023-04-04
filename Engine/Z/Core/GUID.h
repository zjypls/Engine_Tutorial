//
// Created by 32725 on 2023/4/4.
//

#ifndef ENGINE_TUTORIAL_GUID_H
#define ENGINE_TUTORIAL_GUID_H

#include <xhash>
#include <random>

namespace Z {

	class GUID {
	public:
		using Type = uint64_t;

		GUID();

		GUID(Type value):value(value) {}

		GUID(const GUID &other) = default;

		operator uint64_t() const { return value; }

	private:
		Type value;
	};

}
namespace std {
	template<>
	struct hash<Z::GUID> {
		size_t operator()(const Z::GUID &guid) const {
			return std::hash<Z::GUID::Type>()(guid);
		}
	};
}


#endif //ENGINE_TUTORIAL_GUID_H
