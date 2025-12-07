#pragma once
#include <vector>
#include <string>
#include <set>

// 選択肢エフェクト状態
enum class ChoiceEffectState {
    Normal,      // 通常表示
    Flash,       // 白フラッシュ中
    FadeOut,     // フェードアウト中
    Locked,      // ロック状態（除外）
    Incorrect    // 不正解（赤色表示）
};

// 推理の要素（犯人、動機、凶器）
struct ReasoningElement {
    std::string text;               // 表示テキスト
    std::vector<std::string> contradictoryEvidences; // これを持っていると除外される（複数可）
    bool isEliminated;              // 除外フラグ
    bool isCorrect;                 // 正解かどうか

    // エフェクト管理変数
    ChoiceEffectState effectState;
    float alpha;
    float flashTimer;
    float fadeTimer;
    bool wasEnabled;

    ReasoningElement(std::string t, std::vector<std::string> contras, bool correct)
        : text(t), contradictoryEvidences(contras), isEliminated(false), isCorrect(correct),
        effectState(ChoiceEffectState::Normal), alpha(255.0f),
        flashTimer(0.0f), fadeTimer(0.0f), wasEnabled(true) {}
};

enum class ReasoningStep {
    Suspect,    // 犯人選択
    Motive,     // 動機選択
    Weapon,     // 凶器選択
    Confirmation // 最終確認
};

class ReasoningManager {
private:
    std::vector<ReasoningElement> suspects;
    std::vector<ReasoningElement> motives;
    std::vector<ReasoningElement> weapons;

    ReasoningStep currentStep;
    int selectedIndex;
    bool isActive;

    int chosenSuspectIndex;
    int chosenMotiveIndex;
    int chosenWeaponIndex;

    int maxLife;
    int currentLife;

    std::string lastHintMessage;

public:
    ReasoningManager();
    void Initialize();
    void Update(float deltaTime);
    void Draw() const;

    void FilterOptions(const std::vector<std::string>& collectedEvidence);

    void SetActive(bool active) { isActive = active; }
    bool IsActive() const { return isActive; }

    ReasoningStep GetCurrentStep() const { return currentStep; }
    int GetSelectedIndex() const { return selectedIndex; }
    int GetCurrentLife() const { return currentLife; }
    int GetMaxLife() const { return maxLife; }

    const std::vector<ReasoningElement>& GetCurrentOptions() const;
    std::string GetStepTitle() const;
    std::string GetCurrentSelectionText() const;

    std::string GetLastHintMessage() const { return lastHintMessage; }

    bool IsGameClear() const;
    bool IsGameOver() const;

    void UpdateChoiceEffects(float deltaTime);

private:
    void ConfirmSelection();
    void CheckAnswer();
    void ResetToFirstStep();
};