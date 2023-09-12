//
// Created by 32725 on 2023/3/11.
//
#include "./ZEditor.h"
#include "Z/Core/Entry.h"

//make application use discrete GPU
extern "C" {
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
// The following line is to favor the high performance NVIDIA GPU if there are multiple GPUs
// Has to be .exe module to be correctly detected.
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// And the AMD equivalent
// Also has to be .exe module to be correctly detected.
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}

class Editor : public Z::Application {
public:
	Editor(const Z::ApplicationSpec &spec) : Application(spec) {
		PushLayer(new Z::EditorLayer());
	}

	~Editor() override = default;
};

Z::Application *Z::GetApplication(const Z::CommandArgs &args) {
	return new Editor({"ZEditor", args, Z_SOURCE_DIR});
}


