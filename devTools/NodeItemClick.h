#ifndef __NODEITEMCLICK_H__
#define __NODEITEMCLICK_H__

#include <vector>
#include "NodeItem.h"

USING_NS_CC;
class NodeItemClick : public NodeItem {
public:
    static NodeItemClick* create (LayerUserCtrlEditor* pUserCtrlEditor, const int ainimationId);

    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor, const int ainimationId);

    virtual void updateItem();

    virtual std::string checkParams();

    virtual void addDataToJsonDoc (rapidjson::Document& doc);

	virtual NodeItem* clone();
private:
	l2dClickAnimation* getl2dClickAnimation();
	void updatel2dClickAnimation();
	void doUI();
private:
    int  m_bEnabled;
    Rect m_rect;
    int  m_aniIndex;
	int m_ainimationId;
};
#endif // !__NODEITEMFILES_H__
