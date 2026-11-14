#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

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

        auto fillSpr = ButtonSprite::create("Fill fake", 80, true, "bigFont.fnt", "GJ_button_04.png", 30.f, 0.6f);
        auto fillBtn = CCMenuItemSpriteExtra::create(fillSpr, this, menu_selector(FakeLoginPopup::onFill));
        m_buttonMenu->addChildAtPosition(fillBtn, Anchor::Bottom, {-60.f, 30.f});

        auto loginSpr = ButtonSprite::create("Login", 80, true, "goldFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
        auto loginBtn = CCMenuItemSpriteExtra::create(loginSpr, this, menu_selector(FakeLoginPopup::onLogin));
        m_buttonMenu->addChildAtPosition(loginBtn, Anchor::Bottom, {60.f, 30.f});

        return true;
    }

    void onFill(CCObject*) {
        m_userInput->setString("FakeUser123");
        m_passInput->setString("hunter2");
    }

    void onLogin(CCObject*) {
        std::string name = m_userInput->getString();
        if (name.empty()) name = "Nobody";
        FLAlertLayer::create(
            "Fake Login",
            fmt::format("Welcome, <cy>{}</c>!\nThis login is fake. Nothing was saved or sent.", name),
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

class $modify(FakeLoginMenu, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto spr = ButtonSprite::create("Fake Login", 90, true, "bigFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FakeLoginMenu::onFakeLogin));

        auto menu = CCMenu::create();
        menu->addChild(btn);
        menu->setID("fake-login-menu"_spr);

        auto winSize = CCDirector::get()->getWinSize();
        menu->setPosition({70.f, winSize.height - 25.f});
        this->addChild(menu);

        return true;
    }

    void onFakeLogin(CCObject*) {
        FakeLoginPopup::create()->show();
    }
};
