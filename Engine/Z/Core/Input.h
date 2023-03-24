//
// Created by 32725 on 2023/3/14.
//

#ifndef ENGINE_TUTORIAL_INPUT_H
#define ENGINE_TUTORIAL_INPUT_H
#include "Core.h"
#include <unordered_map>
#include"Z/Core/KeyCodes.h"
#include"Z/Core/MouseButtonCodes.h"

namespace Z {
	class Z_API Input {
	public:
		static bool IsKeyPressed(Z::KeyCode keycode);
		static bool IsMouseButtonPressed(Z::MouseCode button);
		inline static float GetMouseX(){return GetMousePosition().first;};
		inline static float GetMouseY(){return GetMousePosition().second;}
		static std::pair<float,float> GetMousePosition() ;

	};

}

#endif //ENGINE_TUTORIAL_INPUT_H
