#include "DxLib.h"
#include "Utility/Application.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	Application* a = Application::GetInstance();

	a->WakeUp();
	a->Run();

	return D_SUCCESS;

}