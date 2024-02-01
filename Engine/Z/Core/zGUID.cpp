//
// Created by 32725 on 2023/4/4.
//

#include "Z/Core/zGUID.h"


namespace Z {
	static std::random_device rd;
	static std::mt19937_64 randomEngine(rd());
	static std::uniform_int_distribution<uint64> distribution;

	zGUID::zGUID(): value(distribution(randomEngine)) {
	}

}