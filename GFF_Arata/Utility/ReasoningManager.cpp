#include "ReasoningManager.h"
#include "DxLib.h"
#include "InputManager.h"
#include "ChoiceEffect.h"
#include <algorithm>

ReasoningManager::ReasoningManager()
    : currentStep(ReasoningStep::Suspect), selectedIndex(0), isActive(false),
    chosenSuspectIndex(-1), chosenMotiveIndex(-1), chosenWeaponIndex(-1),
    maxLife(3), currentLife(3)
{
}

void ReasoningManager::Initialize() {
    suspects.clear();
    motives.clear();
    weapons.clear();
    currentStep = ReasoningStep::Suspect;
    selectedIndex = 0;
    currentLife = maxLife;
    lastHintMessage = "";

    // === 犯人候補 ===
    // 藤崎美咲：防犯カメラ（時間不一致）やレジ記録で不在が証明される
    suspects.push_back(ReasoningElement("藤崎 美咲（元従業員）",
        { "防犯カメラの映像", "壊れたレジの記録" }, false));

    // 佐々木健太：手帳（予定なし）や窓ガラス（内部犯行示唆）で除外
    suspects.push_back(ReasoningElement("佐々木 健太（隣の店主）",
        { "店主の手帳", "割れた窓ガラス" }, false));

    // 木村達也：真犯人（否定証拠なし）
    suspects.push_back(ReasoningElement("木村 達也（被害者の弟）",
        {}, true));

    // 山田一郎：防犯カメラに映っていない、レジ記録の時間と矛盾
    suspects.push_back(ReasoningElement("山田 一郎（常連客）",
        { "防犯カメラの映像", "壊れたレジの記録" }, false));

    // === 動機候補 ===
    // 不当解雇：藤崎のアリバイが証明されれば消える
    motives.push_back(ReasoningElement("不当解雇の恨み",
        { "防犯カメラの映像" }, false));

    // 商売敵：佐々木が除外されれば消える
    motives.push_back(ReasoningElement("商売敵としての排除",
        { "割れた窓ガラス" }, false));

    // 遺産相続・借金：正解
    motives.push_back(ReasoningElement("遺産相続と借金",
        {}, true));

    // 過去の確執：山田が除外されれば消える
    motives.push_back(ReasoningElement("政治談義での確執",
        { "壊れたレジの記録" }, false));

    // === 凶器候補 ===
    // 金属バット：正解
    weapons.push_back(ReasoningElement("血のついた金属バット",
        {}, true));

    // 鈍器（商品）：本物の凶器（バット）が見つかれば除外
    weapons.push_back(ReasoningElement("店内の商品（鈍器）",
        { "血のついた金属バット" }, false));

    // ガラス片：窓ガラスの詳細（内側から割れた＝凶器ではない）で除外
    weapons.push_back(ReasoningElement("ガラスの破片",
        { "割れた窓ガラス" }, false));

    // 素手：バットが見つかれば除外
    weapons.push_back(ReasoningElement("素手・殴打",
        { "血のついた金属バット" }, false));
}

void ReasoningManager::Update(float deltaTime) {
    if (!isActive) return;

    UpdateChoiceEffects(deltaTime);

    InputManager* input = InputManager::GetInstance();
    const auto& options = GetCurrentOptions();

    if (!options.empty()) {
        if (input->GetKeyState(KEY_INPUT_UP) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_DPAD_UP) == eInputState::Pressed) {
            int originalIndex = selectedIndex;
            do {
                selectedIndex--;
                if (selectedIndex < 0) selectedIndex = (int)options.size() - 1;
            } while (options[selectedIndex].isEliminated && selectedIndex != originalIndex);
        }

        if (input->GetKeyState(KEY_INPUT_DOWN) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_DPAD_DOWN) == eInputState::Pressed) {
            int originalIndex = selectedIndex;
            do {
                selectedIndex++;
                if (selectedIndex >= (int)options.size()) selectedIndex = 0;
            } while (options[selectedIndex].isEliminated && selectedIndex != originalIndex);
        }
    }

    if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
        ConfirmSelection();
    }

    if ((input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
        input->GetButtonState(XINPUT_BUTTON_B) == eInputState::Pressed) && currentStep != ReasoningStep::Suspect) {

        if (currentStep == ReasoningStep::Motive) currentStep = ReasoningStep::Suspect;
        else if (currentStep == ReasoningStep::Weapon) currentStep = ReasoningStep::Motive;
        else if (currentStep == ReasoningStep::Confirmation) currentStep = ReasoningStep::Weapon;

        selectedIndex = 0;
        lastHintMessage = "";
    }
}

void ReasoningManager::ConfirmSelection() {
    if (currentStep == ReasoningStep::Suspect) {
        chosenSuspectIndex = selectedIndex;
        currentStep = ReasoningStep::Motive;
        selectedIndex = 0;
    }
    else if (currentStep == ReasoningStep::Motive) {
        chosenMotiveIndex = selectedIndex;
        currentStep = ReasoningStep::Weapon;
        selectedIndex = 0;
    }
    else if (currentStep == ReasoningStep::Weapon) {
        chosenWeaponIndex = selectedIndex;
        currentStep = ReasoningStep::Confirmation;
        selectedIndex = 0;
    }
    else if (currentStep == ReasoningStep::Confirmation) {
        if (selectedIndex == 0) {
            CheckAnswer();
        }
        else {
            ResetToFirstStep();
        }
    }

    if (currentStep != ReasoningStep::Confirmation) {
        const auto& options = GetCurrentOptions();
        for (int i = 0; i < options.size(); i++) {
            if (!options[i].isEliminated) {
                selectedIndex = i;
                break;
            }
        }
    }
}

void ReasoningManager::CheckAnswer() {
    bool isSuspectCorrect = suspects[chosenSuspectIndex].isCorrect;
    bool isMotiveCorrect = motives[chosenMotiveIndex].isCorrect;
    bool isWeaponCorrect = weapons[chosenWeaponIndex].isCorrect;

    if (isSuspectCorrect && isMotiveCorrect && isWeaponCorrect) {
        lastHintMessage = "推理的中！これが真実だ！";
    }
    else {
        currentLife--;

        if (!isSuspectCorrect) {
            lastHintMessage = "犯人が違うようだ...アリバイを確認しろ。";
        }
        else if (!isMotiveCorrect) {
            lastHintMessage = "犯人は合っているが、動機が違う。手帳などを見たか？";
        }
        else if (!isWeaponCorrect) {
            lastHintMessage = "凶器が違う。現場に落ちていたアレだ！";
        }

        if (currentLife > 0) {
            ResetToFirstStep();
        }
    }
}

void ReasoningManager::ResetToFirstStep() {
    currentStep = ReasoningStep::Suspect;
    selectedIndex = 0;
    for (int i = 0; i < suspects.size(); i++) {
        if (!suspects[i].isEliminated) {
            selectedIndex = i;
            break;
        }
    }
}

void ReasoningManager::FilterOptions(const std::vector<std::string>& collectedEvidence) {
    auto filter = [&](std::vector<ReasoningElement>& list) {
        for (auto& item : list) {
            if (item.contradictoryEvidences.empty()) continue;

            // 設定された否定証拠のいずれか一つでも持っていれば除外
            for (const auto& contra : item.contradictoryEvidences) {
                for (const auto& evidence : collectedEvidence) {
                    if (evidence == contra) {
                        item.isEliminated = true;
                        item.effectState = ChoiceEffectState::Locked;
                        goto NextItem; // 二重ループ脱出
                    }
                }
            }
        NextItem:;
        }
        };

    filter(suspects);
    filter(motives);
    filter(weapons);
}

void ReasoningManager::UpdateChoiceEffects(float deltaTime) {
    for (auto& item : suspects) ChoiceEffectManager::UpdateChoiceEffect(item, deltaTime);
    for (auto& item : motives) ChoiceEffectManager::UpdateChoiceEffect(item, deltaTime);
    for (auto& item : weapons) ChoiceEffectManager::UpdateChoiceEffect(item, deltaTime);
}

const std::vector<ReasoningElement>& ReasoningManager::GetCurrentOptions() const {
    if (currentStep == ReasoningStep::Suspect) return suspects;
    if (currentStep == ReasoningStep::Motive) return motives;
    if (currentStep == ReasoningStep::Weapon) return weapons;

    // ★修正: 第2引数を "" から {} に変更しました
    static std::vector<ReasoningElement> confirmationDummy = {
        ReasoningElement("推理を実行する", {}, false),
        ReasoningElement("もう一度考え直す", {}, false)
    };
    return confirmationDummy;
}

std::string ReasoningManager::GetStepTitle() const {
    switch (currentStep) {
    case ReasoningStep::Suspect: return "Step 1: 犯人は誰だ？";
    case ReasoningStep::Motive:  return "Step 2: 動機は何だ？";
    case ReasoningStep::Weapon:  return "Step 3: 凶器は何だ？";
    case ReasoningStep::Confirmation: return "最終確認：これで間違いないか？";
    default: return "";
    }
}

std::string ReasoningManager::GetCurrentSelectionText() const {
    std::string s = (chosenSuspectIndex >= 0) ? suspects[chosenSuspectIndex].text : "???";
    std::string m = (chosenMotiveIndex >= 0) ? motives[chosenMotiveIndex].text : "???";
    std::string w = (chosenWeaponIndex >= 0) ? weapons[chosenWeaponIndex].text : "???";
    return "犯人:" + s + " / 動機:" + m + " / 凶器:" + w;
}

bool ReasoningManager::IsGameClear() const {
    return currentLife > 0 && lastHintMessage == "推理的中！これが真実だ！";
}

bool ReasoningManager::IsGameOver() const {
    return currentLife <= 0;
}

void ReasoningManager::Draw() const {}