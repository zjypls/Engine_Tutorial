//
// Created by 32725 on 2023/3/11.
//
#ifdef Z_PLATFORM_WIN32

extern Z::Application* Z::GetApplication();
int main(){
	Z::Log::Init();
	auto app=Z::GetApplication();
	app->Run();
	delete app;
	return 0;
}
#endif