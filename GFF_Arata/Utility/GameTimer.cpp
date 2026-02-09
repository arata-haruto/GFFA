#include "GameTimer.h"

GameTimer::GameTimer()
    : timeLimit(60.0f),
    remainingTime(60.0f),
    isPaused(true)
{
}

GameTimer::~GameTimer()
{
}

void GameTimer::Initialize(float limit)
{
    timeLimit = limit;
    remainingTime = limit;
    isPaused = true;
}

void GameTimer::Update(float deltaTime)
{
    if (isPaused) return;

    remainingTime -= deltaTime;
    if (remainingTime < 0.0f) {
        remainingTime = 0.0f;
    }
}

void GameTimer::Draw() const
{
    DrawTimerBox();
    DrawPoliceMessage();
}

void GameTimer::DrawTimerBox() const
{
    int x = 20, y = 20;
    int boxWidth = 300, boxHeight = 80;

    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(0, 0, 0), TRUE);
    DrawBox(x - 10, y - 10, x + boxWidth, y + boxHeight, GetColor(100, 100, 100), FALSE);

    int minutes = (int)(remainingTime / 60.0f);
    int seconds = (int)(remainingTime) % 60;

    unsigned int color;
    if (remainingTime < 10.0f) {
        color = GetColor(255, 0, 0);
        if (((int)(remainingTime * 4)) % 2 == 0) {
            color = GetColor(255, 255, 0);
        }
    }
    else if (remainingTime < 30.0f) {
        color = GetColor(255, 150, 0);
    }
    else {
        color = GetColor(255, 255, 255);
    }

    SetFontSize(32);
    DrawFormatString(x, y, color, "Time: %02d:%02d", minutes, seconds);
    SetFontSize(16);
}

void GameTimer::DrawPoliceMessage() const
{
    int x = 20, y = 20;

    if (remainingTime <= 40.0f && remainingTime > 30.0f) {
        DrawFormatString(x, y + 45, GetColor(200, 200, 200), "警察官：「まだ余裕か？」");
    }
    else if (remainingTime <= 30.0f && remainingTime > 20.0f) {
        DrawFormatString(x, y + 45, GetColor(255, 150, 0), "警察官：「あと30秒だぞ！」");
    }
    else if (remainingTime <= 20.0f && remainingTime > 10.0f) {
        DrawFormatString(x, y + 45, GetColor(255, 100, 0), "警察官：「急げ！20秒だ！」");
    }
    else if (remainingTime <= 10.0f) {
        DrawFormatString(x, y + 45, GetColor(255, 0, 0), "警察官：「時間だ！答えろ！」");
    }
}

void GameTimer::Pause()
{
    isPaused = true;
}

void GameTimer::Resume()
{
    isPaused = false;
}

void GameTimer::AddTime(float seconds)
{
    remainingTime += seconds;
    if (remainingTime > timeLimit) {
        remainingTime = timeLimit;
    }
}

void GameTimer::SubtractTime(float seconds)
{
    remainingTime -= seconds;
    if (remainingTime < 0.0f) {
        remainingTime = 0.0f;
    }
}

bool GameTimer::IsExpired() const
{
    return remainingTime <= 0.0f;
}

bool GameTimer::IsPaused() const
{
    return isPaused;
}

float GameTimer::GetRemainingTime() const
{
    return remainingTime;
}

float GameTimer::GetTimeLimit() const
{
    return timeLimit;
}
