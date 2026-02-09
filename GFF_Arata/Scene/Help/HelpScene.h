#pragma once
#include "../SceneBase.h"

class HelpScene : public SceneBase
{
private:
	int back_ground_image;
	int title_font_handle;
	int menu_font_handle;

public:
	HelpScene();
	virtual ~HelpScene();

	virtual void Initialize() override;
	virtual eSceneType Update(float delta_second) override;
	virtual void Draw() const override;
	virtual void Finalize() override;
	virtual eSceneType GetNowSceneType() const override;
};

