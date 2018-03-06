#ifndef __NODEITEMSIZE_H__
#define __NODEITEMSIZE_H__

#include "NodeItem.h"

class NodeItemSize : public NodeItem {
public:
    static NodeItemSize* create (LayerUserCtrlEditor* pUserCtrlEditor);

    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor);

    virtual void updateItem();

    virtual std::string checkParams();

    virtual void addDataToJsonDoc (rapidjson::Document& doc);
private:
    Size m_sz;
};
#endif // !__NODEITEMSCALE_H__
