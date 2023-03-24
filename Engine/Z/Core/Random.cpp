//
// Created by 32725 on 2023/3/21.
//

#include "Random.h"

namespace Z{
	std::mt19937 Random::RandomEngine=std::mt19937();
	std::uniform_real_distribution<float> Random::Distribution=std::uniform_real_distribution<float>(0.0f,1.0f);

}