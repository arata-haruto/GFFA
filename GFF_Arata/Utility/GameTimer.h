#pragma once
#include "DxLib.h"

class GameTimer {
private:
    float timeLimit;
    float remainingTime;
    bool isPaused;

public:
    GameTimer();
    ~GameTimer();

    void Initialize(float limit = 60.0f);
    void Update(float deltaTime);
    void Draw() const;

    void Pause();
    void Resume();
    void AddTime(float seconds);
    void SubtractTime(float seconds);

    bool IsExpired() const;
    bool IsPaused() const;
    float GetRemainingTime() const;
    float GetTimeLimit() const;

private:
    void DrawTimerBox() const;
    void DrawPoliceMessage() const;
};
