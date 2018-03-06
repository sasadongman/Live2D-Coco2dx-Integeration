#include "NodeItemPosition.h"

NodeItemPosition * NodeItemPosition::create (LayerUserCtrlEditor * pUserCtrlEditor) {
	NodeItemPosition *ret = new (std::nothrow) NodeItemPosition();

    if (ret && ret->init (pUserCtrlEditor)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

bool NodeItemPosition::init (LayerUserCtrlEditor * pUserCtrlEditor) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

    m_position = m_pUserCtrlEditor->getModel()->getPosition();

    addLabel ("position:", 0);

    addEditBox ("x:  ", 2, StringUtils::format ("%.2f", m_position.x), [&] (std::string s) {
        m_position.x = __String (s).floatValue();
        m_pUserCtrlEditor->getModel()->setPosition(m_position);
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("y:  ", 2, StringUtils::format ("%.2f", m_position.y), [&] (std::string s) {
        m_position.y = __String (s).floatValue();
        m_pUserCtrlEditor->getModel()->setPosition(m_position);
        m_pUserCtrlEditor->updateItems();
    });

    updateItem();
    return true;
}

std::string NodeItemPosition::checkParams() {
    return "";
}

void NodeItemPosition::addDataToJsonDoc (rapidjson::Document & doc) {
    rapidjson::Value obj (rapidjson::kObjectType);
    obj.AddMember ("x", m_position.x, doc.GetAllocator());
    obj.AddMember ("y", m_position.y, doc.GetAllocator());

    doc.AddMember ("position", obj, doc.GetAllocator());
}
