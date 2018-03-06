#ifndef __NODEITEMSCALE_H__
#define __NODEITEMSCALE_H__

#include "NodeItem.h"

class NodeItemScale : public NodeItem {
public:
    static NodeItemScale* create (LayerUserCtrlEditor* pUserCtrlEditor);

    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor);

    virtual std::string checkParams();

    virtual void addDataToJsonDoc (rapidjson::Document& doc);
private:
    float m_scale;
};
#endif // !__NODEITEMSCALE_H__
