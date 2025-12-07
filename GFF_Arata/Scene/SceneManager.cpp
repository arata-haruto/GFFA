#include "SceneManager.h"
#include "SceneFactory.h"
#include "../Utility/Application.h"
#include "../Utility/InputManager.h"

SceneManager::SceneManager() : current_scene(nullptr)
{

}

void SceneManager::Initialize()
{
	//ChangeScene(eSceneType::eTitle);
	ChangeScene(eSceneType::eTitle);
}

bool SceneManager::Update(float delta_second)
{
	InputManager* input = InputManager::GetInstance();

	input->Update();

	eSceneType next_scene = current_scene->Update(delta_second);

	// 画面の初期化
	ClearDrawScreen();
	// シーンの描画処理
	current_scene->Draw();
	// 裏画面の内容を表画面に反映する
	ScreenFlip();

	// ゲームを終了するか確認する
	if ((next_scene == eSceneType::eExit) ||
		(eInputState::Release == input->GetButtonState(XINPUT_BUTTON_BACK)) ||
		(eInputState::Release == input->GetKeyState(KEY_INPUT_ESCAPE)))
	{
		return false;
	}

	if (current_scene->GetNowSceneType() != next_scene)
	{
		ChangeScene(next_scene);
	}

	return true;
}


void SceneManager::Finalize()
{
	if (current_scene != nullptr)
	{
		current_scene->Finalize();
		delete current_scene;
		current_scene = nullptr;
	}
}

bool SceneManager::ChangeScene(enum eSceneType type)
{
	//新しいシーンを作成する
	SceneBase* next_scene = SceneFactory::CreateScene(type);

	//エラーチェック
	if (next_scene == nullptr)
	{
		throw("シーン生成できませんでした\n");
		return false;
	}

	//変更前シーンの終了処理
	if (current_scene != nullptr)
	{
		current_scene->Finalize();
		delete current_scene;
	}

	//新しいシーンの初期化処理
	next_scene->Initialize();
	//シーンの上書き
	current_scene = next_scene;

	return true;
}
