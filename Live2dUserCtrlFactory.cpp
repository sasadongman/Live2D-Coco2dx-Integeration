#include "Live2dUserCtrlFactory.h"
#include "cocos2d.h"
#include <vector>

#define assert_return_1(condit) if(!(condit)){return 1;}
#define assert_return_0(condit) if(!(condit)){return 0;}
#define assert_delete_return_0(condit, obj) if(!(condit)) \
{\
    delete obj;\
    obj = NULL;\
    return 0;\
}

using rapidjson::Document;

Live2dUserCtrl * Live2dUserCtrlFactory::create (const std::string & configFile) {
    std::string jsonString = cocos2d::FileUtils::getInstance()->getStringFromFile (configFile);

    Document doc;
    doc.Parse<0> (jsonString.c_str());
    assert_return_0 (!doc.HasParseError());

    Live2dUserCtrl* l2dUserCtrl = createL2dUserCtrl (configFile, doc);
    assert_return_0 (l2dUserCtrl);

	assert_delete_return_0(initPosition(l2dUserCtrl, doc), l2dUserCtrl);
    assert_delete_return_0 (initScale (l2dUserCtrl, doc), l2dUserCtrl);
    assert_delete_return_0 (initSize (l2dUserCtrl, doc), l2dUserCtrl);
    assert_delete_return_0 (initAnimationTimes (l2dUserCtrl, doc), l2dUserCtrl);
    assert_delete_return_0 (initClickConfig (l2dUserCtrl, doc), l2dUserCtrl);
    assert_delete_return_0 (initDragConfig (l2dUserCtrl, doc), l2dUserCtrl);

	l2dUserCtrl->playAnimationNormal();
	return l2dUserCtrl;
}

Live2dUserCtrl * Live2dUserCtrlFactory::createL2dUserCtrl (const std::string & configJsonFile, Document & doc) {
    assert_return_0 (doc.HasMember ("modelName"));
    assert_return_0 (doc.HasMember ("jsonName"));
    assert_return_0 (doc.HasMember ("textureList"));

    rapidjson::Value& vModelName = doc["modelName"];
    rapidjson::Value& vJsonName = doc["jsonName"];
    rapidjson::Value& vTextureList = doc["textureList"];
    assert_return_0 (vModelName.IsString());
    assert_return_0 (vJsonName.IsString());
    assert_return_0 (vTextureList.IsArray());

    FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
    std::string modelName = fileUtils->fullPathFromRelativeFile (vModelName.GetString(), configJsonFile);
    std::string jsonName = fileUtils->fullPathFromRelativeFile (vJsonName.GetString(), configJsonFile);

	std::vector<std::string> textureList;

    for (size_t i = 0; i < vTextureList.Size(); i++) {
        assert_return_0 (vTextureList[i].IsString());
        std::string texture = fileUtils->fullPathFromRelativeFile(vTextureList[i].GetString(), configJsonFile);
        textureList.push_back(texture);
    }

    return Live2dUserCtrl::create (modelName.c_str(), jsonName.c_str(), textureList);
}

bool Live2dUserCtrlFactory::initPosition(Live2dUserCtrl * l2dUserCtrl, Document & doc)
{
	assert_return_0(doc.HasMember("position"));

	rapidjson::Value& vPosition = doc["position"];
	assert_return_0(vPosition.IsObject());

	assert_return_0(vPosition.HasMember("x"));

	rapidjson::Value& vX = vPosition["x"];
	assert_return_0(vX.IsNumber());

	float x = vX.GetDouble();

	assert_return_0(vPosition.HasMember("y"));

	rapidjson::Value& vY = vPosition["y"];
	assert_return_0(vY.IsNumber());

	float y = vY.GetDouble();
	l2dUserCtrl->setPosition(x, y);

	return true;
}

bool Live2dUserCtrlFactory::initScale (Live2dUserCtrl * l2dUserCtrl, Document & doc) {
    assert_return_0 (doc.HasMember ("scale"));

    rapidjson::Value& vScale = doc["scale"];
    assert_return_0 (vScale.IsNumber());

    float scale = vScale.GetDouble();
    assert_return_0 (scale > 0);

    l2dUserCtrl->setScale (scale);
    return true;
}

bool Live2dUserCtrlFactory::initSize (Live2dUserCtrl * l2dUserCtrl, Document & doc) {
    assert_return_0 (doc.HasMember ("size"));

    rapidjson::Value& vSize = doc["size"];
    assert_return_0 (vSize.IsObject());

    assert_return_0 (vSize.HasMember ("width"));

    rapidjson::Value& vWidth = vSize["width"];
    assert_return_0 (vWidth.IsNumber());

    float width = vWidth.GetDouble();

    assert_return_0 (vSize.HasMember ("height"));

    rapidjson::Value& vHeight = vSize["height"];
    assert_return_0 (vHeight.IsNumber());

    float height = vHeight.GetDouble();
    l2dUserCtrl->setModelSize (Size (width, height));

    return true;
}

bool Live2dUserCtrlFactory::initAnimationTimes (Live2dUserCtrl * l2dUserCtrl, Document & doc) {
    assert_return_0 (doc.HasMember ("times"));

    rapidjson::Value& vTimes = doc["times"];
    assert_return_0 (vTimes.IsArray());

    std::vector<float> times;

    for (size_t i = 0; i < vTimes.Size(); i++) {
        assert_return_0 (vTimes[i].IsNumber());
        times.push_back (vTimes[i].GetDouble());
    }

    if (times.empty()) {
        times.push_back (l2dUserCtrl->getAnimationInterval());
    }

    l2dUserCtrl->splitAnimations (times);
    return true;
}

bool Live2dUserCtrlFactory::initClickConfig (Live2dUserCtrl * l2dUserCtrl, Document & doc) {
    assert_return_1 (doc.HasMember ("clickConfig"));

    rapidjson::Value& vClickConfig = doc["clickConfig"];
    assert_return_0 (vClickConfig.IsArray());

    for (size_t i = 0; i < vClickConfig.Size(); i++) {
        assert_return_0 (vClickConfig[i].IsObject());
        assert_return_0 (vClickConfig[i].HasMember ("bEnable"));
        assert_return_0 (vClickConfig[i].HasMember ("rectOriginX"));
        assert_return_0 (vClickConfig[i].HasMember ("rectOriginY"));
        assert_return_0 (vClickConfig[i].HasMember ("rectWidth"));
        assert_return_0 (vClickConfig[i].HasMember ("rectHeight"));
        assert_return_0 (vClickConfig[i].HasMember ("animationIndex"));

        rapidjson::Value& vBEnable = vClickConfig[i]["bEnable"];
        rapidjson::Value& vRectOriginX = vClickConfig[i]["rectOriginX"];
        rapidjson::Value& vRectOriginY = vClickConfig[i]["rectOriginY"];
        rapidjson::Value& vRectWidth = vClickConfig[i]["rectWidth"];
        rapidjson::Value& vRectHeight = vClickConfig[i]["rectHeight"];
        rapidjson::Value& vAnimationIndex = vClickConfig[i]["animationIndex"];

        assert_return_0 (vBEnable.IsNumber());
        assert_return_0 (vRectOriginX.IsNumber());
        assert_return_0 (vRectOriginY.IsNumber());
        assert_return_0 (vRectWidth.IsNumber());
        assert_return_0 (vRectHeight.IsNumber());
        assert_return_0 (vAnimationIndex.IsNumber());

        bool bEnable = (bool) vBEnable.GetInt();
        float rectOriginX = vRectOriginX.GetDouble();
        float rectOriginY = vRectOriginY.GetDouble();
        float rectWidth = vRectWidth.GetDouble();
        float rectHeight = vRectHeight.GetDouble();
        int nAnimationIndex = vAnimationIndex.GetInt();

        int aniId = l2dUserCtrl->addClickAnimation (
                        Rect (rectOriginX, rectOriginY, rectWidth, rectHeight),
                        nAnimationIndex);

        if (!bEnable) {
            l2dUserCtrl->setAnimationEnabled (aniId, false);
        }
    }

    return true;
}

bool Live2dUserCtrlFactory::initDragConfig (Live2dUserCtrl * l2dUserCtrl, Document & doc) {
    assert_return_1 (doc.HasMember ("dragConfig"));

    rapidjson::Value& vDragConfig = doc["dragConfig"];
    assert_return_0 (vDragConfig.IsArray());

    for (size_t i = 0; i < vDragConfig.Size(); i++) {
        assert_return_0 (vDragConfig[i].IsObject());
        assert_return_0 (vDragConfig[i].HasMember ("bEnable"));
        assert_return_0 (vDragConfig[i].HasMember ("rectOriginX"));
        assert_return_0 (vDragConfig[i].HasMember ("rectOriginY"));
        assert_return_0 (vDragConfig[i].HasMember ("rectWidth"));
        assert_return_0 (vDragConfig[i].HasMember ("rectHeight"));
        assert_return_0 (vDragConfig[i].HasMember ("paramName"));
        assert_return_0 (vDragConfig[i].HasMember ("dragAngle"));
        assert_return_0 (vDragConfig[i].HasMember ("dragMaxLength"));

        rapidjson::Value& vBEnable = vDragConfig[i]["bEnable"];
        rapidjson::Value& vRectOriginX = vDragConfig[i]["rectOriginX"];
        rapidjson::Value& vRectOriginY = vDragConfig[i]["rectOriginY"];
        rapidjson::Value& vRectWidth = vDragConfig[i]["rectWidth"];
        rapidjson::Value& vRectHeight = vDragConfig[i]["rectHeight"];
        rapidjson::Value& vParamName = vDragConfig[i]["paramName"];
        rapidjson::Value& vDragAngle = vDragConfig[i]["dragAngle"];
        rapidjson::Value& vDragMaxLength = vDragConfig[i]["dragMaxLength"];

        assert_return_0 (vBEnable.IsNumber());
        assert_return_0 (vRectOriginX.IsNumber());
        assert_return_0 (vRectOriginY.IsNumber());
        assert_return_0 (vRectWidth.IsNumber());
        assert_return_0 (vRectHeight.IsNumber());
        assert_return_0 (vParamName.IsString());
        assert_return_0 (vDragAngle.IsNumber());
        assert_return_0 (vDragMaxLength.IsNumber());

        bool bEnable = (bool) vBEnable.GetInt();
        float rectOriginX = vRectOriginX.GetDouble();
        float rectOriginY = vRectOriginY.GetDouble();
        float rectWidth = vRectWidth.GetDouble();
        float rectHeight = vRectHeight.GetDouble();
        float dragAngle = vDragAngle.GetDouble();
        float dragMaxLength = vDragMaxLength.GetDouble();
        std::string paramName = vParamName.GetString();

        int aniId = l2dUserCtrl->addDragAnimation (
                        Rect (rectOriginX, rectOriginY, rectWidth, rectHeight),
                        dragMaxLength,
                        dragAngle,
                        paramName);

        if (!bEnable) {
            l2dUserCtrl->setAnimationEnabled (aniId, false);
        }
    }

    return true;
}
