#pragma once
#include "../SceneBase.h"
#include "../../Objects/Player.h"
#include "../../Objects/NPC.h"
#include "../../Utility/NPCManager.h"
#include "../../Utility/DialogueSystem.h"
#include "../../Utility/ReasoningManager.h"
#include "../../Utility/ReasoningUI.h"
#include "../../Utility/ItemManager.h"
#include "../../Utility/MiniGameManager.h"
#include "../../Utility/AreaTransitionManager.h"
#include "../../Utility/GameTimer.h"
#include "../../Utility/BackgroundManager.h"
#include "MapData.h"
#include <vector>

enum class GamePhase {
    Opening,
    EvidenceCollection,
    NPCDialogue,
    Reasoning,
    Result,
    MiniGame,
    AreaTransition
};

class InGameScene : public SceneBase {
private:
    Player* player1;
    ItemManager itemManager;
    NPCManager npcManager;
    DialogueSystem* dialogueSystem;

    // 新しいマネージャークラス
    MiniGameManager miniGameManager;
    AreaTransitionManager areaTransitionManager;
    GameTimer gameTimer;
    BackgroundManager backgroundManager;

    int charHandle_Player;
    int charHandle_Police;
    int charHandle_Fujisaki;
    int charHandle_Sasaki;
    int charHandle_Kimura;
    int charHandle_Yamada;

    bool allEvidenceCollected;

    GamePhase currentPhase;
    bool isSelectingEvidence;
    float evidenceResultTimer;
    std::string evidenceResultMessage;

    ReasoningManager* reasoningManager;
    ReasoningUI* reasoningUI;

    bool showResult;
    bool isCorrect;
    bool isGameOver;
    float resultDisplayTime;

    mutable float cameraX;

    int mainbgm;
    int se_success;
    int se_fail;

    bool showNPCIndicator;

public:
    InGameScene();
    virtual ~InGameScene();

    void Initialize() override;
    eSceneType Update(float delta_second) override;
    void Draw() const override;
    void Finalize() override;
    eSceneType GetNowSceneType() const override;

private:
    void LoadCharacterImages();
    void SetupOpeningDialogue();
    void SetupNPCDialogue(NPC* npc);

    void TransitionToReasoning();
    void DrawPhaseInfo() const;
    void DrawResult() const;
    void DrawOpening() const;
    void DrawAreaInfo() const;

    void DrawNPCIndicator() const;
    int CountQuestionedNPCs() const;
};