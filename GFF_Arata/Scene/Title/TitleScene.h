#pragma once
#include "../SceneBase.h"
#include <vector>
//#include "../../Objects/BackImageBase.h"

enum eSelectMenu
{
	eNONE,
	ePLAY,
	eHELP,
	eEXIT
};

class TitleScene : public SceneBase
{
public:
	/*std::vector<int> UI_num;
	std::vector<int> UI_string;
	std::vector<int> UI_coin;*/

	int title_font_handle;
	int menu_font_handle;
	int menu_font_mini;

	int pickup_color;
	int dropoff_color;
	int num_image;
	int string_image[26];
	int back_ground_image;	//”wŒi‰æ‘œ
	int TitleDoll_image;
	eSelectMenu select_menu;

	int cursor_sound;
	int decision_sound;

	virtual void Initialize() override;
	virtual eSceneType Update(float delta_second) override;
	virtual void Draw() const override;
	virtual void Finalize() override;
	virtual eSceneType GetNowSceneType() const override;
	void SetUpSelectMenuType();
	void SetDownSelectMenuType();
};