#include "ItemManager.h"
#include "DxLib.h"
#include "InputManager.h"
#include <algorithm>

void ItemManager::Add(std::unique_ptr<Item> item) {
    if (item) {
        item->Init();
        items.push_back(std::move(item));
    }
}

void ItemManager::Init() {
    mode = ItemMode::List;
    selectedIndex = 0;
    isListOpen = false;
}

void ItemManager::Update(float playerX, float playerY, float deltaTime) {
    for (const auto& item : items) {
        item->Update(playerX, playerY, deltaTime);
    }

    if (isListOpen) {
        InputManager* input = InputManager::GetInstance();
        if (items.empty()) return;

        if (selectedIndex < 0 || selectedIndex >= (int)items.size()) {
            selectedIndex = 0;
        }

        if (input->GetKeyState(KEY_INPUT_UP) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_DPAD_UP) == eInputState::Pressed) {
            selectedIndex--;
            if (selectedIndex < 0) selectedIndex = (int)items.size() - 1;
        }
        if (input->GetKeyState(KEY_INPUT_DOWN) == eInputState::Pressed ||
            input->GetButtonState(XINPUT_BUTTON_DPAD_DOWN) == eInputState::Pressed) {
            selectedIndex++;
            if (selectedIndex >= (int)items.size()) selectedIndex = 0;
        }
    }
}

void ItemManager::DrawPixelArtBox(int x, int y, int width, int height) const {
    DrawBox(x, y, x + width, y + height, GetColor(30, 30, 35), TRUE);
    unsigned int borderColor = GetColor(200, 200, 200);
    DrawBox(x, y, x + width, y + height, borderColor, FALSE);
    DrawBox(x + 2, y + 2, x + width - 2, y + height - 2, borderColor, FALSE);
}

void ItemManager::Draw(float cameraOffsetX) const {
    for (const auto& item : items) {
        item->Draw(cameraOffsetX);
    }

    if (isListOpen) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
        DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        int winX = 140, winY = 100, winW = 1000, winH = 520;
        DrawPixelArtBox(winX, winY, winW, winH);

        SetFontSize(28);
        DrawFormatString(winX + 20, winY + 20, GetColor(255, 255, 100), "【証拠品リスト】");
        SetFontSize(16);

        int listX = winX + 40, listY = winY + 80, itemHeight = 40, listW = 450;
        DrawBox(listX - 10, listY - 10, listX + listW + 10, winY + winH - 30, GetColor(40, 40, 45), TRUE);
        DrawBox(listX - 10, listY - 10, listX + listW + 10, winY + winH - 30, GetColor(150, 150, 150), FALSE);

        if (items.empty()) {
            DrawFormatString(listX, listY, GetColor(255, 255, 255), "アイテムがありません");
        }
        else {
            for (int i = 0; i < (int)items.size(); i++) {
                int drawY = listY + i * itemHeight;
                bool isSelected = (i == selectedIndex);
                bool isCollected = items[i]->GetIsCollected();

                if (isSelected) {
                    DrawBox(listX - 5, drawY, listX + listW + 5, drawY + itemHeight - 10, GetColor(80, 80, 120), TRUE);
                    DrawTriangle(listX, drawY + 10, listX, drawY + 25, listX + 10, drawY + 18, GetColor(255, 255, 0), TRUE);
                }

                unsigned int textColor = isSelected ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
                if (isCollected) {
                    DrawFormatString(listX + 20, drawY + 5, textColor, "%s", items[i]->GetName().c_str());
                }
                else {
                    DrawFormatString(listX + 20, drawY + 5, GetColor(100, 100, 100), "？？？？？？");
                }
            }
        }

        int detailX = winX + 530, detailY = winY + 80, detailW = 430, detailH = 400;
        DrawBox(detailX, detailY, detailX + detailW, detailY + detailH, GetColor(50, 50, 60), TRUE);
        DrawBox(detailX, detailY, detailX + detailW, detailY + detailH, GetColor(150, 150, 150), FALSE);

        if (!items.empty()) {
            const Item* selectedItem = items[selectedIndex].get();
            if (selectedItem->GetIsCollected()) {
                SetFontSize(24);
                DrawFormatString(detailX + 20, detailY + 20, GetColor(100, 255, 255), "%s", selectedItem->GetName().c_str());
                SetFontSize(16);

                std::string desc = selectedItem->GetDescription();
                int currentDescY = detailY + 70;
                size_t start = 0;
                size_t end = desc.find('\n');
                while (end != std::string::npos) {
                    DrawFormatString(detailX + 20, currentDescY, GetColor(255, 255, 255), "%s", desc.substr(start, end - start).c_str());
                    currentDescY += 25;
                    start = end + 1;
                    end = desc.find('\n', start);
                }
                DrawFormatString(detailX + 20, currentDescY, GetColor(255, 255, 255), "%s", desc.substr(start, desc.length() - start).c_str());
            }
            else {
                DrawFormatString(detailX + 150, detailY + 180, GetColor(150, 150, 150), "未入手の証拠です");
            }
        }

        DrawFormatString(winX + 20, winY + winH - 30, GetColor(200, 200, 200),
            "↑↓: 選択　A: 決定(推理時)  B: 閉じる");
    }
}

void ItemManager::ToggleList() {
    isListOpen = !isListOpen;
    if (isListOpen) {
        mode = ItemMode::List;
        selectedIndex = 0;
    }
}

const Item* ItemManager::GetSelectedItem() const {
    if (items.empty() || selectedIndex < 0 || selectedIndex >= (int)items.size()) return nullptr;
    return items[selectedIndex].get();
}

std::vector<std::string> ItemManager::GetCollectedItems() const {
    std::vector<std::string> collected;
    for (const auto& item : items) {
        if (item->GetIsCollected()) {
            collected.push_back(item->GetName());
        }
    }
    return collected;
}

int ItemManager::GetCollectedCount() const {
    int count = 0;
    for (const auto& item : items) {
        if (item->GetIsCollected()) {
            count++;
        }
    }
    return count;
}
