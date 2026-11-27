#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;

// A joke login popup. It does NOT save, store, or send anything anywhere.
class FakeLoginPopup : public Popup<> {
protected:
    TextInput* m_userInput = nullptr;
    TextInput* m_passInput = nullptr;

    bool setup() override {
        this->setTitle("Fake Login");

        m_userInput = TextInput::create(220.f, "Fake username");
        m_userInput->setMaxCharCount(20);
        m_mainLayer->addChildAtPosition(m_userInput, Anchor::Center, {0.f, 35.f});

        m_passInput = TextInput::create(220.f, "Fake password");
        m_passInput->setMaxCharCount(20);
        m_passInput->setPasswordMode(true);
        m_mainLayer->addChildAtPosition(m_passInput, Anchor::Center, {0.f, -5.f});

        auto fillSpr = ButtonSprite::create("Fill RopTop", 85, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.55f);
        auto fillBtn = CCMenuItemSpriteExtra::create(fillSpr, this, menu_selector(FakeLoginPopup::onFill));
        m_buttonMenu->addChildAtPosition(fillBtn, Anchor::Bottom, {-60.f, 30.f});

        auto loginSpr = ButtonSprite::create("Login", 80, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
        auto loginBtn = CCMenuItemSpriteExtra::create(loginSpr, this, menu_selector(FakeLoginPopup::onLogin));
        m_buttonMenu->addChildAtPosition(loginBtn, Anchor::Bottom, {60.f, 30.f});

        return true;
    }

    void onFill(CCObject*) {
        m_userInput->setString("RopTop");
        m_passInput->setString("2point2soon");
    }

    void onLogin(CCObject*) {
        std::string name = m_userInput->getString();
        if (name.empty()) name = "Nobody";
        Mod::get()->setSavedValue("fake-logged-in-name", name);

        FLAlertLayer::create(
            "Fake Login",
            fmt::format(
                "Welcome, <cy>{}</c>!\n"
                "Fake Moderator status granted. Open your profile to see your badge!",
                name
            ),
            "OK"
        )->show();
        this->onClose(nullptr);
    }

public:
    static FakeLoginPopup* create() {
        auto ret = new FakeLoginPopup();
        if (ret->initAnchored(300.f, 190.f)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class $modify(FakeModProfilePage, ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);

        // Only apply to your own profile page
        if (!m_ownProfile) return;

        auto mod = Mod::get();
        auto badgeType = mod->getSettingValue<std::string>("fake-mod-badge");
        if (badgeType == "None") return;

        // Change displayed username if the user used Fake Login
        auto fakeName = mod->getSavedValue<std::string>("fake-logged-in-name", "");
        if (!fakeName.empty() && m_usernameLabel) {
            m_usernameLabel->setString(fakeName.c_str());
            m_usernameLabel->limitLabelWidth(160.f, 0.8f, 0.3f);
        }

        // Remove any existing fake badge if page reloads
        if (auto oldBadge = m_mainLayer->getChildByID("fake-mod-badge"_spr)) {
            oldBadge->removeFromParent();
        }

        // Pick the requested Geometry Dash Moderator sprite
        const char* spriteName = "modBadge_02_001.png";
        if (badgeType == "Mod") {
            spriteName = "modBadge_01_001.png";
        } else if (badgeType == "Leaderboard Mod") {
            spriteName = "modBadge_03_001.png";
        }

        auto badge = CCSprite::createWithSpriteFrameName(spriteName);
        if (badge && m_usernameLabel) {
            badge->setID("fake-mod-badge"_spr);
            float labelLeft = m_usernameLabel->getPositionX()
                - (m_usernameLabel->getContentSize().width * m_usernameLabel->getScaleX() * 0.5f);
            badge->setPosition({ labelLeft - 14.f, m_usernameLabel->getPositionY() });
            m_mainLayer->addChild(badge, 10);
        }
    }
};

class $modify(FakeLoginMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto mod = Mod::get();
        auto winSize = CCDirector::get()->getWinSize();

        // 1. Fake Login button in the top-left corner
        auto spr = ButtonSprite::create("Fake Login", 90, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FakeLoginMenu::onFakeLogin));

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setID("fake-login-menu"_spr);
        menu->setPosition({70.f, winSize.height - 25.f});
        this->addChild(menu);

        // 2. Toggleable beta watermark in the bottom-right corner
        if (mod->getSettingValue<bool>("show-beta-watermark")) {
            auto betaLabel = CCLabelBMFont::create(
                fmt::format("{} {}", mod->getName(), mod->getVersion().toVString()).c_str(),
                "chatFont.fnt"
            );
            betaLabel->setScale(0.45f);
            betaLabel->setOpacity(110);
            betaLabel->setAnchorPoint({ 1.f, 0.f });
            betaLabel->setPosition({ winSize.width - 6.f, 4.f });
            betaLabel->setID("beta-watermark"_spr);
            this->addChild(betaLabel, 100);
        }

        // 3. One-time Beta Tester Notice when launching this version for the first time
        auto savedVer = mod->getSavedValue<std::string>("last-seen-beta-version", "");
        auto currentVer = mod->getVersion().toVString();
        if (savedVer != currentVer) {
            mod->setSavedValue("last-seen-beta-version", currentVer);
            Loader::get()->queueInMainThread([currentVer]() {
                FLAlertLayer::create(
                    "Beta Build Notice",
                    fmt::format(
                        "You are running <cy>{}</c>.\n"
                        "Official RopTop beta build (100% real, trust). Please report any bugs on GitHub.",
                        currentVer
                    ),
                    "Got It"
                )->show();
            });
        }

        return true;
    }

    void onFakeLogin(CCObject*) {
        FakeLoginPopup::create()->show();
    }
};
