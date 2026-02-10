#pragma once
#include "../Utility/Singleton.h"
#include "SceneBase.h"
#include <memory>

class SceneManager : public Singleton<SceneManager>
{
public:
    std::unique_ptr<SceneBase> current_scene;

    SceneManager();

    void Initialize();
    bool Update(float delta_second);
    void Finalize();

private:
    bool ChangeScene(enum eSceneType type);
};
