//
// Created by 32725 on 2023/3/17.
//

#ifndef ENGINE_TUTORIAL_TIME_H
#define ENGINE_TUTORIAL_TIME_H
#include "Z/Core/Core.h"

namespace Z {
	class Z_API Time {
	public:
		Time() : time(GetTime()) {}
		operator float() const { return time; }
		float operator -(const Time& other) const { return time - other.time; }
		static void FlushTime(){lastFlushTime=GetTime();}
		static float GetFlushTime(){return GetTime()-lastFlushTime;}
		static float GetTime();
		inline static void Update(){deltaTime=GetTime()-lastTime;lastTime=GetTime();}
		inline static float DeltaTime(){return deltaTime;}
	private:
		float time;
		static float lastTime;
		static float deltaTime;
		static float lastFlushTime;
	};

}


#endif //ENGINE_TUTORIAL_TIME_H
