#include "LayerUserCtrlEditor.h"
#include "NodeItemFiles.h"
#include "NodeItemPosition.h"
#include "NodeItemScale.h"
#include "NodeItemSize.h"
#include "NodeItemTimes.h"
#include "NodeItemClick.h"
#include "NodeItemDrag.h"
#include "NodeItemExport.h"
#include "live2d\Live2dUserCtrlFactory.h"

const int BtnCount = 5;
const float hightButton = 25;
LayerUserCtrlEditor * LayerUserCtrlEditor::import(
	const std::string & folder,
	const std::string & configFile, 
	const std::string & modelName, 
	const std::string & jsonName, 
	const std::vector<std::string>& textureNames)
{
	LayerUserCtrlEditor *ret = new (std::nothrow) LayerUserCtrlEditor();

	if (ret && ret->initOnImport(folder, configFile, modelName, jsonName, textureNames)) {
		ret->autorelease();
		return ret;
	}
	else {
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

LayerUserCtrlEditor * LayerUserCtrlEditor::create(
	const std::string& folder,
	const std::string& modelName,
	const std::string& jsonName,
	const std::vector<std::string>& textureNames) {
	LayerUserCtrlEditor *ret = new (std::nothrow) LayerUserCtrlEditor();

	if (ret && ret->initOnCreate(folder, modelName, jsonName, textureNames)) {
		ret->autorelease();
		return ret;
	}
	else {
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool LayerUserCtrlEditor::initOnImport(
	const std::string& folder,
	const std::string & configFile,
	const std::string& modelName,
	const std::string& jsonName,
	const std::vector<std::string>& textureNames) {
	if (!Layer::init()) {
		return false;
	}

	m_fileNameToExport = configFile;

	m_model = Live2dUserCtrlFactory::create(folder + "/" + configFile);
	addChild(m_model);

	createAndAddChildOperatorBtns();
	createAndAddChildListView(folder, modelName, jsonName, textureNames);
	createAndAddChildLogLabel();

	schedule(schedule_selector(LayerUserCtrlEditor::updateItems), 1.0f);
	return true;
}

bool LayerUserCtrlEditor::initOnCreate(
    const std::string& folder,
    const std::string& modelName,
    const std::string& jsonName,
    const std::vector<std::string>& textureNames) {
    if (!Layer::init()) {
        return false;
    }
	
    std::vector<std::string> texturePath;
    for (size_t i = 0; i < textureNames.size(); i++) {
        texturePath.push_back (folder + "/" + textureNames[i]);
    }

    m_model = Live2dUserCtrl::create (
                  std::string (folder) + "/" + modelName,
                  std::string (folder) + "/" + jsonName,
                  texturePath);
    addChild (m_model);

	createAndAddChildOperatorBtns();
	createAndAddChildListView(folder, modelName, jsonName, textureNames);
	createAndAddChildLogLabel();
	
	schedule(schedule_selector(LayerUserCtrlEditor::updateItems), 1.0f);
    return true;
}

Live2dUserCtrl* LayerUserCtrlEditor::getModel() {
    return m_model;
}

void LayerUserCtrlEditor::updateItems(float) {
    auto items = m_listView->getItems();

    for (size_t i = 0; i < items.size(); i++) {
        ( (NodeItem*) items.at (i))->updateItem();
    }
}

void LayerUserCtrlEditor::createAndAddChildOperatorBtns()
{
	int index = 0;

	addChild(createBtn(index++, "Add CLICK animation", [&]() {
		Size sz = m_model->getModelSize();

		if (sz.height == 0.f || sz.width == 0.f) {
			log("Err: you should input [size] first.");
		}
		else if (m_model->getAnimationTimes().size() <= 1) {
			log("Err: you should split animation times first.");
		}
		else {
			log("");
			m_listView->pushBackCustomItem(NodeItemClick::create(this, -1));
			updateSelectItems(m_listView->getItems().size() - 1);
		}
	}));

	addChild(createBtn(index++, "Add DRAG animation", [&]() {
		Size sz = m_model->getModelSize();

		if (sz.height == 0.f || sz.width == 0.f) {
			log("Err: you should input [size] first.");
		}
		else {
			log("");
			m_listView->pushBackCustomItem(NodeItemDrag::create(this, -1));
			updateSelectItems(m_listView->getItems().size() - 1);
		}
	}));

	addChild(createBtn(index++, "Clone selected item", [&]() {
		int selectIndex = m_listView->getCurSelectedIndex();
		auto items = m_listView->getItems();

		if (selectIndex < 0 || selectIndex >= items.size())
		{
			log("Err: clone failed. Please select a validate item");
			return;
		}

		NodeItem* item = ((NodeItem*)items.at(selectIndex))->clone();
		if (item)
		{
			m_listView->pushBackCustomItem(item);
			updateSelectItems(items.size());
			log("");
		}
		else
		{
			log("Err: clone failed. The selected item is not clone allowed!");
		}
	}));

	addChild(createBtn(index++, "Show/Hide listview", [&]() {
		m_listView->setVisible(!m_listView->isVisible());
	}));

	addChild(createBtn(index++, "Export", [&]() {
		auto items = m_listView->getItems();

		for (size_t i = 0; i < items.size(); i++) {
			std::string err = ((NodeItem*)items.at(i))->checkParams();

			if (!err.empty()) {
				updateSelectItems(i);
				log(err);
				return;
			}
		}

		exportJsonFile();
		log("export successfully!");
	}));
}

void LayerUserCtrlEditor::createAndAddChildListView(
	const std::string& folder,
	const std::string& modelName,
	const std::string& jsonName,
	const std::vector<std::string>& textureNames)
{
	auto sz = Director::getInstance()->getVisibleSize();

	m_listView = ui::ListView::create();
	m_listView->setColor(Color3B::RED);
	m_listView->setAnchorPoint(Vec2(1, 1));
	m_listView->setPosition(sz - Size(0, hightButton * BtnCount));
	m_listView->setContentSize(Size(NodeItem().getDefaultWidth(), sz.height - hightButton * BtnCount));
	m_listView->setDirection(ui::ScrollView::Direction::VERTICAL);
	m_listView->setTouchEnabled(true);
	m_listView->pushBackCustomItem(NodeItemFiles::create(this, modelName, jsonName, textureNames));
	m_listView->pushBackCustomItem(NodeItemExport::create(this, folder, m_fileNameToExport));
	m_listView->pushBackCustomItem(NodeItemPosition::create(this));
	m_listView->pushBackCustomItem(NodeItemTimes::create(this));
	m_listView->pushBackCustomItem(NodeItemScale::create(this));
	m_listView->pushBackCustomItem(NodeItemSize::create(this));

	auto& mapAni = m_model->getAnimationsMap();
	for (auto iter = mapAni.begin(); iter != mapAni.end(); ++iter)
	{
		if (GestureType::eClick == iter->second->getType())
		{
			m_listView->pushBackCustomItem(NodeItemClick::create(this, iter->second->getId()));
		}
		else
		{
			m_listView->pushBackCustomItem(NodeItemDrag::create(this, iter->second->getId()));
		}
	}

	m_listView->addEventListener([&](Ref * ref, ui::ListView::EventType evt) {
		int selectIndex = m_listView->getCurSelectedIndex();
		updateSelectItems(selectIndex);
	});

	addChild(m_listView);
}

ui::Button * LayerUserCtrlEditor::createBtn(
	const int btnIndex,
	const std::string label, 
	std::function<void()> onClickFunc)
{
	auto sz = Director::getInstance()->getVisibleSize();
	auto button = ui::Button::create();
	button->setContentSize(Size(NodeItem().getDefaultWidth(), hightButton));
	button->setPosition(sz - Size(0, hightButton) * btnIndex);
	button->setAnchorPoint(Vec2(1, 1));
	button->setColor(Color3B::GRAY);
	button->setTitleText(label);
	button->setTitleFontSize(25);
	button->setTitleColor(Color3B::YELLOW);
	button->addClickEventListener([=](Ref*) {
		onClickFunc();
	});
	return button;
}

void LayerUserCtrlEditor::createAndAddChildLogLabel() {
    m_logLabel = Label::create();
    m_logLabel->setTextColor (Color4B::RED);
    m_logLabel->setSystemFontSize (25);
    m_logLabel->setAnchorPoint (Vec2 (0, 0));
    m_logLabel->setPosition (0, 0);
    m_logLabel->setAlignment (TextHAlignment::LEFT);
	addChild(m_logLabel);
}

void LayerUserCtrlEditor::log (const std::string & msg) {
    m_logLabel->setString (msg);
}

void LayerUserCtrlEditor::removeListViewItem (ui::Widget * w) {
    auto items = m_listView->getItems();

    for (size_t i = 0; i < items.size(); i++) {
        if (items.at (i) == w) {
            m_listView->removeItem (i);
            return;
        }
    }
}

void LayerUserCtrlEditor::exportJsonFile() {
    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();

    auto items = m_listView->getItems();

    for (size_t i = 0; i < items.size(); i++) {
        ( (NodeItem*) items.at (i))->addDataToJsonDoc (jsonDoc);
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer (buffer);
    jsonDoc.Accept (writer);

    std::string fullPathModelFile = FileUtils::getInstance()->fullPathForFilename (getModel()->getModelName());
    std::string file = FileUtils::getInstance()->fullPathFromRelativeFile (m_fileNameToExport, fullPathModelFile);
    FILE* myFile = fopen (file.c_str(), "w");

    if (myFile) {
        fputs (buffer.GetString(), myFile);
        fclose (myFile);
    }
}

void LayerUserCtrlEditor::updateSelectItems(const int selectItemIndex)
{
	auto items = m_listView->getItems();
	for (size_t i = 0; i < items.size(); i++) {
		bool bSelect = i == selectItemIndex;
		((NodeItem*)items.at(i))->setSelectState(bSelect);
	}
}
