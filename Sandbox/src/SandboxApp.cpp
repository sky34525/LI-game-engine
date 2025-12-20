#include "LI.h"

class SandboxApp : public LI::Application
{
	public:
	SandboxApp() {}
	~SandboxApp() {}
};

LI::Application* LI::CreateApplication()
{
	return new SandboxApp();
}