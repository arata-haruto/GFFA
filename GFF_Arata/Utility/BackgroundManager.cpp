#include "BackgroundManager.h"
#include "ResourceManager.h"

const float BackgroundManager::SCREEN_WIDTH = 1280.0f;
const float BackgroundManager::SCREEN_HEIGHT = 720.0f;

BackgroundManager::BackgroundManager()
    : mainBackgroundImage(-1)
{
    for (int i = 0; i < 5; i++) {
        bgHandles[i] = -1;
    }
}

BackgroundManager::~BackgroundManager()
{
    Finalize();
}

void BackgroundManager::Initialize()
{
    ResourceManager* rm = ResourceManager::GetInstance();
    mainBackgroundImage = rm->GetImageResource("Resource/Background/Shop_BG.png")[0];

    if (mainBackgroundImage == -1) {
        bgHandles[0] = rm->GetImageResource("Resource/Background/BG.jpg")[0];
        bgHandles[1] = rm->GetImageResource("Resource/Background/BG.jpg")[0];
        bgHandles[2] = rm->GetImageResource("Resource/Background/BG.jpg")[0];
        bgHandles[3] = rm->GetImageResource("Resource/Background/BG.jpg")[0];
        bgHandles[4] = rm->GetImageResource("Resource/Background/BG.jpg")[0];
    }

    LoadBackgroundObjects();
}

void BackgroundManager::LoadBackgroundObjects()
{
    ResourceManager* rm = ResourceManager::GetInstance();
    backgroundObjects.clear();
    auto objects = MapData::GetBackgroundObjects();
    for (const auto& obj : objects) {
        int handle = -1;
        if (!obj.imagePath.empty()) {
            handle = rm->GetImageResource(obj.imagePath)[0];
        }
        backgroundObjects.push_back({ handle, obj.x, obj.y, obj.width, obj.height });
    }
}

void BackgroundManager::Finalize()
{
    mainBackgroundImage = -1;

    for (int i = 0; i < 5; i++) {
        bgHandles[i] = -1;
    }
    backgroundObjects.clear();
}

void BackgroundManager::DrawBackground(int areaIndex, bool dimmed) const
{
    if (dimmed) {
        DrawBox(0, 0, (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT, GetColor(0, 0, 0), TRUE);
        int handle = bgHandles[areaIndex];
        if (handle != -1) {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
            DrawGraph(0, 0, handle, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        return;
    }

    if (mainBackgroundImage != -1) {
        int bgW, bgH;
        GetGraphSize(mainBackgroundImage, &bgW, &bgH);
        int srcX = areaIndex * 800;
        int srcW = 800;
        int drawX = ((int)SCREEN_WIDTH - 800) / 2;

        // エリアごとの色補正
        if (areaIndex == 0) SetDrawBright(200, 200, 200);
        else if (areaIndex == 1) SetDrawBright(220, 220, 255);
        else if (areaIndex == 2) SetDrawBright(255, 255, 200);
        else if (areaIndex == 3) SetDrawBright(255, 200, 200);
        else SetDrawBright(100, 100, 150);

        DrawRectGraph(drawX, 0, srcX, 0, srcW, (int)SCREEN_HEIGHT, mainBackgroundImage, TRUE, FALSE);
        SetDrawBright(255, 255, 255);
    }
    else {
        int handle = bgHandles[areaIndex];
        if (handle != -1) {
            DrawGraph(0, 0, handle, TRUE);
        }
    }
}

void BackgroundManager::DrawBackgroundObjects(float cameraOffsetX) const
{
    for (const auto& obj : backgroundObjects) {
        float drawX = obj.x - cameraOffsetX;
        if (drawX + obj.width < 0 || drawX > SCREEN_WIDTH) continue;

        if (obj.handle != -1) {
            DrawExtendGraph((int)drawX, (int)obj.y,
                (int)(drawX + obj.width), (int)(obj.y + obj.height),
                obj.handle, TRUE);
        }
        else {
            DrawBox((int)drawX, (int)obj.y,
                (int)(drawX + obj.width), (int)(obj.y + obj.height),
                GetColor(100, 100, 100), TRUE);
        }
    }
}

int BackgroundManager::GetMainBackgroundImage() const
{
    return mainBackgroundImage;
}

int BackgroundManager::GetBgHandle(int index) const
{
    if (index >= 0 && index < 5) {
        return bgHandles[index];
    }
    return -1;
}
