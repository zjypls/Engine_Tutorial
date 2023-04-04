//
// Created by 32725 on 2023/3/27.
//

#include "Entity.h"
#include "Components.h"

namespace Z {
	GUID::Type Entity::GetUID() {
		return GetComponent<IDComponent>().ID;
	}

	const std::string &Entity::GetName() const { return GetComponent<TagComponent>().tag; }

}