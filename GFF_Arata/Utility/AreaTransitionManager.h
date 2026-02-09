#pragma once
#include "DxLib.h"
#include "../Scene/InGame/MapData.h"

class AreaTransitionManager {
private:
    int currentAreaIndex;
    int nextAreaIndex;
    float fadeAlpha;
    bool isFadingOut;
    bool isTransitioning;

public:
    AreaTransitionManager();
    ~AreaTransitionManager();

    void Initialize();
    void Update(float deltaTime);

    // エリア遷移のチェックと開始
    bool CheckAndStartTransition(float playerX);
    void StartTransition(int newAreaIndex);

    // 状態取得
    bool IsTransitioning() const;
    float GetFadeAlpha() const;
    int GetCurrentAreaIndex() const;
    int GetNextAreaIndex() const;
    bool IsFadingOut() const;

    // 遷移完了時の新しいプレイヤー位置を計算
    float CalculateNewPlayerX(int prevAreaIndex) const;

    // 遷移完了チェック
    bool IsTransitionComplete() const;
    void CompleteTransition();
};
