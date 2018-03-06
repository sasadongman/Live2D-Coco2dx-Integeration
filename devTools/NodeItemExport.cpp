#include "NodeItemExport.h"

NodeItemExport * NodeItemExport::create (
    LayerUserCtrlEditor * pUserCtrlEditor,
    const std::string floder,
    std::string & fileNameToExport) {
    NodeItemExport *ret = new (std::nothrow) NodeItemExport (fileNameToExport);

    if (ret && ret->init (pUserCtrlEditor, floder)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

NodeItemExport::NodeItemExport (std::string & fileNameToExport) :
    NodeItem(), m_fileNameToExport (fileNameToExport) {
}

bool NodeItemExport::init (LayerUserCtrlEditor * pUserCtrlEditor, const std::string floder) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

    m_floder = floder;

    addEditBox ("export file name:  ", 0, m_fileNameToExport, [&] (std::string s) {
        m_fileNameToExport = s;
    });

    updateItem();
    return true;
}

std::string NodeItemExport::checkParams() {
    if (m_fileNameToExport.empty()) {
        return "Err: invalid [export file name]";
    }

    std::string file = m_floder + "/" + m_fileNameToExport;

    if (FileUtils::getInstance()->isFileExist (file)) {
        return "Err: " + file + " is already exist.";
    }

    return "";
}
