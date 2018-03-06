#ifndef __LayerUserCtrlEditor_H__
#define __LayerUserCtrlEditor_H__

#include "cocos2d.h"
#include "ui\CocosGUI.h"
#include "live2d\Live2dUserCtrl.h"
#include "json\document.h"
#include "json\stringbuffer.h"
#include "json\writer.h"

USING_NS_CC;

class LayerUserCtrlEditor: public Layer {
public:

	static LayerUserCtrlEditor* import(
		const std::string& folder,
		const std::string& configFile,
		const std::string& modelName,
		const std::string& jsonName,
		const std::vector<std::string>& textureNames);

	static LayerUserCtrlEditor* create(
		const std::string& folder,
		const std::string& modelName,
		const std::string& jsonName,
		const std::vector<std::string>& textureNames);

	bool initOnImport(
		const std::string& folder,
		const std::string& configFile,
		const std::string& modelName,
		const std::string& jsonName,
		const std::vector<std::string>& textureNames);
		
    bool initOnCreate (
        const std::string& folder,
        const std::string& modelName,
        const std::string& jsonName,
        const std::vector<std::string>& textureNames);

    Live2dUserCtrl* getModel();

    void updateItems(float dt = 0.f);

    void log (const std::string& msg);

    void removeListViewItem (ui::Widget* w);

private:
	void createAndAddChildOperatorBtns();
	void createAndAddChildListView(
		const std::string& folder,
		const std::string& modelName,
		const std::string& jsonName,
		const std::vector<std::string>& textureNames);
	void createAndAddChildLogLabel();
	
	ui::Button* createBtn(
		const int btnIndex,
		const std::string label, 
		std::function<void()> onClickFunc);
	

	void exportJsonFile();

	void updateSelectItems(const int selectItemIndex);
private:
    Live2dUserCtrl* m_model;
    ui::ListView*   m_listView;
    Label*          m_logLabel;
    std::string     m_fileNameToExport;
};

#endif // !__LayerUserCtrlEditor_H__
