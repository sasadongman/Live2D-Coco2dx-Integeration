#ifndef __LIVE2DUSERCTRLFACTORY_H__
#define __LIVE2DUSERCTRLFACTORY_H__

#include "Live2dUserCtrl.h"
#include "json/document.h"

using rapidjson::Document;
using rapidjson::Value;
/*------------------------------------json file----------------------------------
{
"注释-modelName": "模型名字",
"modelName": "21.moc3",

"注释-jsonName": "json文件的名字",
"jsonName": "Scene1.motion3.json",

"注释-textureList": "纹理的列表, 不能为空",
"textureList": [
"21.2048/texture_00.png"
],

"注释-position": "模型的位置"
"position":{
"x":100,
"y":100
}

"注释-scale": "模型显示的缩放比例",
"scale": 1,

"注释-size": "scale为1时的模型大小",
"size": {
"width": 150,
"height": 325
},

"注释-times": "动画按时间分段。如果数组为空，那么整个动画为一段。[0,3],[3,5]",
"times": [
3, 5
],

"注释-clickConfig0": "点击的响应配置。（可在代码中自定义更为灵活的点击方式）",
"注释-clickConfig1": "bEnable为0则表示不支持点击操作。",
"注释-clickConfig2": "animationIndex 当玩家点击的时候，播放第clickIndex段动画。",
"注释-clickConfig3": "rectxxx 点击的响应区域（scale为1时）。",
"clickConfig": [{
"bEnable": 1,
"rectOriginX": 0,
"rectOriginY": 0,
"rectWidth": 100,
"rectHeight": 100,
"animationIndex": 1
}],


"注释-dragConfig0": "拖拽的响应配置。",
"注释-dragConfig1": "bEnable为0则表示不支持拖拽操作（可在代码中自定义更为灵活的拖拽方式）",
"注释-dragConfig2": "paramTable参数列表",
"注释-dragConfig3": "paramName参数名",
"注释-dragConfig4": "dragAngle拖拽角度，沿X轴正向为0，y轴正向为90",
"注释-dragConfig5": "dragMaxLength拖拽的最大长度。",
"dragConfig": [{
"bEnable": 1,
"rectOriginX": 0,
"rectOriginY": 0,
"rectWidth": 100,
"rectHeight": 100,
"paramName": "",
"dragAngle": 90,
"dragMaxLength": 100
}]

}
---------------------------------------------------------------------------------*/


class Live2dUserCtrlFactory {
public:
    static Live2dUserCtrl* create (const std::string& configFile);

private:
    static Live2dUserCtrl* createL2dUserCtrl (const std::string& jsonFile, Document& doc);
	static bool initPosition(Live2dUserCtrl* l2dUserCtrl, Document& doc);
    static bool initScale (Live2dUserCtrl* l2dUserCtrl, Document& doc);
    static bool initSize (Live2dUserCtrl* l2dUserCtrl, Document& doc);
    static bool initAnimationTimes (Live2dUserCtrl* l2dUserCtrl, Document& doc);
    static bool initClickConfig (Live2dUserCtrl* l2dUserCtrl, Document& doc);
    static bool initDragConfig (Live2dUserCtrl* l2dUserCtrl, Document& doc);
};

#endif // !__LIVE2DUSERCTRLFACTORY
