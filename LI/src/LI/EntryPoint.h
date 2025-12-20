#pragma once

#ifdef LI_PLATFORM_WINDOWS

extern LI::Application* LI::CreateApplication();

int main(int argc, char** argv) {
	LI::log::Init();
	LI_CORE_WARN("Initialized log");
	std::string name = "jfm";
	LI_INFO("Hello! {0}", name);
	auto app = LI::CreateApplication();
	app->Run();
	delete app;
}

#endif // LI_PLATFORM_WINDOWS