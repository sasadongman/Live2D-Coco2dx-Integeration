#include "NodeItemClick.h"

NodeItemClick * NodeItemClick::create (LayerUserCtrlEditor * pUserCtrlEditor, const int ainimationId) {
    NodeItemClick *ret = new (std::nothrow) NodeItemClick();

    if (ret && ret->init (pUserCtrlEditor, ainimationId)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

bool NodeItemClick::init (LayerUserCtrlEditor * pUserCtrlEditor, const int ainimationId) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

	m_ainimationId = ainimationId;
	l2dClickAnimation* ani = getl2dClickAnimation();
	if (ani)
	{
		m_bEnabled = ani->isEnabled() ? 0 : 1;
		m_rect = ani->getRect();
		m_aniIndex = ani->getAnimationIndex();
	}
	else
	{
		m_bEnabled = 1;
		m_rect = Rect(0, 0, 0, 0);
		m_aniIndex = 0;
		updatel2dClickAnimation();
	}

    addLabel ("click config:", 0);

    addEditBox ("isEnable: ", 2, StringUtils::format ("%d", m_bEnabled), [&] (std::string & s) {
        m_bEnabled = __String (s).intValue();
		updatel2dClickAnimation();
    });

    addEditBox ("rect.x: ", 2, StringUtils::format ("%.2f", m_rect.origin.x), [&] (std::string & s) {
        m_rect.origin.x = __String (s).floatValue();
		updatel2dClickAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("rect.y: ", 2, StringUtils::format ("%.2f", m_rect.origin.y), [&] (std::string & s) {
        m_rect.origin.y = __String (s).floatValue();
		updatel2dClickAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("rect.w: ", 2, StringUtils::format ("%.2f", m_rect.size.width), [&] (std::string & s) {
        m_rect.size.width = __String (s).floatValue();
		updatel2dClickAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("rect.h: ", 2, StringUtils::format ("%.2f", m_rect.size.height), [&] (std::string & s) {
        m_rect.size.height = __String (s).floatValue();
		updatel2dClickAnimation();
        m_pUserCtrlEditor->updateItems();
    });
    addEditBox ("animation index: ", 2, StringUtils::format ("%d", m_aniIndex), [&] (std::string & s) {
        m_aniIndex = __String (s).intValue();
		updatel2dClickAnimation();
    });

    addRemoveBtn();

    updateItem();
    return true;
}

void NodeItemClick::updateItem() {
    NodeItem::updateItem();

    Size sz = m_pUserCtrlEditor->getModel()->getModelSize();
    Rect globalRect = m_pUserCtrlEditor->getModel()->getGlobalRectForDev();
    Rect rect = m_rect;
    rect.size.width *= globalRect.size.width / sz.width;
    rect.size.height *= globalRect.size.height / sz.height;

    rect.origin += globalRect.origin;

    rect.origin = m_drawNode->getParent()->convertToNodeSpace (rect.origin);

    m_drawNode->drawRect (rect.origin, rect.origin + rect.size, getColorOnModel());
}

std::string NodeItemClick::checkParams() {
    if (m_rect.size.width <= 0.f || m_rect.size.height <= 0.f) {
        return "Err: invalid rectangle size [click config]";
    }

    if (m_aniIndex < 0) {
        return "Err: invalid animation index [click config]. animation index must >= 0.";
    }

    if (m_aniIndex >= m_pUserCtrlEditor->getModel()->getAnimationTimes().size()) {
        return "Err: invalid animation index [click config]. animation index must < [times].size";
    }

    return "";
}

void NodeItemClick::addDataToJsonDoc (rapidjson::Document & doc) {
    rapidjson::Value obj (rapidjson::kObjectType);
    obj.AddMember ("bEnable", m_bEnabled, doc.GetAllocator());
    obj.AddMember ("rectOriginX", m_rect.origin.x, doc.GetAllocator());
    obj.AddMember ("rectOriginY", m_rect.origin.y, doc.GetAllocator());
    obj.AddMember ("rectWidth", m_rect.size.width, doc.GetAllocator());
    obj.AddMember ("rectHeight", m_rect.size.height, doc.GetAllocator());
    obj.AddMember ("animationIndex", m_aniIndex, doc.GetAllocator());

    if (!doc.HasMember ("clickConfig")) {
        rapidjson::Value array (rapidjson::kArrayType);
        doc.AddMember ("clickConfig", array, doc.GetAllocator());
    }

    doc["clickConfig"].PushBack (obj, doc.GetAllocator());
}

NodeItem * NodeItemClick::clone()
{
	NodeItemClick* item = NodeItemClick::create(m_pUserCtrlEditor, -1);
	item->m_bEnabled = m_bEnabled;
	item->m_rect = m_rect;
	item->m_aniIndex = m_aniIndex;
	item->doUI();
	return item;
}

l2dClickAnimation * NodeItemClick::getl2dClickAnimation()
{
	Mapl2dGestureAnimation& mapAnimaiton = m_pUserCtrlEditor->getModel()->getAnimationsMap();
	auto findRes = mapAnimaiton.find(m_ainimationId);
	if (findRes != mapAnimaiton.end())
	{
		return dynamic_cast<l2dClickAnimation*>(findRes->second);
	}
	else
	{
		return NULL;
	}
	return nullptr;
}

void NodeItemClick::updatel2dClickAnimation()
{
	l2dClickAnimation* aniOld = getl2dClickAnimation();
	if (aniOld)
	{
		m_pUserCtrlEditor->getModel()->removeAnimation(m_ainimationId);
		m_ainimationId = -1;
	}

	m_ainimationId = m_pUserCtrlEditor->getModel()->addClickAnimation(
		m_rect,
		m_aniIndex);

	l2dClickAnimation* aniNew = getl2dClickAnimation();
	aniNew->setEnabled(m_bEnabled);
}

void NodeItemClick::doUI()
{
	getChildByName<ui::TextField*>("isEnable: ")->setString(StringUtils::format("%d", m_bEnabled));

	getChildByName<ui::TextField*>("rect.x: ")->setString(StringUtils::format("%.2f", m_rect.origin.x));

	getChildByName<ui::TextField*>("rect.y: ")->setString(StringUtils::format("%.2f", m_rect.origin.y));

	getChildByName<ui::TextField*>("rect.w: ")->setString(StringUtils::format("%.2f", m_rect.size.width));

	getChildByName<ui::TextField*>("rect.h: ")->setString(StringUtils::format("%.2f", m_rect.size.height));

	getChildByName<ui::TextField*>("animation index :")->setString(StringUtils::format("%d", m_aniIndex));
}
