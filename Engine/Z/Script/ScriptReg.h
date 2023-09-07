//
// Created by 32725 on 2023/4/7.
//

#ifndef ENGINE_TUTORIAL_SCRIPTREG_H
#define ENGINE_TUTORIAL_SCRIPTREG_H
#define Z_INTERNAL_FUNC(Name) mono_add_internal_call("Z.Internal::"#Name,(void*)Name)

namespace Z {
	class ScriptReg {


	public:
		static void Reg();

		static void RegComponents();


	};
}


#endif //ENGINE_TUTORIAL_SCRIPTREG_H
