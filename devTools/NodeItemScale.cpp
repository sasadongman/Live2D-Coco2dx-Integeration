#include "NodeItemScale.h"

NodeItemScale * NodeItemScale::create (LayerUserCtrlEditor * pUserCtrlEditor) {
    NodeItemScale *ret = new (std::nothrow) NodeItemScale();

    if (ret && ret->init (pUserCtrlEditor)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

bool NodeItemScale::init (LayerUserCtrlEditor * pUserCtrlEditor) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

	m_scale = m_pUserCtrlEditor->getModel()->getScale();
    addEditBox ("scale:  ", 0, StringUtils::format ("%.2f", m_scale), [&] (std::string s) {
        m_scale = __String (s).floatValue();

        if (m_scale > 0.f) {
            m_pUserCtrlEditor->getModel()->setScale (m_scale);
            m_pUserCtrlEditor->updateItems();
        }
    });

    updateItem();
    return true;
}

std::string NodeItemScale::checkParams() {
    if (m_scale <= 0.f) {
        return "Err: invald parameter [scale]. [scale] must > 0.f.";
    }

    return "";
}

void NodeItemScale::addDataToJsonDoc (rapidjson::Document & doc) {
    doc.AddMember ("scale", m_scale, doc.GetAllocator());
}
