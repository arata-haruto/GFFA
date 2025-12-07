#include "ItemManager.h"
#include "DxLib.h"
#include "InputManager.h"
#include <algorithm>

struct NamedItem {
    Item* item;
    std::string name;
};

ItemManager::~ItemManager() {
    for (auto& item : items) {
        delete item;
    }
    items.clear();
}

void ItemManager::Add(Item* item) {
    if (item) {
        item->Init();
        items.push_back(item);
    }
}

void ItemManager::Init() {
    mode = ItemMode::List;
    selectedIndex = 0;
    isListOpen = false;
    inputCooldown = 0.0f;
}

void ItemManager::Update(float playerX, float playerY, float deltaTime) {
    // リストが開いていない時のみアイテム取得を処理
    if (!isListOpen) {
        for (size_t i = 0; i < items.size(); ++i) {
            items[i]->Update(playerX, playerY, deltaTime);
        }
    }

    // 入力クールダウン更新
    if (inputCooldown > 0.0f) {
        inputCooldown -= deltaTime;
        if (inputCooldown < 0.0f) inputCooldown = 0.0f;
    }

    if (isListOpen) {
        InputManager* input = InputManager::GetInstance();

        // アイテムが空の場合は何もしない
        if (items.empty()) {
            return;
        }

        // selectedIndexの範囲チェック
        if (selectedIndex < 0 || selectedIndex >= (int)items.size()) {
            selectedIndex = 0;
        }

        if (mode == ItemMode::List) {
            // 上下キーで選ぶ（クールダウン付き）
            if (inputCooldown <= 0.0f) {
                if (input->GetKeyState(KEY_INPUT_UP) == eInputState::Pressed ||
                    input->GetButtonState(XINPUT_BUTTON_DPAD_UP) == eInputState::Pressed) {
                    selectedIndex--;
                    if (selectedIndex < 0) selectedIndex = (int)items.size() - 1;
                    inputCooldown = 0.15f;  // 150ミリ秒のクールダウン
                }
                if (input->GetKeyState(KEY_INPUT_DOWN) == eInputState::Pressed ||
                    input->GetButtonState(XINPUT_BUTTON_DPAD_DOWN) == eInputState::Pressed) {
                    selectedIndex++;
                    if (selectedIndex >= (int)items.size()) selectedIndex = 0;
                    inputCooldown = 0.15f;  // 150ミリ秒のクールダウン
                }
            }

            // Zキーで詳細モードへ
            if (input->GetKeyState(KEY_INPUT_Z) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_A) == eInputState::Pressed) {
                mode = ItemMode::Detail;
            }
        }
        else if (mode == ItemMode::Detail) {
            // Xキーでリストに戻る
            if (input->GetKeyState(KEY_INPUT_X) == eInputState::Pressed ||
                input->GetButtonState(XINPUT_BUTTON_B) == eInputState::Pressed) {
                mode = ItemMode::List;
            }
        }
    }
}

void ItemManager::Draw(float cameraOffsetX) const {
    // フィールド上のアイテム描画
    for (auto& item : items) {
        item->Draw(cameraOffsetX);
    }

    // UI描画
    if (isListOpen) {
        // アイテムが空の場合の処理
        if (items.empty()) {
            int x = 850;  // 右側に移動
            int y = 50;
            DrawBox(x - 20, y - 40, x + 300, y + 60,
                GetColor(0, 0, 0), TRUE);
            DrawBox(x - 20, y - 40, x + 300, y + 60,
                GetColor(255, 255, 255), FALSE);
            DrawFormatString(x, y, GetColor(255, 255, 255), "アイテムがありません");
            return;
        }

        // selectedIndexの範囲チェック
        int safeIndex = selectedIndex;
        if (safeIndex < 0 || safeIndex >= (int)items.size()) {
            safeIndex = 0;
        }

        int x = 850;  // 右側に移動（左側のUIと重ならないように）
        int y = 50;
        int boxWidth = 400;
        int boxHeight = 0;

        if (mode == ItemMode::List) {
            boxHeight = 20 + (int)items.size() * 20;
        }
        else if (mode == ItemMode::Detail) {
            // 詳細モードの高さ計算
            const Item* item = items[safeIndex];
            int descLength = (int)item->GetDescription().length();
            int descLines = (descLength / 20) + 1;
            boxHeight = 150 + descLines * 20;
        }

        // 背景ボックス
        DrawBox(x - 20, y - 40, x + boxWidth, y + boxHeight,
            GetColor(0, 0, 0), TRUE);
        DrawBox(x - 20, y - 40, x + boxWidth, y + boxHeight,
            GetColor(255, 255, 255), FALSE);

        if (mode == ItemMode::List) {
            DrawFormatString(x, y - 30, GetColor(255, 255, 255), "ItemList");
            for (int i = 0; i < (int)items.size(); i++) {
                int drawY = y + i * 20;

                // 選択中のハイライト
                if (i == safeIndex) {
                    DrawBox(x - 10, drawY - 2, x + boxWidth - 10, drawY + 18,
                        GetColor(50, 50, 150), TRUE);
                    DrawBox(x - 10, drawY - 2, x + boxWidth - 10, drawY + 18,
                        GetColor(100, 150, 255), FALSE);
                    DrawFormatString(x - 5, drawY, GetColor(255, 255, 0), ">");
                }

                // アイテム名の描画
                unsigned int color = (i == safeIndex) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

                // 取得済みかどうかで色を変える
                if (items[i]->GetIsCollected()) {
                    DrawFormatString(x + 15, drawY, color, "%s", items[i]->GetName().c_str());
                }
                else {
                    DrawFormatString(x + 15, drawY, GetColor(100, 100, 100), "???");
                }
            }
        }
        else if (mode == ItemMode::Detail) {
            const Item* item = items[safeIndex];

            // 未取得なら詳細は見せない
            if (!item->GetIsCollected()) {
                DrawFormatString(x, y, GetColor(255, 255, 255), "Name: ???");
                DrawFormatString(x, y + 40, GetColor(255, 255, 255), "まだ入手していません");
            }
            else {
                DrawFormatString(x, y, GetColor(255, 255, 255), "Name: %s", item->GetName().c_str());

                std::string desc = item->GetDescription();
                // 簡易的なワードラップ処理
                int lineWidth = (boxWidth - 40) / 9;
                int currentY = y + 40;
                size_t pos = 0;

                while (pos < desc.length()) {
                    size_t len = desc.length() - pos;
                    size_t count = (len > (size_t)lineWidth) ? (size_t)lineWidth : len;

                    std::string line = desc.substr(pos, count);
                    DrawFormatString(x, currentY, GetColor(255, 255, 255), "%s", line.c_str());
                    currentY += 20;
                    pos += count;
                }
            }
            DrawFormatString(x, y + boxHeight - 30, GetColor(200, 200, 200), "Press X to back");
        }
    }
}

void ItemManager::ToggleList() {
    isListOpen = !isListOpen;
    if (isListOpen) {
        mode = ItemMode::List;
    }
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
