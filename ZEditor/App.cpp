//
// Created by 32725 on 2023/3/11.
//
#include "./ZEditor.h"
#include "Z/Core/Entry.h"
#include "glm/gtc/matrix_transform.hpp"

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

class Editor : public Z::Application {
public:
	Editor(const Z::ApplicationSpec&spec):Application(spec) {
		PushLayer(new Z::EditorLayer());
	}

	~Editor() override = default;
};

Z::Application *Z::GetApplication(const Z::CommandArgs& args) {
	return new Editor({ "ZEditor",args,Z_SOURCE_DIR});
}


