#include "Live2dModel.h"
#include "live2d\framework\Local.h"


NS_CC_BEGIN

Live2dModel::Live2dModel() {
    m_csmData = new Live2dModel::CSMData();
    _scaleX = 1.f;
    _scaleY = 1.f;
    _position = Vec2 (0.f, 0.f);
}

Live2dModel::~Live2dModel() {
    // Release resources.
    if (m_csmData) {
        for (size_t i = 0; i < m_csmData->_ccTexures.size(); i++) {
            m_csmData->_ccTexures[i]->release();
        }

        m_csmData->_ccTexures.clear();

        csmReleaseGlRenderer (m_csmData->_csmGlRenderer);

        // Free memory.
        Deallocate (m_csmData->_csmGlRenderer);
        Deallocate (m_csmData->_csmAni);
        Deallocate (m_csmData->_csmTbl);
        DeallocateAligned (m_csmData->_csmModel);
        DeallocateAligned (m_csmData->_csmMoc);

        CC_SAFE_DELETE (m_csmData);
    }
}

Live2dModel * Live2dModel::create (
    const std::string& modelPath,
    const std::string& jsonPath,
    const std::vector<std::string>& textureName) {
    Live2dModel* pModel = new (std::nothrow) Live2dModel;

    if (pModel && pModel->init (modelPath, jsonPath, textureName)) {
        pModel->autorelease();
        return pModel;
    }

    CC_SAFE_DELETE (pModel);
    return nullptr;
}

bool Live2dModel::init (
	const std::string& modelName, 
	const std::string& jsonName, 
	const std::vector<std::string>& textureName) {
    if (!Node::init()) {
        return false;
    }
	m_modelName = modelName;
	m_jsonName = jsonName;
	m_textureNames = textureName;

    // Load and revive moc.
    unsigned int mocSize = 0;
    void* mocMemory = readAligned (modelName.c_str(), csmAlignofMoc, &mocSize);
    m_csmData->_csmMoc = csmReviveMocInPlace (mocMemory, mocSize);

    // Instantiate moc.
    unsigned int modelSize = csmGetSizeofModel (m_csmData->_csmMoc);
    void* modelMemory = AllocateAligned (modelSize, csmAlignofModel);
    m_csmData->_csmModel = csmInitializeModelInPlace (m_csmData->_csmMoc, modelMemory, modelSize);

    // Create hash table.
    unsigned int tblSize = csmGetSizeofModelHashTable (m_csmData->_csmModel);
    void* tblMemory = Allocate (tblSize);
    m_csmData->_csmTbl = csmInitializeModelHashTableInPlace (m_csmData->_csmModel, tblMemory, tblSize);

    // Load animation.
    unsigned int jsonSize = 0;
    void* motionJsonMemory = read (jsonName.c_str(), &jsonSize);
    unsigned int aniSize = csmGetDeserializedSizeofAnimation ( (char*) motionJsonMemory);
    void* aniMemory = Allocate (aniSize);
    m_csmData->_csmAni = csmDeserializeAnimationInPlace ( (char*) motionJsonMemory, aniMemory, aniSize);
    Deallocate (motionJsonMemory);

    // Initialize animation state.
    csmInitializeAnimationState (&m_csmData->_csmAniState, m_csmData->_csmAni->Duration);

    // Create renderer.
    unsigned int rendererSize = csmGetSizeofGlRenderer (m_csmData->_csmModel);
    void* rendererMemory = Allocate (rendererSize);
    m_csmData->_csmGlRenderer = csmMakeGlRendererInPlace (m_csmData->_csmModel, rendererMemory, rendererSize);

    // Load texture.
    for (size_t i = 0; i < textureName.size(); i++) {
        Texture2D* texture = loadAndRetainTexture (textureName[i].c_str());
        m_csmData->_ccTexures.push_back (texture);
    }

    // init local data
    m_onPlayEndFunc = NULL;

    m_defaultScale = calcuDefaultScale();

    m_lastUpdateTime = 0;

    m_modelSize = Director::getInstance()->getVisibleSize();

    return true;
}

const std::string & Live2dModel::getModelName() const
{
	return m_modelName;
}

const std::string & Live2dModel::getJsonName() const
{
	return m_jsonName;
}

const std::vector<std::string>& Live2dModel::getTextureNames() const
{
	return m_textureNames;
}

void Live2dModel::playAnimation (const bool bLoop) {
    csmSetAniEnabled (&m_csmData->_csmAniState, true);
    csmSetLoopEnabled (&m_csmData->_csmAniState, bLoop);
    setCurrentAnimationTime (m_csmData->_csmAniState.TimeAniStart);
}

void Live2dModel::stopAnimation() {
    csmSetAniEnabled (&m_csmData->_csmAniState, false);
}

float Live2dModel::getAnimationInterval() {
    return m_csmData->_csmAniState.Duration;
}

bool Live2dModel::setCurrentAnimationTime (const float time) {
    return csmSetCurrentAnimationTime (&m_csmData->_csmAniState, time);
}

float Live2dModel::getCurrentAnimationTime() {
    return m_csmData->_csmAniState.Time;
}

bool Live2dModel::playSpecifyAnimation (const bool bLoop, const float timeStart, const float timeEnd) {
    if (!csmSetTimeAniStart (&m_csmData->_csmAniState, timeStart))
        return false;

    if (!csmSetTimeAniEnd (&m_csmData->_csmAniState, timeEnd))
        return false;

    playAnimation (bLoop);
    return true;
}

void Live2dModel::setPlayEndFunc (std::function<void() > playEndFunc) {
    m_onPlayEndFunc = playEndFunc;
}

bool Live2dModel::getParameterValueRange (const std::string param, float & min, float & max) {
    int count = csmGetParameterCount (m_csmData->_csmModel);
    const char** paramname = csmGetParameterIds (m_csmData->_csmModel);
    const float* minValues = csmGetParameterMinimumValues (m_csmData->_csmModel);
    const float* maxValues = csmGetParameterMaximumValues (m_csmData->_csmModel);
    float* values = csmGetParameterValues (m_csmData->_csmModel);

    for (size_t i = 0; i < count; i++) {
        std::string name = StringUtils::format ("%s", paramname[i]);

        if (name == param) {
            min = minValues[i];
            max = maxValues[i];
            return true;
        }
    }

    return false;
}

void Live2dModel::draw (Renderer * renderer, const Mat4 & transform, uint32_t flags) {
    if (simpleCheckVisible()) {
        m_customCommand.init (_globalZOrder);
        m_customCommand.func = CC_CALLBACK_0 (Live2dModel::onDrawModel, this, transform, flags);
        renderer->addCommand (&m_customCommand);
    }
}

void Live2dModel::onDrawModel (const Mat4 &transform, uint32_t flags) {
    kmGLPushMatrix();
    kmGLLoadMatrix (&transform);

    bool bPlayEndCallFunc = csmUpdateAnimationState (
                                &m_csmData->_csmAniState,
                                Director::getInstance()->getDeltaTime());

    bool bNeedUpdate = m_lastUpdateTime != m_csmData->_csmAniState.Time;
    m_lastUpdateTime = m_csmData->_csmAniState.Time;

    if (bNeedUpdate) {
        csmEvaluateAnimationFAST (m_csmData->_csmAni,
                                  &m_csmData->_csmAniState,
                                  csmOverrideFloatBlendFunction,
                                  1.0f,
                                  m_csmData->_csmModel,
                                  m_csmData->_csmTbl,
                                  0,
                                  0);
    }

    if (updateDrawModelParam() || bNeedUpdate) {
        csmUpdateModel (m_csmData->_csmModel);
        csmUpdateGlRenderer (m_csmData->_csmGlRenderer);
        csmResetDrawableDynamicFlags (m_csmData->_csmModel);
    }

    GLuint textureName[128] = { 0 };

    for (size_t i = 0; i < m_csmData->_ccTexures.size(); i++) {
        textureName[i] = m_csmData->_ccTexures[i]->getName();
    }

    csmGlDraw (m_csmData->_csmGlRenderer, updateMvp(), textureName);

    if (bPlayEndCallFunc && m_onPlayEndFunc) {
        m_onPlayEndFunc();
    }

    kmGLPopMatrix();
}

void Live2dModel::setParameterValue (const std::string param, float value) {
	m_mapParam[param] = value;
}

void Live2dModel::resetParameterValue (const std::string param) {
    int count = csmGetParameterCount (m_csmData->_csmModel);
    const char** paramname = csmGetParameterIds (m_csmData->_csmModel);
    const float* defValues = csmGetParameterDefaultValues (m_csmData->_csmModel);
    float* values = csmGetParameterValues (m_csmData->_csmModel);

    for (size_t i = 0; i < count; i++) {
        std::string name = StringUtils::format ("%s", paramname[i]);

        if (name == param) {
            values[i] = defValues[i];
            auto findRes = m_mapParam.find (param);

            if (findRes != m_mapParam.end()) {
                m_mapParam.erase (findRes);
            }
        }
    }
}

void Live2dModel::setModelSize (const Size & sz) {
    m_modelSize = sz;
}

const Size& Live2dModel::getModelSize() const{
    return m_modelSize;
}

bool Live2dModel::containsPoint (const Vec2 & worldPoint) {
    return getGlobalRect().containsPoint (worldPoint);
}

GLfloat * Live2dModel::updateMvp() {
    m_csmData->_mvp = getNodeToWorldTransform();

    Size szVisible = Director::getInstance()->getVisibleSize();
	m_csmData->_mvp.m[12] = 2.f * m_csmData->_mvp.m[12] / szVisible.width - 1.f;
	m_csmData->_mvp.m[13] = 2.f * m_csmData->_mvp.m[13] / szVisible.height - 1.f;

	m_csmData->_mvp.m[0] *= m_defaultScale.x;
	m_csmData->_mvp.m[5] *= m_defaultScale.y;

    return m_csmData->_mvp.m;
}

Vec2 Live2dModel::calcuDefaultScale() {
    Vec2 defaultScale;
    Size szVisible = Director::getInstance()->getVisibleSize();
    float aspect = szVisible.width / szVisible.height;
    defaultScale.x = 1.f / szVisible.width;
    defaultScale.y = 1.f / szVisible.height;
    defaultScale = defaultScale.getNormalized();

    csmVector2 outSizeInPixel;
    csmVector2 outOriginalInPixel;
    float outPixelPerUnit = 0.f;
    csmReadCanvasInfo (m_csmData->_csmModel, &outSizeInPixel, &outOriginalInPixel, &outPixelPerUnit);

    float scale = 0.f;

    if (outPixelPerUnit == outSizeInPixel.X) {
        scale = outPixelPerUnit / szVisible.width;
    } else {
        scale = outPixelPerUnit / szVisible.height;
    }

    defaultScale *= scale;
    return defaultScale;
}

bool Live2dModel::updateDrawModelParam() {
    int count = csmGetParameterCount (m_csmData->_csmModel);
    const char** paramname = csmGetParameterIds (m_csmData->_csmModel);
    const float* maxValues = csmGetParameterMaximumValues (m_csmData->_csmModel);
    const float* minValues = csmGetParameterMinimumValues (m_csmData->_csmModel);
    float* values = csmGetParameterValues (m_csmData->_csmModel);

    for (size_t i = 0; i < count; i++) {
        std::string name = StringUtils::format ("%s", paramname[i]);
        auto findRes = m_mapParam.find (name);

        if (findRes != m_mapParam.end()) {
            float value = findRes->second;
            value = std::min (value, maxValues[i]);
            value = std::max (value, minValues[i]);
            values[i] = value;
        }
    }

    return !m_mapParam.empty();
}

bool Live2dModel::simpleCheckVisible() {
    if (!isVisible()) {
        return false;
    }

    if (!getParent()) {
        return false;
    }

    Rect rectGlobal = getGlobalRect();
    Rect rectScreen (Vec2 (0, 0), Director::getInstance()->getVisibleSize());

    return rectScreen.intersectsRect (rectGlobal);
}

Rect Live2dModel::getGlobalRect() {
    Node* parent = getParent();

    if (!parent) {
        return Rect();
    }

    Vec2 sz = m_modelSize;
    {
        Node* node = this;

        while (node) {
            sz.x *= node->getScaleX();
            sz.y *= node->getScaleY();
            node = node->getParent();
        }
    }

    csmVector2 outSizeInPixel;
    csmVector2 outOriginalInPixel;
    float outPixelPerUnit = 0.f;
    csmReadCanvasInfo (m_csmData->_csmModel, &outSizeInPixel, &outOriginalInPixel, &outPixelPerUnit);

    Vec2 anchor (outOriginalInPixel.X / outSizeInPixel.X, outOriginalInPixel.Y / outSizeInPixel.Y);
    Vec2 pos = getPosition();

    pos.x -= sz.x * anchor.x;
    pos.y -= sz.y * anchor.y;
    pos = parent->convertToWorldSpace (pos);

    return Rect (pos, (Size) sz);
}

Texture2D * Live2dModel::loadAndRetainTexture (const char * path) {
    auto texture = Director::getInstance()->getTextureCache()->addImage (std::string (path));

    const Texture2D::TexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };

    texture->setTexParameters (texParams);
    texture->generateMipmap();
    texture->retain();
    return texture;
}

void * Live2dModel::read (const char * path, unsigned int * outSize) {
    Data data = FileUtils::getInstance()->getDataFromFile (path);

    *outSize = data.getSize();

    void* buffer = Allocate (*outSize);

    memcpy (buffer, data.getBytes(), *outSize);

    return buffer;
}

void * Live2dModel::readAligned (const char * path, const unsigned int alignment, unsigned int * outSize) {
    Data data = FileUtils::getInstance()->getDataFromFile (path);

    *outSize = data.getSize();

    void* buffer = AllocateAligned (*outSize, alignment);

    memcpy (buffer, data.getBytes(), *outSize);

    return buffer;
}

NS_CC_END
