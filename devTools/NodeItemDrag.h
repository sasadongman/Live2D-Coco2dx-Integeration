#ifndef __NODEITEMDRAG_H__
#define __NODEITEMDRAG_H__

#include <vector>
#include "NodeItem.h"

USING_NS_CC;
class NodeItemDrag : public NodeItem {
public:
    static NodeItemDrag* create (LayerUserCtrlEditor* pUserCtrlEditor, const int ainimationId);

    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor, const int ainimationId);

    virtual void updateItem();

    virtual std::string checkParams();

    virtual void addDataToJsonDoc (rapidjson::Document& doc);

	virtual NodeItem* clone();
private:
	l2dDragAnimation* getl2dDragAnimation();
	void updatel2dDragAnimation();
	void doUI();
private:
    int  m_bEnabled;
    Rect m_rect;
    std::string m_paramName;
    float m_dragAngle;
    float m_dragLength;
	int m_ainimationId;
};
#endif // !__NODEITEMFILES_H__
