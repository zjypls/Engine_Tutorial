//
// Created by 32725 on 2023/3/11.
//
#ifdef Z_PLATFORM_WIN32

extern Z::Application* Z::GetApplication(const Z::CommandArgs& args);
int main(int argc,char** argv){
	Z::Log::Init();
	auto app=Z::GetApplication({argc,argv});
	app->Run();
	delete app;
	return 0;
}
#endif