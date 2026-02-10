#include "InGameScene.h"
#include "../SceneFactory.h"
#include "../../Utility/InputManager.h"
#include "../../Utility/ResourceManager.h"
#include "../../Utility/Vector2D.h"
#include "../../Objects/Player.h"
#include "../../Objects/Floor.h"
#include "../../Objects/Item.h"
#include <cmath>
#include <memory>

const float SCREEN_WIDTH = 1280.0f;
const float WORLD_WIDTH = MapData::MAP_WIDTH;

Floor g_floor;

InGameScene::InGameScene()
    : currentPhase(GamePhase::Opening),
    allEvidenceCollected(false),
    showResult(false),
    isCorrect(false),
    isGameOver(false),
    resultDisplayTime(0.0f),
    cameraX(0.0f),
    mainbgm(-1),
    se_success(-1),
    se_fail(-1),
    showNPCIndicator(true),
    charHandle_Player(-1),
    charHandle_Police(-1),
    charHandle_Fujisaki(-1),
    charHandle_Sasaki(-1),
    charHandle_Kimura(-1),
    charHandle_Yamada(-1),
    isSelectingEvidence(false),
    evidenceResultTimer(0.0f)
{
}

InGameScene::~InGameScene()
{
    Finalize();
}

void InGameScene::Initialize()
{
    ResourceManager* rm = ResourceManager::GetInstance();

    player1 = std::make_unique<Player>();
    player1->Initialize();
    g_floor.Initialize();

    itemManager.Init();
    auto evidenceList = MapData::GetEvidenceList();
    for (const auto& evidence : evidenceList) {
        itemManager.Add(std::make_unique<Item>(
            evidence.x, evidence.y,
            evidence.name, evidence.description,
            evidence.gameType
        ));
    }

    npcManager.Initialize();
    npcManager.AddSuspect(SuspectType::Fujisaki, MapData::AREA0_START + 400.0f, 560.0f);
    npcManager.AddSuspect(SuspectType::Kimura, MapData::AREA1_START + 400.0f, 560.0f);
    npcManager.AddSuspect(SuspectType::Sasaki, MapData::AREA2_START + 400.0f, 560.0f);
    npcManager.AddSuspect(SuspectType::Yamada, MapData::AREA4_START + 400.0f, 560.0f);
    for (const auto& npc : npcManager.GetAllNPCs()) {
        npc->Initialize();
    }

    dialogueSystem = std::make_unique<DialogueSystem>();
    dialogueSystem->Initialize();

    LoadCharacterImages();

    backgroundManager.Initialize();
    areaTransitionManager.Initialize();
    gameTimer.Initialize(60.0f);
    miniGameManager.Initialize();

    reasoningManager = std::make_unique<ReasoningManager>();
    reasoningManager->Initialize();
    reasoningManager->SetActive(false);

    reasoningUI = std::make_unique<ReasoningUI>();
    reasoningUI->Initialize();

    currentPhase = GamePhase::Opening;

    cameraX = -240.0f;
    player1->SetX(100.0f);

    mainbgm = rm->GetSoundResource("Resource/Sound/BGM.mp3");
    se_success = rm->GetSoundResource("Resource/Sound/GetItem.mp3");
    se_fail = rm->GetSoundResource("Resource/Sound/footSE.mp3");

    miniGameManager.SetSoundEffects(se_success, se_fail);

    if (mainbgm != -1) {
        ChangeVolumeSoundMem(180, mainbgm);
        PlaySoundMem(mainbgm, DX_PLAYTYPE_LOOP);
    }

    SetupOpeningDialogue();
}

void InGameScene::LoadCharacterImages() {
    ResourceManager* rm = ResourceManager::GetInstance();
    charHandle_Player = rm->GetImageResource("Resource/Characters/Player/chara.png")[0];
    charHandle_Police = rm->GetImageResource("Resource/Characters/NPC/Police.png")[0];
    charHandle_Fujisaki = rm->GetImageResource("Resource/Characters/Suspects/Fujisaki.png")[0];
    charHandle_Sasaki = rm->GetImageResource("Resource/Characters/Suspects/Sasaki.png")[0];
    charHandle_Kimura = rm->GetImageResource("Resource/Characters/Suspects/Kimura.png")[0];
    charHandle_Yamada = rm->GetImageResource("Resource/Characters/Suspects/Yamada.png")[0];
}

void InGameScene::SetupOpeningDialogue() {
    dialogueSystem->ClearDialogues();
    dialogueSystem->AddDialogue(DialogueData("", "13:30. You arrived at the crime scene.", CharacterPosition::Center));
    dialogueSystem->AddDialogue(DialogueData("Police", "Interview suspects and collect evidence.", CharacterPosition::Right, charHandle_Player, charHandle_Police));
    dialogueSystem->AddDialogue(DialogueData("Detective", "Understood. I will start the investigation.", CharacterPosition::Left, charHandle_Player, charHandle_Police));
    dialogueSystem->Start();
}

void InGameScene::SetupNPCDialogue(NPC* npc) {
    if (!npc) return;
    dialogueSystem->ClearDialogues();
    int targetHandle = charHandle_Police;
    std::string npcName = npc->GetName();

    const auto& testimonies = npc->GetAllTestimonies();
    for (const auto& testimony : testimonies) {
        dialogueSystem->AddDialogue(DialogueData(npcName, testimony.text, CharacterPosition::Right, charHandle_Player, targetHandle));
        if (testimony.isImportant) {
            dialogueSystem->AddDialogue(DialogueData("Detective", "This statement may be important.", CharacterPosition::Left, charHandle_Player, targetHandle));
        }
    }
    dialogueSystem->Start();
}

eSceneType InGameScene::Update(float delta_second)
{
    if (currentPhase == GamePhase::AreaTransition) {
        areaTransitionManager.Update(delta_second);
        if (!areaTransitionManager.IsTransitioning()) {
            currentPhase = GamePhase::EvidenceCollection;
            gameTimer.Resume();
        }
        return GetNowSceneType();
    }

    switch (currentPhase) {
    case GamePhase::Opening:
    case GamePhase::NPCDialogue:
        return UpdateOpeningOrNpcDialogue(delta_second);
    case GamePhase::EvidenceCollection:
        return UpdateEvidenceCollection(delta_second);
    case GamePhase::MiniGame:
        return UpdateMiniGame(delta_second);
    case GamePhase::Reasoning:
        return UpdateReasoning(delta_second);
    default:
        return GetNowSceneType();
    }
}

eSceneType InGameScene::UpdateOpeningOrNpcDialogue(float delta_second)
{
    dialogueSystem->Update(delta_second);
    if (dialogueSystem->IsFinished()) {
        if (currentPhase == GamePhase::Opening) {
            currentPhase = GamePhase::EvidenceCollection;
        }
        else {
            currentPhase = GamePhase::EvidenceCollection;
            gameTimer.Resume();
            npcManager.EndDialogue();
        }
    }
    return GetNowSceneType();
}

eSceneType InGameScene::UpdateEvidenceCollection(float delta_second)
{
    InputManager* input = InputManager::GetInstance();

    if (gameTimer.IsPaused()) {
        if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
            gameTimer.Resume();
        }
        return GetNowSceneType();
    }

    if (itemManager.IsOpen()) {
        itemManager.Update(player1->GetX(), player1->GetY(), delta_second);
        if (input->GetKeyState(KEY_INPUT_E) == eInputState::Pressed) {
            itemManager.ToggleList();
        }
        return GetNowSceneType();
    }

    player1->Update();

    if (areaTransitionManager.CheckAndStartTransition(player1->GetX())) {
        currentPhase = GamePhase::AreaTransition;
        gameTimer.Pause();
    }

    if (input->GetKeyState(KEY_INPUT_A) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_START) == eInputState::Pressed)
    {
        return eSceneType::eTitle;
    }

    if (input->GetKeyState(KEY_INPUT_E) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_Y) == eInputState::Pressed) {
        itemManager.ToggleList();
    }

    if (input->GetKeyState(KEY_INPUT_TAB) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_BACK) == eInputState::Pressed) {
        showNPCIndicator = !showNPCIndicator;
    }

    itemManager.Update(player1->GetX(), player1->GetY(), delta_second);
    npcManager.Update(player1->GetX(), player1->GetY(), delta_second);

    const auto& npcs = npcManager.GetAllNPCs();
    for (const auto& npc : npcs) {
        if (npc->IsInteracting() &&
            (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)) {
            currentPhase = GamePhase::NPCDialogue;
            gameTimer.Pause();
            npc->SetQuestioned(true);
            npc->SetState(NPCState::Talking);
            SetupNPCDialogue(npc.get());
            break;
        }
    }

    const auto& items = itemManager.GetItems();
    for (const auto& item : items) {
        if (!item) continue;
        if (!item->GetIsCollected()) {
            float dx = std::abs(player1->GetX() - item->GetX());
            float dy = std::abs(player1->GetY() - item->GetY());
            if (dx < 80.0f && dy < 150.0f) {
                if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                    input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)
                {
                    currentPhase = GamePhase::MiniGame;
                    miniGameManager.Start(item.get());
                    break;
                }
            }
        }
    }

    float areaStartX = MapData::GetAreaStartX(areaTransitionManager.GetCurrentAreaIndex());
    cameraX = (areaStartX + 400.0f) - 640.0f;

    gameTimer.Update(delta_second);

    allEvidenceCollected = itemManager.GetCollectedCount() >= itemManager.GetTotalCount();
    if (gameTimer.IsExpired() || allEvidenceCollected) {
        TransitionToReasoning();
    }

    return GetNowSceneType();
}

eSceneType InGameScene::UpdateMiniGame(float delta_second)
{
    float remainingTime = gameTimer.GetRemainingTime();
    miniGameManager.Update(delta_second, remainingTime);

    if (remainingTime < gameTimer.GetRemainingTime()) {
        gameTimer.SubtractTime(gameTimer.GetRemainingTime() - remainingTime);
    }

    if (!miniGameManager.IsActive()) {
        currentPhase = GamePhase::EvidenceCollection;
        gameTimer.Resume();
    }

    gameTimer.Update(delta_second);
    if (gameTimer.IsExpired()) {
        TransitionToReasoning();
    }

    return GetNowSceneType();
}

eSceneType InGameScene::UpdateReasoning(float delta_second)
{
    InputManager* input = InputManager::GetInstance();

    if (evidenceResultTimer > 0.0f) {
        evidenceResultTimer -= delta_second;
        return GetNowSceneType();
    }

    if (isSelectingEvidence) {
        itemManager.Update(0, 0, delta_second);
        if (input->GetKeyState(KEY_INPUT_E) == eInputState::Pressed ||
            input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_B) == eInputState::Pressed) {
            itemManager.ToggleList();
            isSelectingEvidence = false;
        }
        if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
            const Item* selectedItem = itemManager.GetSelectedItem();
            if (selectedItem && selectedItem->GetIsCollected()) {
                int result = reasoningManager->TryEliminateWithEvidence(selectedItem->GetName());
                if (result == 0) {
                    evidenceResultMessage = "Correct";
                    if (se_success != -1) PlaySoundMem(se_success, DX_PLAYTYPE_BACK);
                }
                else if (result == 1) {
                    evidenceResultMessage = "Wrong evidence. You lose one life.";
                    reasoningManager->DamageLife();
                    if (se_fail != -1) PlaySoundMem(se_fail, DX_PLAYTYPE_BACK);
                }
                else {
                    evidenceResultMessage = "Evidence cannot be used now.";
                }
                evidenceResultTimer = 1.5f;
                itemManager.ToggleList();
                isSelectingEvidence = false;
            }
        }
        return GetNowSceneType();
    }

    if (showResult) {
        resultDisplayTime += delta_second;
        if (resultDisplayTime >= 3.0f) {
            return eSceneType::eTitle;
        }
    }
    else {
        reasoningManager->Update(delta_second);

        if (input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_Y) == eInputState::Pressed) {
            isSelectingEvidence = true;
            itemManager.ToggleList();
        }

        if (reasoningManager->IsGameOver()) {
            isCorrect = false;
            isGameOver = true;
            showResult = true;
            resultDisplayTime = 0.0f;
            if (se_fail != -1) PlaySoundMem(se_fail, DX_PLAYTYPE_BACK);
        }
        else if (reasoningManager->IsGameClear()) {
            isCorrect = true;
            isGameOver = false;
            showResult = true;
            resultDisplayTime = 0.0f;
            if (se_success != -1) PlaySoundMem(se_success, DX_PLAYTYPE_BACK);
        }
    }

    return GetNowSceneType();
}

void InGameScene::Draw() const
{
    int currentAreaIndex = areaTransitionManager.GetCurrentAreaIndex();

    if (currentPhase == GamePhase::Opening || currentPhase == GamePhase::NPCDialogue) {
        backgroundManager.DrawBackground(currentAreaIndex, true);
    }
    else {
        backgroundManager.DrawBackground(currentAreaIndex, false);
        backgroundManager.DrawBackgroundObjects(cameraX);
    }

    switch (currentPhase) {
    case GamePhase::Opening:
    case GamePhase::NPCDialogue:
        DrawOpeningOrNpcDialogue();
        break;
    case GamePhase::EvidenceCollection:
        DrawEvidenceCollection();
        break;
    case GamePhase::AreaTransition:
        DrawAreaTransition();
        break;
    case GamePhase::MiniGame:
        DrawMiniGame();
        break;
    case GamePhase::Reasoning:
        DrawReasoning();
        break;
    default:
        break;
    }

    if (itemManager.IsOpen() && currentPhase != GamePhase::Reasoning) {
        itemManager.Draw(cameraX);
    }
}

void InGameScene::DrawOpeningOrNpcDialogue() const
{
    if (currentPhase == GamePhase::Opening) {
        dialogueSystem->Draw();
        return;
    }

    g_floor.Draw(cameraX);
    npcManager.Draw(cameraX);
    itemManager.Draw(cameraX);
    player1->Draw(cameraX);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    dialogueSystem->Draw();
}

void InGameScene::DrawEvidenceCollection() const
{
    int currentAreaIndex = areaTransitionManager.GetCurrentAreaIndex();

    g_floor.Draw(cameraX);
    npcManager.Draw(cameraX);
    itemManager.Draw(cameraX);
    player1->Draw(cameraX);

    if (gameTimer.IsPaused()) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawFormatString(300, 320, GetColor(255, 255, 255), "Press A to resume");
    }
    else if (!itemManager.IsOpen()) {
        gameTimer.Draw();
        DrawPhaseInfo();
        const char* areas[] = { "Area0", "Area1", "Area2", "Area3", "Area4" };
        int areaIdx = currentAreaIndex;
        if (areaIdx < 0) areaIdx = 0;
        if (areaIdx > 4) areaIdx = 4;
        DrawFormatString(240, 20, GetColor(200, 255, 255), "Current area: %s", areas[areaIdx]);

        if (currentAreaIndex < 4) DrawFormatString(1280 - 150, 360, GetColor(255, 255, 0), "-> Area");
        if (currentAreaIndex > 0) DrawFormatString(50, 360, GetColor(255, 255, 0), "Area <-");

        if (showNPCIndicator && !npcManager.IsDialogueActive()) {
            DrawNPCIndicator();
        }
    }
}

void InGameScene::DrawAreaTransition() const
{
    g_floor.Draw(cameraX);
    itemManager.Draw(cameraX);
    player1->Draw(cameraX);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)areaTransitionManager.GetFadeAlpha());
    DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void InGameScene::DrawMiniGame() const
{
    g_floor.Draw(cameraX);
    itemManager.Draw(cameraX);
    player1->Draw(cameraX);
    gameTimer.Draw();
    miniGameManager.Draw();
}

void InGameScene::DrawReasoning() const
{
    if (showResult) {
        DrawResult();
        return;
    }

    int bgImage = backgroundManager.GetMainBackgroundImage();
    if (bgImage != -1) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
        DrawGraph(0, 0, bgImage, TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }

    reasoningUI->Draw(reasoningManager.get());

    if (isSelectingEvidence) {
        itemManager.Draw(cameraX);
        DrawFormatString(140, 580, GetColor(255, 255, 0), "Select evidence and press A.");
    }

    if (evidenceResultTimer > 0.0f) {
        DrawBox(300, 300, 980, 420, GetColor(0, 0, 0), TRUE);
        DrawBox(300, 300, 980, 420, GetColor(255, 255, 255), FALSE);
        DrawFormatString(350, 350, GetColor(255, 255, 255), "%s", evidenceResultMessage.c_str());
    }
}

void InGameScene::DrawNPCIndicator() const {
    if (!player1) return;
    float playerX = player1->GetX();
    const auto& npcs = npcManager.GetAllNPCs();
    int indicatorY = 120;
    int indicatorHeight = 60;
    DrawBox(20, indicatorY, 320, indicatorY + indicatorHeight, GetColor(0, 0, 0), TRUE);
    DrawBox(20, indicatorY, 320, indicatorY + indicatorHeight, GetColor(100, 100, 100), FALSE);
    DrawFormatString(30, indicatorY + 5, GetColor(255, 200, 100), "NPC Indicator");
    int yOffset = indicatorY + 30;
    for (const auto& npc : npcs) {
        float npcX = npc->GetX();
        float distance = abs(npcX - playerX);
        unsigned int color = GetColor(255, 255, 255);
        std::string direction = "";
        if (distance < 100.0f) { color = GetColor(100, 255, 100); direction = "*"; }
        else if (npcX > playerX) { color = GetColor(200, 200, 255); direction = "->"; }
        else { color = GetColor(255, 200, 200); direction = "<-"; }
        std::string mark = npc->HasBeenQuestioned() ? "[DONE]" : "";
        DrawFormatString(30, yOffset, color, "%s %s %s", direction.c_str(), npc->GetName().c_str(), mark.c_str());
        yOffset += 20;
        if (yOffset > indicatorY + indicatorHeight - 10) break;
    }
}

int InGameScene::CountQuestionedNPCs() const {
    int count = 0;
    const auto& npcs = npcManager.GetAllNPCs();
    for (const auto& npc : npcs) {
        if (npc->HasBeenQuestioned()) count++;
    }
    return count;
}

void InGameScene::Finalize() {
    if (mainbgm >= 0) {
        StopSoundMem(mainbgm);
        mainbgm = -1;
    }
    se_success = -1;
    se_fail = -1;

    charHandle_Player = -1;
    charHandle_Police = -1;
    charHandle_Fujisaki = -1;
    charHandle_Sasaki = -1;
    charHandle_Kimura = -1;
    charHandle_Yamada = -1;

    player1.reset();
    reasoningManager.reset();
    reasoningUI.reset();
    dialogueSystem.reset();
}

eSceneType InGameScene::GetNowSceneType() const { return eSceneType::eInGame; }

void InGameScene::TransitionToReasoning() {
    if (player1) player1->StopAudio();
    currentPhase = GamePhase::Reasoning;
    if (reasoningManager) {
        std::vector<std::string> collected = itemManager.GetCollectedItems();
        reasoningManager->SetActive(true);
    }
}

void InGameScene::DrawPhaseInfo() const {
    DrawFormatString(20, 50, GetColor(200, 200, 200), "Evidence: %d / %d  Questioned NPC: %d/4",
        itemManager.GetCollectedCount(), itemManager.GetTotalCount(), CountQuestionedNPCs());
}

void InGameScene::DrawResult() const {
    int x = 200, y = 150;
    int boxWidth = 880, boxHeight = 420;
    DrawBox(x - 20, y - 20, x + boxWidth, y + boxHeight, GetColor(0, 0, 0), TRUE);
    DrawBox(x - 20, y - 20, x + boxWidth, y + boxHeight, GetColor(255, 255, 255), FALSE);
    if (isCorrect) {
        SetFontSize(40);
        DrawFormatString(x + 280, y + 20, GetColor(0, 255, 0), "CASE CLOSED");
        SetFontSize(24);
        DrawFormatString(x + 320, y + 70, GetColor(255, 255, 0), "Solved");
        SetFontSize(16);
        DrawFormatString(x + 40, y + 150, GetColor(255, 255, 255), "You identified the true culprit.");
    }
    else {
        SetFontSize(40);
        DrawFormatString(x + 280, y + 20, GetColor(255, 0, 0), "CASE FAILED");
        SetFontSize(24);
        DrawFormatString(x + 320, y + 70, GetColor(255, 150, 0), "Unsolved");
        SetFontSize(16);
        DrawFormatString(x + 40, y + 150, GetColor(255, 255, 255), "Investigation ended without enough proof.");
    }
}

void InGameScene::DrawOpening() const {}
void InGameScene::DrawAreaInfo() const {}
