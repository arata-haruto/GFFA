#pragma once
#include "ProjectConfig.h"
#include "singleton.h"
#include <string>
#include "../Scene/SceneManager.h"

#define D_SUCCESS		(0)		//成功
#define D_FAILURE		(-1)	//失敗

class Application : public Singleton<Application>
{
private:
	float delta_second;
	LONGLONG start_time;
	LONGLONG now_time;
	float refresh_rate;
	class SceneManager* scene;
	bool exit;						//終了フラグ

public:
	Application();
	~Application();
	bool WakeUp();
	void Run();
	void Shutdown();
	int ErrorThrow(std::string error_log);
	float Get_delta_Second();
private:
	void UpdateDeLtaTime();
};