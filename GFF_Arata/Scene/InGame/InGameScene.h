#pragma once
#include "../SceneBase.h"
#include "../../Objects/Player.h"
#include "../../Objects/Floor.h"
#include "../../Utility/ItemManager.h"
#include "../../Utility/ReasoningManager.h"
#include "../../Utility/ReasoningUI.h"

enum class GamePhase {
    Opening,            // �I�[�v�j���O
    EvidenceCollection, // �؋����W
    Reasoning,          // �����t�F�[�Y
    Result              // ���ʕ\��
};

class InGameScene : public SceneBase {
private:
    Player* player1;
    Floor* floor;
    GamePhase currentPhase;

    // �^�C�}�[�֘A
    float timeLimit;
    float remainingTime;

    // �؋����W�֘A
    ItemManager itemManager;
    bool allEvidenceCollected;

    // �����֘A
    ReasoningManager* reasoningManager;
    ReasoningUI* reasoningUI;

    // ���ʕ\��
    bool showResult;
    bool isCorrect;
    float resultDisplayTime;

    // �J����
    float cameraX;

    // ���\�[�X
    int mainbgm;
    int bg_backdoor;       // ����
    int bg_storeinner;     // �X����
    int bg_cashregister;   // ���W����
    int bg_storefront;     // �X��
    int bg_outside;        // �X�O

public:
    InGameScene();

    void Initialize() override;
    eSceneType Update(float delta_second) override;
    void Draw() const override;
    void Finalize() override;
    eSceneType GetNowSceneType() const override;

private:
    void TransitionToReasoning();
    void DrawTimer() const;
    void DrawPhaseInfo() const;
    void DrawOpening() const;
    void DrawResult() const;
};