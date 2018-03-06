#ifndef __NODEITEMPOSITION_H__
#define __NODEITEMPOSITION_H__

#include "NodeItem.h"

class NodeItemPosition : public NodeItem {
public:
    static NodeItemPosition* create (LayerUserCtrlEditor* pUserCtrlEditor);

    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor);
	
    virtual std::string checkParams();

    virtual void addDataToJsonDoc (rapidjson::Document& doc);
private:
    Vec2 m_position;
};
#endif // !__NODEITEMSCALE_H__
