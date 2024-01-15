//
// Created by 32725 on 2023/3/27.
//
#include "Z/Scene/Entity.hpp"
#include "Z/Core/zGUID.h"
#include "Z/Scene/Components.h"

namespace Z {
	zGUID Entity::GetUID() {
		return GetComponent<IDComponent>().ID;
	}

	const std::string &Entity::GetName() const { return GetComponent<TagComponent>().tag; }

}