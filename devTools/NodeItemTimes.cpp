#include "NodeItemTimes.h"

static std::vector<std::string> &split (const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss (s);
    std::string item;

    while (std::getline (ss, item, delim)) {
        elems.push_back (item);
    }

    return elems;
}

NodeItemTimes * NodeItemTimes::create (LayerUserCtrlEditor * pUserCtrlEditor) {
    NodeItemTimes *ret = new (std::nothrow) NodeItemTimes();

    if (ret && ret->init (pUserCtrlEditor)) {
        ret->autorelease();
        return ret;
    } else {
        CC_SAFE_DELETE (ret);
        return nullptr;
    }
}

bool NodeItemTimes::init (LayerUserCtrlEditor * pUserCtrlEditor) {
    if (!NodeItem::init (pUserCtrlEditor)) {
        return false;
    }

	m_times = m_pUserCtrlEditor->getModel()->getAnimationTimes();
    addEditBox ("times:  ", 0, getStringTimes(), [&] (std::string s) {
        std::vector <std::string> times;
        split (s, ',', times);

        m_times.clear();

        for (size_t i = 0; i < times.size(); i++) {
            if (!times[i].empty()) {
                float time = __String (times[i]).floatValue();
                m_times.push_back (time);
            }
        }

        m_pUserCtrlEditor->getModel()->splitAnimations (m_times);
        m_pUserCtrlEditor->getModel()->playAnimationNormal();
    });

    addLabel ("(split animation into multiple parts)", 0);

    updateItem();
    return true;
}

std::string NodeItemTimes::checkParams() {
    float lastTimes = 0.f;

    for (size_t i = 0; i < m_times.size(); i++) {
        if (m_times[i] <= lastTimes) {
            return "Err: invald parameter [times].";
        }
    }

    return "";
}

void NodeItemTimes::addDataToJsonDoc (rapidjson::Document & doc) {
    rapidjson::Value array (rapidjson::kArrayType);

    for (size_t i = 0; i < m_times.size(); i++) {
        array.PushBack (m_times[i], doc.GetAllocator());
    }

    doc.AddMember ("times", array, doc.GetAllocator());
}

std::string NodeItemTimes::getStringTimes()
{
	std::string stringTimes;
	std::string splitChar = "";
	for (size_t i = 0; i < m_times.size(); i++)
	{
		stringTimes += (splitChar + StringUtils::format("%.2f", m_times[i]));
		splitChar = ",";
	}
	return stringTimes;
}
