#ifndef __CC_LIVE2DMODEL_H__
#define __CC_LIVE2DMODEL_H__

#include "cocos2d.h"
#include <Live2DCubismCore.h>
#include "live2d/include/Live2DCubismFramework.h"
#include "live2d/include/Live2DCubismGlRendering.h"
#include <map>
#include <functional>
USING_NS_CC;

NS_CC_BEGIN

class Live2dModel : public Node {
public:
	typedef std::map<std::string, float> MapParemeter;
    struct CSMData {
        CSMData() :
            _csmMoc (NULL),
            _csmModel (NULL),
            _csmTbl (NULL),
            _csmAni (NULL),
            _csmAniState(),
            _csmGlRenderer (NULL) {
            _csmAniState.Time = 0;
			_mvp.setIdentity();
        }
		Mat4					_mvp;
        csmMoc*                 _csmMoc;
        csmModel*               _csmModel;
        csmModelHashTable*      _csmTbl;
        csmAnimation*           _csmAni;
        csmAnimationState       _csmAniState;
        csmGlRenderer*          _csmGlRenderer;
        std::vector<Texture2D*> _ccTexures;
    };

    Live2dModel();

    virtual ~Live2dModel();

    static Live2dModel* create (
        const std::string& modelPath,
        const std::string& jsonPath,
        const std::vector<std::string>& textureNames);

public:
    virtual bool init (
        const std::string& modelName,
        const std::string& jsonName,
        const std::vector<std::string>& textureNames);

	virtual const std::string& getModelName() const;

	virtual const std::string& getJsonName() const;

	virtual const std::vector<std::string>& getTextureNames() const;

    virtual void  playAnimation (const bool bLoop);

    virtual void  stopAnimation();

    virtual float getAnimationInterval();

    virtual bool  setCurrentAnimationTime (const float time);

    virtual float getCurrentAnimationTime();

    virtual bool  playSpecifyAnimation (const bool bLoop, const float timeStart, const float timeEnd);

    virtual void  setPlayEndFunc (std::function<void() > playEndFunc);

	virtual bool  getParameterValueRange(const std::string param, float& min, float& max);

    virtual void  setParameterValue (const std::string param, float value);

    virtual void  resetParameterValue (const std::string param);

    virtual void  setModelSize (const Size& sz);

    virtual const Size&  getModelSize() const;

    virtual bool  containsPoint (const Vec2& worldPoint);

public:

    virtual void draw (Renderer *renderer, const Mat4 &transform, uint32_t flags);

    void onDrawModel (const Mat4 &transform, uint32_t flags);

protected:

    GLfloat* updateMvp();

    Vec2 calcuDefaultScale();

    bool updateDrawModelParam();

    bool simpleCheckVisible();

    Rect getGlobalRect();

    Texture2D* loadAndRetainTexture (const char * path);

    void* read (const char* path, unsigned int* outSize);

    void* readAligned (const char* path, const unsigned int alignment, unsigned int* outSize);

private:
	std::string				m_modelName;
	std::string				m_jsonName;
	std::vector<std::string>m_textureNames;
    CustomCommand			m_customCommand;
    CSMData*				m_csmData;
    Vec2					m_defaultScale;
    float					m_lastUpdateTime;
    Size					m_modelSize;
	MapParemeter			m_mapParam;
	std::function<void() >  m_onPlayEndFunc;
};

NS_CC_END
#endif // !__CC_LIVE2DMODEL_H__
