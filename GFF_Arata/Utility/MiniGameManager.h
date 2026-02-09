#pragma once
#include "DxLib.h"
#include "../Objects/Item.h"

// ミニゲームの状態
enum class MiniGameState {
    Inactive,   // 非アクティブ
    Playing,    // プレイ中
    ShowResult  // 結果表示中
};

class MiniGameManager {
private:
    MiniGameState state;
    float barPosition;
    float barSpeed;
    float targetMin;
    float targetMax;
    Item* targetItem;
    float resultTimer;
    bool lastResultSuccess;

    int seSuccess;
    int seFail;

public:
    MiniGameManager();
    ~MiniGameManager();

    void Initialize();
    void Update(float deltaTime, float& remainingTime);
    void Draw() const;

    void Start(Item* item);
    bool IsActive() const;
    bool IsCompleted() const;
    bool WasSuccessful() const;
    Item* GetTargetItem() const;
    void Reset();

    void SetSoundEffects(int successSE, int failSE);
};
