#ifndef __LISTVIEW_MANAAGER_H__
#define __LISTVIEW_MANAAGER_H__

#include "cocos2d.h"
#include "live2d\devTools\LayerUserCtrlEditor.h"
USING_NS_CC;

class NodeItem : public ui::Layout {
public:
    virtual bool init (LayerUserCtrlEditor* pUserCtrlEditor);
    virtual void updateItem();
    virtual void setSelectState (const bool bSelect);
    virtual bool isSelect();
    virtual Color4F getColor();
	virtual Color4F getColorOnModel();
    virtual int  getDisplayZorder();
    virtual float getDefaultWidth();
    virtual std::string checkParams();
    virtual void addDataToJsonDoc (rapidjson::Document& doc);
	virtual NodeItem* clone();

protected:
    ui::Button* addRemoveBtn();
    Label* addLabel (const std::string& title, const int spaceCount);
    ui::TextField* addEditBox (const std::string& title, const int spaceCount, const std::string& defaultValue, std::function<void (std::string) > onInput);
    void offsetChildPositions (const Vec2& offset);
    void refreshSize (const Vec2& offset);
protected:
    LayerUserCtrlEditor* m_pUserCtrlEditor;
    DrawNode*           m_drawNode;
    bool                m_bSelect;
    LayerColor*         m_mask;
};
#endif // !__NODEITEM_MANAAGER_H__
