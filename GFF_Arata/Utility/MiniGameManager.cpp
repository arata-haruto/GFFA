#include "MiniGameManager.h"
#include "InputManager.h"

const float SCREEN_WIDTH = 1280.0f;
const float SCREEN_HEIGHT = 720.0f;

MiniGameManager::MiniGameManager()
    : state(MiniGameState::Inactive),
    barPosition(0.0f),
    barSpeed(200.0f),
    targetMin(40.0f),
    targetMax(60.0f),
    targetItem(nullptr),
    resultTimer(0.0f),
    lastResultSuccess(false),
    seSuccess(-1),
    seFail(-1)
{
}

MiniGameManager::~MiniGameManager()
{
}

void MiniGameManager::Initialize()
{
    state = MiniGameState::Inactive;
    barPosition = 0.0f;
    resultTimer = 0.0f;
    lastResultSuccess = false;
    targetItem = nullptr;
}

void MiniGameManager::SetSoundEffects(int successSE, int failSE)
{
    seSuccess = successSE;
    seFail = failSE;
}

void MiniGameManager::Start(Item* item)
{
    state = MiniGameState::Playing;
    targetItem = item;
    barPosition = 0.0f;
    barSpeed = 200.0f;
    targetMin = 40.0f;
    targetMax = 60.0f;
    resultTimer = 0.0f;
}

void MiniGameManager::Update(float deltaTime, float& remainingTime)
{
    if (state == MiniGameState::Inactive) {
        return;
    }

    if (state == MiniGameState::ShowResult) {
        resultTimer -= deltaTime;
        if (resultTimer <= 0.0f) {
            if (lastResultSuccess && targetItem) {
                targetItem->SetCollected(true);
            }
            state = MiniGameState::Inactive;
            targetItem = nullptr;
        }
        return;
    }

    // Playing state
    InputManager* input = InputManager::GetInstance();
    barPosition += barSpeed * deltaTime;
    if (barPosition > 100.0f) {
        barPosition = 0.0f;
    }

    if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed)
    {
        if (barPosition >= targetMin && barPosition <= targetMax) {
            if (seSuccess != -1) PlaySoundMem(seSuccess, DX_PLAYTYPE_BACK);
            lastResultSuccess = true;
        }
        else {
            if (seFail != -1) PlaySoundMem(seFail, DX_PLAYTYPE_BACK);
            remainingTime -= 10.0f;
            lastResultSuccess = false;
        }
        state = MiniGameState::ShowResult;
        resultTimer = 1.0f;
    }
}

void MiniGameManager::Draw() const
{
    if (state == MiniGameState::Inactive) {
        return;
    }

    int cx = (int)SCREEN_WIDTH / 2;
    int cy = (int)SCREEN_HEIGHT / 2;
    int w = 400;
    int h = 60;

    DrawBox(cx - w / 2 - 5, cy - h / 2 - 5, cx + w / 2 + 5, cy + h / 2 + 5, GetColor(255, 255, 255), FALSE);
    DrawBox(cx - w / 2, cy - h / 2, cx + w / 2, cy + h / 2, GetColor(30, 30, 30), TRUE);

    if (state == MiniGameState::ShowResult) {
        if (lastResultSuccess) {
            DrawFormatString(cx - 60, cy - 10, GetColor(0, 255, 0), "GET EVIDENCE!");
        }
        else {
            DrawFormatString(cx - 40, cy - 10, GetColor(255, 0, 0), "FAILED...");
            DrawFormatString(cx - 50, cy + 20, GetColor(255, 100, 100), "-10 Seconds");
        }
        return;
    }

    // Playing state - draw target and bar
    if (targetItem) {
        DrawFormatString(cx - 100, cy - 60, GetColor(255, 255, 0), "TARGET: %s", targetItem->GetName().c_str());
    }

    int z1 = cx - w / 2 + (int)(w * (targetMin / 100.0f));
    int z2 = cx - w / 2 + (int)(w * (targetMax / 100.0f));
    DrawBox(z1, cy - h / 2, z2, cy + h / 2, GetColor(0, 255, 0), TRUE);

    int bx = cx - w / 2 + (int)(w * (barPosition / 100.0f));
    DrawBox(bx - 3, cy - h / 2 - 5, bx + 3, cy + h / 2 + 5, GetColor(255, 50, 50), TRUE);

    DrawFormatString(cx - 80, cy + 40, GetColor(255, 255, 255), "Aボタンでタイミングよく止めろ！");
}

bool MiniGameManager::IsActive() const
{
    return state != MiniGameState::Inactive;
}

bool MiniGameManager::IsCompleted() const
{
    return state == MiniGameState::Inactive && resultTimer <= 0.0f;
}

bool MiniGameManager::WasSuccessful() const
{
    return lastResultSuccess;
}

Item* MiniGameManager::GetTargetItem() const
{
    return targetItem;
}

void MiniGameManager::Reset()
{
    state = MiniGameState::Inactive;
    targetItem = nullptr;
    resultTimer = 0.0f;
}
