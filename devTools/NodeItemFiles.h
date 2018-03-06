#ifndef __NODEITEMFILES_H__
#define __NODEITEMFILES_H__

#include <vector>
#include "NodeItem.h"

class NodeItemFiles : public NodeItem {
public:
    static NodeItemFiles* create (
        LayerUserCtrlEditor* pUserCtrlEditor,
        const std::string& modelPath,
        const std::string& jsonPath,
        const std::vector<std::string>& textureNames);

    virtual bool init (
        LayerUserCtrlEditor* pUserCtrlEditor,
        const std::string& modelName,
        const std::string& jsonName,
        const std::vector<std::string>& textureNames);

    virtual void addDataToJsonDoc (rapidjson::Document& doc);
private:
    std::string m_modelPath;
    std::string m_jsonPath;
    std::vector<std::string> m_textureNames;
};
#endif // !__NODEITEMFILES_H__
