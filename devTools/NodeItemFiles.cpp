#include "NodeItemFiles.h"

NodeItemFiles * NodeItemFiles::create (
    LayerUserCtrlEditor* pUserCtrlEditor,
    const std::string& modelPath,
    const std::string& jsonPath,
    const std::vector<std::string>& textureNames) {
    NodeItemFiles *ret = new (std::nothrow) NodeItemFiles();

    if (ret && ret->init (pUserCtrlEditor, modelPath, jsonPath, textureNames)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

bool NodeItemFiles::init (
    LayerUserCtrlEditor* pUserCtrlEditor,
    const std::string& modelPath,
    const std::string& jsonPath,
    const std::vector<std::string>& textureNames) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

    m_modelPath = modelPath;
    m_jsonPath = jsonPath;

    for (size_t i = 0; i < textureNames.size(); i++) {
        m_textureNames.push_back (textureNames[i]);
    }

    addLabel ("model name:  " + m_modelPath, 0);
    addLabel ("json name:  " + m_jsonPath, 0);
    addLabel ("texture list:", 0);

    for (size_t i = 0; i < m_textureNames.size(); i++) {
        addLabel (m_textureNames[i], 2);
    }

    updateItem();
    return true;
}

void NodeItemFiles::addDataToJsonDoc (rapidjson::Document & doc) {
    doc.AddMember ("modelName",
                   rapidjson::Value (m_modelPath.c_str(), doc.GetAllocator()),
                   doc.GetAllocator());

    doc.AddMember ("jsonName",
                   rapidjson::Value (m_jsonPath.c_str(), doc.GetAllocator()),
                   doc.GetAllocator());

    rapidjson::Value array (rapidjson::kArrayType);

    for (size_t i = 0; i < m_textureNames.size(); i++) {
        array.PushBack (
            rapidjson::Value (m_textureNames[i].c_str(), doc.GetAllocator()),
            doc.GetAllocator());
    }

    doc.AddMember ("textureList", array, doc.GetAllocator());
}
