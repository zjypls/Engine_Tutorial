//
// Created by 32725 on 2023/3/21.
//

#include "Z/Core/Random.h"

namespace Z{
	std::random_device Random::randomDevice{};
	std::mt19937 Random::RandomEngine=std::mt19937{randomDevice()};
	std::uniform_real_distribution<float> Random::Distribution=std::uniform_real_distribution<float>(0.0f,1.0f);

}