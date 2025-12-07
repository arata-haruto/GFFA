#pragma once
#include "Singleton.h"
#include <unordered_map>
#include <string>
#include <vector>

class ResourceManager : public Singleton<ResourceManager>
{
private:
	std::unordered_map<std::string, std::vector<int>> image_container;		//画像コンテナ
	std::unordered_map<std::string, int> sound_container;					//音源コンテナ

public:
	const std::vector<int>& GetImageResource(const std::string& file_name, int all_num = 1, int num_x = 1, int num_y = 1, int size_x = 0, int size_y = 0);
	const int& GetSoundResource(const std::string& file_path);
	void UnloadAllResourceData();

private:
	void CreateImageResource(const std::string& file_path);			//読み込んだ画像をコンテナに入れる
	void CreateImagesResource(std::string file_name, int all_num, int num_x, int num_y, int size_x, int size_y);		//画像の読み込み
	void CreateSoundsResource(std::string file_path);				//音源の読み込み
	void UnloadImageResourceData();			//メモリの開放
	void UnloadSoundResourceData();			//メモリの開放
};