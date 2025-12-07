#pragma once
#include "NPC.h"

enum class SuspectType {
    Fujisaki,      // 藤崎美咲（元従業員）
    Sasaki,        // 佐々木健太（隣の店主）
    Kimura,        // 木村達也（弟）★犯人
    Yamada         // 山田一郎（常連客）
};

class Suspect : public NPC {
private:
    SuspectType suspectType;
    bool isGuilty;              // 犯人かどうか

public:
    Suspect(float posX, float posY, SuspectType type);

    void Initialize() override;
    void SetupTestimonies();    // 証言を設定

    SuspectType GetSuspectType() const { return suspectType; }
    bool IsGuilty() const { return isGuilty; }
};