#include "NodeItem.h"
#include "ui\CocosGUI.h"

const int cFontSize = 26;
const float heightBase = 10.f;
const float widthBase = 400;

bool NodeItem::init (LayerUserCtrlEditor* pUserCtrlEditor) {
    if (!ui::Layout::init()) {
        return false;
    }

    setAnchorPoint (Vec2 (0, 0));

    m_pUserCtrlEditor = pUserCtrlEditor;

    m_mask = LayerColor::create (Color4B (0, 0, 0, 180));
    m_mask->setContentSize (Size (widthBase, heightBase * 2));
    m_mask->setPosition (Vec2 (0, 0));
    addChild (m_mask);

    m_drawNode = DrawNode::create();
    addChild (m_drawNode);

    setContentSize (Size (widthBase, heightBase * 2));


    return true;
}

void NodeItem::updateItem() {
    m_mask->setPosition (0, 0);

    m_drawNode->clear();

    m_drawNode->setPosition (0, 0);

    m_drawNode->setGlobalZOrder (getDisplayZorder());

    m_drawNode->drawRect (Vec2 (0, 0), getContentSize(), getColor());
}

void NodeItem::setSelectState (const bool bSelect) {
    m_bSelect = bSelect;

    updateItem();
}

bool NodeItem::isSelect() {
    return m_bSelect;
}

Color4F NodeItem::getColor() {
    return isSelect() ? Color4F::YELLOW : Color4F::GRAY;
}

Color4F NodeItem::getColorOnModel()
{
	return isSelect() ? Color4F::YELLOW : Color4F(0,0,0,0);
}

int NodeItem::getDisplayZorder() {
    return isSelect() ? 1 : 0;
}

float NodeItem::getDefaultWidth() {
    return widthBase;
}

std::string NodeItem::checkParams() {
    return "";
}

void NodeItem::addDataToJsonDoc (rapidjson::Document & doc) {

}

NodeItem * NodeItem::clone()
{
	return nullptr;
}

ui::Button* NodeItem::addRemoveBtn() {
    auto btn = ui::Button::create();
    btn->setAnchorPoint (Vec2 (1, 1));

    auto sz = getContentSize();
    sz.height -= heightBase;
    btn->setPosition (sz);

    btn->setContentSize (Size (cFontSize * 2, cFontSize * 2));
    btn->setTitleText ("X");
    btn->setTitleFontSize (cFontSize * 2);
    btn->setTitleColor (Color3B::GRAY);
    btn->addClickEventListener ([&] (Ref*) {
        m_pUserCtrlEditor->removeListViewItem (this);
    });
    addChild (btn);
    return btn;
}

Label* NodeItem::addLabel (const std::string & title, const int spaceCount) {
    std::string str = title;

    for (size_t i = 0; i < spaceCount; i++) {
        str = " " + str;
    }

    auto label = Label::create();
	label->setName(title);
    label->setSystemFontSize (cFontSize);
    label->setString (str);
    label->setTextColor (Color4B::GRAY);
    label->setAnchorPoint (Vec2 (0, 0));
    label->setPosition (Vec2 (0, heightBase));

    Size sz = label->getContentSize();
    offsetChildPositions (Vec2 (0, sz.height));
    refreshSize (Vec2 (0, sz.height));

    addChild (label);
    return label;
}

ui::TextField* NodeItem::addEditBox (const std::string & title, const int spaceCount, const std::string& defaultValue, std::function<void (std::string) > onInput) {
    std::string str = title;

    for (size_t i = 0; i < spaceCount; i++) {
        str = " " + str;
    }

    auto label = Label::create();
    label->setSystemFontSize (cFontSize);
    label->setString (str);
    label->setTextColor (Color4B::GRAY);
    label->setAnchorPoint (Vec2 (0, 0));
    label->setPosition (Vec2 (0, heightBase));
    Size sz = label->getContentSize();

    auto tf = ui::TextField::create();
	tf->setName(title);
    tf->setFontSize (cFontSize);
    tf->setString (defaultValue);
    tf->setTextColor (Color4B::WHITE);
    tf->setAnchorPoint (Vec2 (0, 0));
    tf->setPosition (Vec2 (sz.width, heightBase));
    tf->addEventListener ([ = ] (Ref * ref, ui::TextField::EventType type) {
        if (type == ui::TextField::EventType::DETACH_WITH_IME) {
            onInput ( ( (ui::TextField*) ref)->getString());
        }
    });

    offsetChildPositions (Vec2 (0, sz.height));
    refreshSize (Vec2 (0, sz.height));

    addChild (label);
    addChild (tf);

    return tf;
}

void NodeItem::offsetChildPositions (const Vec2 & offset) {
    auto children = getChildren();

    for (size_t i = 0; i < children.size(); i++) {
        Vec2 pos = children.at (i)->getPosition();
        pos += offset;
        children.at (i)->setPosition (pos);
    }
}

void NodeItem::refreshSize (const Vec2 & offset) {
    Size sz = getContentSize();
    sz.height += offset.y;
    setContentSize (sz);

    m_mask->setContentSize (sz);
}

