#pragma once
#include "SceneBase.h"
#include "Title/TitleScene.h"
#include "InGame/InGameScene.h"
#include "Help/HelpScene.h"
//#include "Result/ResultScene.h"
#include <memory>

class SceneFactory
{
public:
	static std::unique_ptr<SceneBase> CreateScene(eSceneType type)
	{
		switch (type)
		{
		case eSceneType::eTitle:
			return std::make_unique<TitleScene>();

		case eSceneType::eInGame:
			return std::make_unique<InGameScene>();

		case eSceneType::eHelp:
			return std::make_unique<HelpScene>();

		/*case eSceneType::eResult:
			return dynamic_cast<SceneBase*>(new ResultScene());*/

		default:
			return {};

		}
	}
};
