#ifndef __NODEITEMTIMES_H__
#define __NODEITEMTIMES_H__

#include "NodeItem.h"

class NodeItemTimes : public NodeItem {
public:
    static NodeItemTimes* create (LayerUserCtrlEditor* pUserCtrlEditor);

    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor);

    virtual std::string checkParams();

    virtual void addDataToJsonDoc (rapidjson::Document& doc);

private:
	std::string getStringTimes();

private:
    std::vector<float> m_times;
};
#endif // !__NODEITEMSCALE_H__
