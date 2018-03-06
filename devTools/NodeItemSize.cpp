#include "NodeItemSize.h"

NodeItemSize * NodeItemSize::create (LayerUserCtrlEditor * pUserCtrlEditor) {
    NodeItemSize *ret = new (std::nothrow) NodeItemSize();

    if (ret && ret->init (pUserCtrlEditor)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

bool NodeItemSize::init (LayerUserCtrlEditor * pUserCtrlEditor) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

	m_sz = m_pUserCtrlEditor->getModel()->getModelSize();
    
    addLabel ("size:", 0);

    addEditBox ("width:   ", 2, StringUtils::format ("%.2f", m_sz.width), [&] (std::string s) {
        m_sz.width = __String (s).floatValue();

        if (m_sz.width > 0.f) {
            m_pUserCtrlEditor->getModel()->setModelSize (m_sz);
            m_pUserCtrlEditor->updateItems();
        }
    });
    addEditBox ("height:  ", 2, StringUtils::format ("%.2f", m_sz.height), [&] (std::string s) {
        m_sz.height = __String (s).floatValue();

        if (m_sz.height > 0.f) {
            m_pUserCtrlEditor->getModel()->setModelSize (m_sz);
            m_pUserCtrlEditor->updateItems();
        }
    });

    updateItem();
    return true;
}

void NodeItemSize::updateItem() {
    NodeItem::updateItem();

    Rect rect = m_pUserCtrlEditor->getModel()->getGlobalRectForDev();

    rect.origin = m_drawNode->getParent()->convertToNodeSpace (rect.origin);

    m_drawNode->drawRect (rect.origin, rect.origin + rect.size, getColorOnModel());
}

std::string NodeItemSize::checkParams() {
    if (m_sz.width <= 0.f || m_sz.height <= 0.f) {
        return "Err: invald parameter [size]. [size] must > 0.f.";
    }

    return "";
}

void NodeItemSize::addDataToJsonDoc (rapidjson::Document & doc) {
    rapidjson::Value obj (rapidjson::kObjectType);
    obj.AddMember ("width", m_sz.width, doc.GetAllocator());
    obj.AddMember ("height", m_sz.height, doc.GetAllocator());

    doc.AddMember ("size", obj, doc.GetAllocator());
}
