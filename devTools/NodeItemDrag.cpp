#include "NodeItemDrag.h"

NodeItemDrag * NodeItemDrag::create (LayerUserCtrlEditor * pUserCtrlEditor, const int ainimationId) {
    NodeItemDrag *ret = new (std::nothrow) NodeItemDrag();

    if (ret && ret->init (pUserCtrlEditor, ainimationId)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

bool NodeItemDrag::init (LayerUserCtrlEditor * pUserCtrlEditor, const int ainimationId) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

	m_ainimationId = ainimationId;

	l2dDragAnimation* ani = getl2dDragAnimation();
	if (ani)
	{
		m_bEnabled = ani->isEnabled() ? 1 : 0;
		m_rect = ani->getRect();
		m_paramName = ani->getParamName();
		m_dragAngle = ani->getDragAngle();
		m_dragLength = ani->getDragMaxLength();
	}
	else
	{
		m_bEnabled = 1;
		m_rect = Rect(0, 0, 0, 0);
		m_paramName = "";
		m_dragAngle = 0.f;
		m_dragLength = 0.f;
		updatel2dDragAnimation();
	}

    addLabel ("drag config:", 0);

    addEditBox ("isEnable: ", 2, StringUtils::format ("%d", m_bEnabled), [&] (std::string & s) {
        m_bEnabled = __String (s).intValue();
		updatel2dDragAnimation();
    });

    addEditBox ("rect.x: ", 2, StringUtils::format ("%.2f", m_rect.origin.x), [&] (std::string & s) {
        m_rect.origin.x = __String (s).floatValue();
		updatel2dDragAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("rect.y: ", 2, StringUtils::format ("%.2f", m_rect.origin.y), [&] (std::string & s) {
        m_rect.origin.y = __String (s).floatValue();
		updatel2dDragAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("rect.w: ", 2, StringUtils::format ("%.2f", m_rect.size.width), [&] (std::string & s) {
        m_rect.size.width = __String (s).floatValue();
		updatel2dDragAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("rect.h: ", 2, StringUtils::format ("%.2f", m_rect.size.height), [&] (std::string & s) {
        m_rect.size.height = __String (s).floatValue();
		updatel2dDragAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("parameter: ", 2, m_paramName, [&] (std::string & s) {
        m_paramName = s;
    });
    addEditBox ("drag angle: ", 2, StringUtils::format ("%.2f", m_dragAngle), [&] (std::string & s) {
        m_dragAngle = __String (s).floatValue();
		updatel2dDragAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("drag max length: ", 2, StringUtils::format ("%.2f", m_dragLength), [&] (std::string & s) {
        m_dragLength = __String (s).floatValue();
		updatel2dDragAnimation();
        m_pUserCtrlEditor->updateItems();
    });

    addRemoveBtn();

    updateItem();
    return true;
}

void NodeItemDrag::updateItem() {
    NodeItem::updateItem();

    Size sz = m_pUserCtrlEditor->getModel()->getModelSize();
    Rect globalRect = m_pUserCtrlEditor->getModel()->getGlobalRectForDev();
    Rect rect = m_rect;
    rect.size.width *= globalRect.size.width / sz.width;
    rect.size.height *= globalRect.size.height / sz.height;

    rect.origin += globalRect.origin;

    rect.origin = m_drawNode->getParent()->convertToNodeSpace (rect.origin);

    m_drawNode->drawRect (rect.origin, rect.origin + rect.size, getColorOnModel());

    Vec2 dragVec = Vec2::forAngle (m_dragAngle / 180 * 3.14);
    dragVec *= (m_dragLength);

    dragVec.x *= globalRect.size.width / sz.width;
    dragVec.y *= globalRect.size.height / sz.height;

    Vec2 rectCenter = rect.origin + rect.size / 2;
    m_drawNode->drawLine (rectCenter, rectCenter + dragVec, getColorOnModel());
}

std::string NodeItemDrag::checkParams() {
    if (m_rect.size.width <= 0.f || m_rect.size.height <= 0.f) {
        return "Err: invalid rectangle size [drag config]";
    }

    if (m_paramName.empty()) {
        return "Err: invalid paramName [drag config]";
    }

    if (m_dragLength <= 0.f) {
        return "Err: invalid drag max length [drag config].";
    }

    return "";
}

void NodeItemDrag::addDataToJsonDoc (rapidjson::Document & doc) {
    rapidjson::Value obj (rapidjson::kObjectType);
    obj.AddMember ("bEnable", m_bEnabled, doc.GetAllocator());
    obj.AddMember ("rectOriginX", m_rect.origin.x, doc.GetAllocator());
    obj.AddMember ("rectOriginY", m_rect.origin.y, doc.GetAllocator());
    obj.AddMember ("rectWidth", m_rect.size.width, doc.GetAllocator());
    obj.AddMember ("rectHeight", m_rect.size.height, doc.GetAllocator());
    obj.AddMember ("paramName", rapidjson::Value (m_paramName.c_str(), doc.GetAllocator()), doc.GetAllocator());
    obj.AddMember ("dragAngle", m_dragAngle, doc.GetAllocator());
    obj.AddMember ("dragMaxLength", m_dragLength, doc.GetAllocator());

    if (!doc.HasMember ("dragConfig")) {
        rapidjson::Value array (rapidjson::kArrayType);
        doc.AddMember ("dragConfig", array, doc.GetAllocator());
    }

    doc["dragConfig"].PushBack (obj, doc.GetAllocator());
}

NodeItem * NodeItemDrag::clone()
{
	NodeItemDrag* item = NodeItemDrag::create(m_pUserCtrlEditor, -1);
	item->m_bEnabled = m_bEnabled;
	item->m_rect = m_rect;
	item->m_paramName = m_paramName;
	item->m_dragAngle = m_dragAngle;
	item->m_dragLength = m_dragLength;
	item->doUI();
	return item;
}

l2dDragAnimation * NodeItemDrag::getl2dDragAnimation()
{
	Mapl2dGestureAnimation& mapAnimaiton = m_pUserCtrlEditor->getModel()->getAnimationsMap();
	auto findRes = mapAnimaiton.find(m_ainimationId);
	if (findRes != mapAnimaiton.end())
	{
		return dynamic_cast<l2dDragAnimation*>(findRes->second);
	}
	else
	{
		return NULL;
	}
	return nullptr;
}

void NodeItemDrag::updatel2dDragAnimation()
{
	l2dDragAnimation* aniOld = getl2dDragAnimation();
	if (aniOld)
	{
		m_pUserCtrlEditor->getModel()->removeAnimation(m_ainimationId);
		m_ainimationId = -1;
	}

	m_ainimationId = m_pUserCtrlEditor->getModel()->addDragAnimation(
		m_rect,
		m_dragLength,
		m_dragAngle,
		m_paramName);
	l2dDragAnimation* aniNew = getl2dDragAnimation();
	aniNew->setEnabled(m_bEnabled);
}

void NodeItemDrag::doUI()
{
	getChildByName<ui::TextField*>("isEnable: ")->setString(StringUtils::format("%d", m_bEnabled));

	getChildByName<ui::TextField*>("rect.x: ")->setString(StringUtils::format("%.2f", m_rect.origin.x));

	getChildByName<ui::TextField*>("rect.y: ")->setString(StringUtils::format("%.2f", m_rect.origin.y));

	getChildByName<ui::TextField*>("rect.w: ")->setString(StringUtils::format("%.2f", m_rect.size.width));

	getChildByName<ui::TextField*>("rect.h: ")->setString(StringUtils::format("%.2f", m_rect.size.height));

	getChildByName<ui::TextField*>("parameter: ")->setString(m_paramName);

	getChildByName<ui::TextField*>("drag angle: ")->setString(StringUtils::format("%.2f", m_dragAngle));

	getChildByName<ui::TextField*>("drag max length: ")->setString(StringUtils::format("%.2f", m_dragLength));
}
