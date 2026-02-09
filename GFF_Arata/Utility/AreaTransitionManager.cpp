#include "AreaTransitionManager.h"

AreaTransitionManager::AreaTransitionManager()
    : currentAreaIndex(0),
    nextAreaIndex(0),
    fadeAlpha(0.0f),
    isFadingOut(false),
    isTransitioning(false)
{
}

AreaTransitionManager::~AreaTransitionManager()
{
}

void AreaTransitionManager::Initialize()
{
    currentAreaIndex = 0;
    nextAreaIndex = 0;
    fadeAlpha = 0.0f;
    isFadingOut = false;
    isTransitioning = false;
}

bool AreaTransitionManager::CheckAndStartTransition(float playerX)
{
    if (isTransitioning) return false;

    float areaStartX = MapData::GetAreaStartX(currentAreaIndex);
    float areaEndX = areaStartX + MapData::AREA_WIDTH;

    if (playerX > areaEndX - 30.0f && currentAreaIndex < 4) {
        StartTransition(currentAreaIndex + 1);
        return true;
    }
    else if (playerX < areaStartX + 30.0f && currentAreaIndex > 0) {
        StartTransition(currentAreaIndex - 1);
        return true;
    }

    return false;
}

void AreaTransitionManager::StartTransition(int newAreaIndex)
{
    isTransitioning = true;
    nextAreaIndex = newAreaIndex;
    isFadingOut = true;
    fadeAlpha = 0.0f;
}

void AreaTransitionManager::Update(float deltaTime)
{
    if (!isTransitioning) return;

    const float fadeSpeed = 300.0f;

    if (isFadingOut) {
        fadeAlpha += fadeSpeed * deltaTime;
        if (fadeAlpha >= 255.0f) {
            fadeAlpha = 255.0f;
            isFadingOut = false;
            currentAreaIndex = nextAreaIndex;
        }
    }
    else {
        fadeAlpha -= fadeSpeed * deltaTime;
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            isTransitioning = false;
        }
    }
}

float AreaTransitionManager::CalculateNewPlayerX(int prevAreaIndex) const
{
    float newAreaStartX = MapData::GetAreaStartX(currentAreaIndex);
    float newAreaEndX = newAreaStartX + MapData::AREA_WIDTH;

    if (prevAreaIndex < currentAreaIndex) {
        return newAreaStartX + 100.0f;
    }
    else {
        return newAreaEndX - 100.0f;
    }
}

bool AreaTransitionManager::IsTransitioning() const
{
    return isTransitioning;
}

float AreaTransitionManager::GetFadeAlpha() const
{
    return fadeAlpha;
}

int AreaTransitionManager::GetCurrentAreaIndex() const
{
    return currentAreaIndex;
}

int AreaTransitionManager::GetNextAreaIndex() const
{
    return nextAreaIndex;
}

bool AreaTransitionManager::IsFadingOut() const
{
    return isFadingOut;
}

bool AreaTransitionManager::IsTransitionComplete() const
{
    return !isTransitioning && fadeAlpha <= 0.0f;
}

void AreaTransitionManager::CompleteTransition()
{
    isTransitioning = false;
    fadeAlpha = 0.0f;
}
