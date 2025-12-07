#include "NPCManager.h"
#include "InputManager.h"

NPCManager::NPCManager()
    : currentTalkingNPC(nullptr),
    isDialogueActive(false),
    dialogueBoxHandle(-1),
    dialogueTimer(0.0f)
{
}

NPCManager::~NPCManager() {
    if (dialogueBoxHandle != -1) {
        DeleteGraph(dialogueBoxHandle);
    }
}

void NPCManager::Initialize() {
    npcs.clear();

    // 容疑者配置（指定の座標に配置）
    // 裏口エリア - 藤崎美咲
    AddSuspect(SuspectType::Fujisaki, 600.0f, 560.0f);

    // 店内奥エリア - 木村達也（犯人）
    AddSuspect(SuspectType::Kimura, 1500.0f, 560.0f);

    // レジ周辺エリア - 佐々木健太
    AddSuspect(SuspectType::Sasaki, 2300.0f, 560.0f);

    // 店外エリア - 山田一郎
    AddSuspect(SuspectType::Yamada, 3500.0f, 560.0f);

    // 各NPCの初期化
    for (auto& npc : npcs) {
        npc->Initialize();
    }
}

void NPCManager::Update(float playerX, float playerY, float deltaTime) {
    InputManager* input = InputManager::GetInstance();

    // ダイアログ表示中の処理
    if (isDialogueActive) {
        dialogueTimer += deltaTime;

        // Zキーで次の証言へ
        if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {

            if (currentTalkingNPC && currentTalkingNPC->HasMoreTestimonies()) {
                currentTalkingNPC->NextTestimony();
                dialogueTimer = 0.0f;
            }
            else {
                EndDialogue();
            }
        }

        // Xキーで会話終了
        if (input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_B) == eInputState::Pressed) {
            EndDialogue();
        }

        return;  // ダイアログ中は他の処理をスキップ
    }

    // 各NPCの更新
    for (auto& npc : npcs) {
        npc->Update(playerX, playerY, deltaTime);

        // Zキーで会話開始
        if (npc->IsInteracting() &&
            (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)) {
            StartDialogue(npc.get());
            break;
        }
    }
}

void NPCManager::Draw(float cameraOffsetX) const {
    // 各NPCの描画
    for (const auto& npc : npcs) {
        npc->Draw(cameraOffsetX);
    }

    // ダイアログ描画
    if (isDialogueActive) {
        DrawDialogue();
    }
}

void NPCManager::AddSuspect(SuspectType type, float x, float y) {
    npcs.push_back(std::make_unique<Suspect>(x, y, type));
}

void NPCManager::AddNPC(NPC* npc) {
    npcs.push_back(std::unique_ptr<NPC>(npc));
}

void NPCManager::StartDialogue(NPC* npc) {
    currentTalkingNPC = npc;
    isDialogueActive = true;
    dialogueTimer = 0.0f;

    npc->SetState(NPCState::Talking);
    npc->SetQuestioned(true);
    npc->ResetTestimonies();
}

void NPCManager::EndDialogue() {
    if (currentTalkingNPC) {
        currentTalkingNPC->SetState(NPCState::Idle);
    }

    currentTalkingNPC = nullptr;
    isDialogueActive = false;
}

void NPCManager::NextDialogue() {
    if (currentTalkingNPC && currentTalkingNPC->HasMoreTestimonies()) {
        currentTalkingNPC->NextTestimony();
        dialogueTimer = 0.0f;
    }
}

void NPCManager::DrawDialogue() const {
    if (!currentTalkingNPC) return;

    const Testimony* testimony = currentTalkingNPC->GetCurrentTestimony();
    if (!testimony) return;

    // ダイアログボックスの描画
    int boxX = 100;
    int boxY = 450;
    int boxWidth = 1080;
    int boxHeight = 220;

    // 背景
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
    DrawBox(boxX, boxY, boxX + boxWidth, boxY + boxHeight,
        GetColor(20, 20, 40), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 枠
    DrawBox(boxX, boxY, boxX + boxWidth, boxY + boxHeight,
        GetColor(200, 200, 200), FALSE);
    DrawBox(boxX + 2, boxY + 2, boxX + boxWidth - 2, boxY + boxHeight - 2,
        GetColor(150, 150, 150), FALSE);

    // 名前表示
    DrawBox(boxX + 20, boxY - 30, boxX + 200, boxY,
        GetColor(40, 40, 80), TRUE);
    DrawBox(boxX + 20, boxY - 30, boxX + 200, boxY,
        GetColor(200, 200, 200), FALSE);

    DrawFormatString(boxX + 40, boxY - 23, GetColor(255, 255, 255),
        "%s", currentTalkingNPC->GetName().c_str());

    // 証言内容
    SetFontSize(20);
    int textX = boxX + 30;
    int textY = boxY + 30;

    // 重要な証言は色を変える
    unsigned int textColor = testimony->isImportant ?
        GetColor(255, 255, 100) : GetColor(255, 255, 255);

    // 改行対応（簡易版：\nで改行）
    std::string text = testimony->text;
    size_t pos = 0;
    int lineHeight = 30;

    while (pos < text.length()) {
        size_t newlinePos = text.find('\n', pos);
        if (newlinePos == std::string::npos) newlinePos = text.length();

        std::string line = text.substr(pos, newlinePos - pos);
        DrawFormatString(textX, textY, textColor, "%s", line.c_str());

        textY += lineHeight;
        pos = newlinePos + 1;
    }

    SetFontSize(16);

    // 操作説明
    int guideY = boxY + boxHeight - 35;

    if (currentTalkingNPC->HasMoreTestimonies()) {
        DrawFormatString(boxX + 30, guideY, GetColor(200, 200, 200),
            "[Z] 次へ  [X] 終了");
    }
    else {
        DrawFormatString(boxX + 30, guideY, GetColor(200, 200, 200),
            "[Z] または [X] で終了");
    }

    // 証拠が必要な場合の表示
    if (testimony->requiresEvidence && !testimony->requiredEvidence.empty()) {
        DrawFormatString(boxX + boxWidth - 300, guideY, GetColor(255, 100, 100),
            "※ 要証拠: %s", testimony->requiredEvidence.c_str());
    }

    // ページインジケーター
    int currentPage = currentTalkingNPC->GetCurrentTestimony() -
        &(*currentTalkingNPC->GetAllTestimonies().begin());
    int totalPages = (int)currentTalkingNPC->GetAllTestimonies().size();

    DrawFormatString(boxX + boxWidth - 100, boxY + 10, GetColor(150, 150, 150),
        "%d / %d", currentPage + 1, totalPages);
}