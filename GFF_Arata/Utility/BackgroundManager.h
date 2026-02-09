#pragma once
#include "DxLib.h"
#include "../Scene/InGame/MapData.h"
#include <vector>
#include <string>

struct RuntimeBgObject {
    int handle;
    float x, y;
    float width, height;
};

class BackgroundManager {
private:
    std::vector<RuntimeBgObject> backgroundObjects;
    int bgHandles[5];
    int mainBackgroundImage;

    static const float SCREEN_WIDTH;
    static const float SCREEN_HEIGHT;

public:
    BackgroundManager();
    ~BackgroundManager();

    void Initialize();
    void LoadBackgroundObjects();
    void Finalize();

    void DrawBackground(int areaIndex, bool dimmed = false) const;
    void DrawBackgroundObjects(float cameraOffsetX) const;

    int GetMainBackgroundImage() const;
    int GetBgHandle(int index) const;
};
