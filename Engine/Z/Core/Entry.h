//
// Created by 32725 on 2023/3/11.
//
#ifdef Z_PLATFORM_WIN32
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
#elifdef Z_PLATFORM_LINUX
void SetEnvForLinux(){
    // https://us.download.nvidia.cn/XFree86/Linux-x86_64/525.78.01/README/primerenderoffload.html
    // noticed : it works well at my machine with Nvidia GeForce 1660Ti
    // Driver Version : 545.29.06 (nvidia-open-dkms)
    // arch 6.6.10 zen kernel
    putenv("__NV_PRIME_RENDER_OFFLOAD=1");
    putenv("__GLX_VENDOR_LIBRARY_NAME=nvidia");
    //Todo : amd ?
}
#endif
extern Z::Application* Z::GetApplication(const Z::CommandArgs& args);

int main(int argc,char** argv){
#ifdef Z_PLATFORM_LINUX
    SetEnvForLinux();
#endif
	Z::Log::Init();
	auto app=Z::GetApplication({argc,argv});
	app->Run();
	delete app;
	return 0;
}