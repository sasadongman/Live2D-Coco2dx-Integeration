#include "Live2dUserCtrl.h"

#define DistanceJudgeMoveSquared 9.f// 3.f * 3.f

#define _dy_cast(instanceNew, instanceOrigin, type) type* instanceNew=dynamic_cast<type*>(instanceOrigin)
l2dGestureAnimation::l2dGestureAnimation (const GestureType type, const Rect & rect) :
    m_type (type), m_rect (rect), m_id (l2dGestureAnimation::createId()), m_bEnabled (true) {
}

l2dGestureAnimation::~l2dGestureAnimation() {
}

const GestureType & l2dGestureAnimation::getType() const {
    return m_type;
}

const Rect & l2dGestureAnimation::getRect() const {
    return m_rect;
}

const int & l2dGestureAnimation::getId() const {
    return m_id;
}

bool l2dGestureAnimation::isEnabled() const {
    return m_bEnabled;
}

void l2dGestureAnimation::setEnabled (const bool bEnable) {
    m_bEnabled = bEnable;
}

int l2dGestureAnimation::createId() {
    static int s_id = 0;
    return ++s_id;
}

l2dClickAnimation::l2dClickAnimation (const Rect & rect, const int animationIndex) :
    l2dGestureAnimation (GestureType::eClick, rect), m_animationIndex (animationIndex) {
}

const int & l2dClickAnimation::getAnimationIndex() const {
    return m_animationIndex;
}

l2dDragAnimation::l2dDragAnimation (const Rect & rect, const float dragMaxLength, const float dragAngle, const std::string paramName) :
    l2dGestureAnimation (GestureType::eDrag, rect), m_dragMaxLength (dragMaxLength), m_dragAngle (dragAngle), m_paramName (paramName) {
}

const float & l2dDragAnimation::getDragMaxLength() const {
    return m_dragMaxLength;
}

const float & l2dDragAnimation::getDragAngle() const {
    return m_dragAngle;
}

const std::string & l2dDragAnimation::getParamName() const {
    return m_paramName;
}

Live2dUserCtrl::~Live2dUserCtrl() {
    auto iter = m_mapl2dGestureAnimation.begin();

    while (iter != m_mapl2dGestureAnimation.end()) {
        delete iter++->second;
    }
}

Live2dUserCtrl * Live2dUserCtrl::create (const std::string& modelPath, const std::string& jsonPath, const std::vector<std::string>& textureNames) {
    Live2dUserCtrl* pModel = new (std::nothrow) Live2dUserCtrl;

    if (pModel && pModel->init (modelPath, jsonPath, textureNames)) {
        pModel->autorelease();
        return pModel;
    }

    CC_SAFE_DELETE (pModel);
    return nullptr;
}

bool Live2dUserCtrl::init (const std::string& modelPath, const std::string& jsonPath, const std::vector<std::string>& textureNames) {
    if (!Live2dModel::init (modelPath, jsonPath, textureNames)) {
        return false;
    }

    m_curAniIndex = -1;
    m_bDraging = false;

    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = CC_CALLBACK_2 (Live2dUserCtrl::handleTouchBegin, this);
    touchListener->onTouchMoved = CC_CALLBACK_2 (Live2dUserCtrl::handleTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2 (Live2dUserCtrl::handleTouchEnd, this);
    touchListener->onTouchCancelled = CC_CALLBACK_2 (Live2dUserCtrl::handleTouchCancelled, this);
    touchListener->setSwallowTouches (true);
    touchListener->setEnabled (false);

    getEventDispatcher()->addEventListenerWithSceneGraphPriority (touchListener, this);

    m_touchListener = touchListener;
    return true;
}

void Live2dUserCtrl::splitAnimations (const std::vector<float>& times) {
    m_times = times;
}

int Live2dUserCtrl::addClickAnimation (const Rect & rect, const int animationIndex) {
    l2dGestureAnimation* ani = new l2dClickAnimation (rect, animationIndex);
    m_mapl2dGestureAnimation[ani->getId()] = ani;
    refreshTouchListenerEnable();
    return ani->getId();
}

int Live2dUserCtrl::addDragAnimation (const Rect & rect, const float dragMaxLength, const float dragAngle, const std::string & paramName) {
    l2dGestureAnimation* ani = new l2dDragAnimation (rect, dragMaxLength, dragAngle, paramName);
    m_mapl2dGestureAnimation[ani->getId()] = ani;
    refreshTouchListenerEnable();
    return ani->getId();
}

void Live2dUserCtrl::removeAnimation (const int id) {
    auto iter = m_mapl2dGestureAnimation.find (id);

    if (iter != m_mapl2dGestureAnimation.end()) {
        delete iter->second;
        m_mapl2dGestureAnimation.erase (iter);
    }

    refreshTouchListenerEnable();
}

void Live2dUserCtrl::removeAllAnimation() {
    auto iter = m_mapl2dGestureAnimation.begin();

    while (iter != m_mapl2dGestureAnimation.end()) {
        delete iter++->second;
    }

    refreshTouchListenerEnable();
}

void Live2dUserCtrl::setAnimationEnabled (const int id, const bool bEnabled) {
    auto iter = m_mapl2dGestureAnimation.find (id);

    if (iter != m_mapl2dGestureAnimation.end()) {
        iter->second->setEnabled (bEnabled);
    }

    refreshTouchListenerEnable();
}

void Live2dUserCtrl::setAllAnimationEnabled (const bool bEnabled) {
    auto iter = m_mapl2dGestureAnimation.begin();

    while (iter != m_mapl2dGestureAnimation.end()) {
        iter->second->setEnabled (bEnabled);
        ++iter;
    }
}

bool Live2dUserCtrl::isAnimationEnabled (const int id) const {
    auto iter = m_mapl2dGestureAnimation.find (id);

    if (iter != m_mapl2dGestureAnimation.end()) {
        return iter->second->isEnabled();
    }

    return false;
}

void Live2dUserCtrl::playAnimationByIndex (const int index) {
    if (index < 0 || index >= m_times.size()) {
        return;
    }

    m_curAniIndex = index;

    float startTime = index > 0 ? m_times[index - 1] : 0.f;
    float endTime = m_times[index];
    playSpecifyAnimation (false, startTime, endTime);
}

void Live2dUserCtrl::playAnimationNormal() {
    int nextIndex = getNextNormalIndex();

    playAnimationByIndex (nextIndex);

    setPlayEndFunc ([ = ]() {
        setPlayEndFunc (NULL);
        playAnimationNormal();
    });
}

bool Live2dUserCtrl::handleTouchBegin (Touch * touch, Event * event) {
    if (!containsPoint (touch->getLocation()))
        return false;

    m_bDraging = false;
    m_targetDragAniIds.clear();
    return true;
}

void Live2dUserCtrl::handleTouchMoved (Touch * touch, Event * event) {
    if (m_bDraging) {
        for (size_t i = 0; i < m_targetDragAniIds.size(); i++) {
            l2dGestureAnimation* ani = getGestureAnimationById (m_targetDragAniIds[i]);

            if (ani) {
                if (l2dDragAnimation* aniDrag = dynamic_cast<l2dDragAnimation*> (ani)) {
                    onDraging (aniDrag, touch);
                }
            }
        }
    } else {
        Vec2 startLoc = touch->getStartLocation();
        Vec2 curLoc = touch->getLocation();

        if ( (curLoc - startLoc).lengthSquared() > DistanceJudgeMoveSquared) {
            m_bDraging = true;

            std::vector<l2dGestureAnimation*> allClickAniForDrag = getGestureAnimaitionByTouch (GestureType::eDrag, touch);

            for (size_t i = 0; i < allClickAniForDrag.size(); i++) {
                m_targetDragAniIds.push_back (allClickAniForDrag[i]->getId());
            }
        } else {
            ;
        }
    }
}

void Live2dUserCtrl::handleTouchEnd (Touch * touch, Event * event) {
    if (m_bDraging) {
        for (size_t i = 0; i < m_targetDragAniIds.size(); i++) {
            l2dGestureAnimation* ani = getGestureAnimationById (m_targetDragAniIds[i]);

            if (ani) {
                if (l2dDragAnimation* aniDrag = dynamic_cast<l2dDragAnimation*> (ani)) {
                    onDragEnd (aniDrag, touch);
                }
            }
        }

        m_bDraging = false;
        m_targetDragAniIds.clear();
    } else {
        std::vector<l2dGestureAnimation*> allClickAniForTouch = getGestureAnimaitionByTouch (GestureType::eClick, touch);

        if (allClickAniForTouch.empty()) {
            return;
        }

        if (_dy_cast (clickAni, allClickAniForTouch[0], l2dClickAnimation)) {
            int aniIndex = clickAni->getAnimationIndex();

            if (aniIndex > 0 && aniIndex < m_times.size()) {
                playAnimationByIndex (aniIndex);
                setPlayEndFunc ([ = ]() {
                    setPlayEndFunc (NULL);
                    playAnimationNormal();
                });
            }
        }
    }
}

void Live2dUserCtrl::handleTouchCancelled (Touch * touch, Event * event) {
    handleTouchEnd (touch, event);
}

Mapl2dGestureAnimation& Live2dUserCtrl::getAnimationsMap() {
    return m_mapl2dGestureAnimation;
}

const Live2dUserCtrl::AnimationTimes& Live2dUserCtrl::getAnimationTimes()const {
    return m_times;
}

Rect Live2dUserCtrl::getGlobalRectForDev() {
    return getGlobalRect();
}

int Live2dUserCtrl::getNextNormalIndex() {
    auto getNextIndex = [ = ] (int index) {
        int nextIndex = index + 1;

        if (nextIndex >= m_times.size()) {
            nextIndex = 0;
        }

        return nextIndex;
    };

    int nextIndex = getNextIndex (m_curAniIndex);

    while (nextIndex != m_curAniIndex && !isAnimationIndexForNormal (nextIndex)) {
        nextIndex = getNextIndex (nextIndex);
    }

    return nextIndex;
}

void Live2dUserCtrl::refreshTouchListenerEnable() {
    auto iter = m_mapl2dGestureAnimation.begin();

    while (iter != m_mapl2dGestureAnimation.end()) {
        l2dGestureAnimation* ani = iter++->second;

        if (ani->isEnabled()) {
            m_touchListener->setEnabled (true);
            return;
        }
    }

    m_touchListener->setEnabled (false);
    return;
}

std::vector<l2dGestureAnimation*> Live2dUserCtrl::getGestureAnimaitionByTouch (const GestureType aniGestureType, Touch* touch) {
    std::vector<l2dGestureAnimation*> targetAnis;

    Rect modelRect = getGlobalRect();

    float scaleX = modelRect.size.width / getModelSize().width;
    float scaleY = modelRect.size.height / getModelSize().height;

    auto iter = m_mapl2dGestureAnimation.begin();

    while (iter != m_mapl2dGestureAnimation.end()) {
        l2dGestureAnimation* ani = iter++->second;

        if (!ani->isEnabled()) {
            continue;
        }

        if (ani->getType() != aniGestureType) {
            continue;
        }

        Rect rect = ani->getRect();
        rect.origin += modelRect.origin;
        rect.size.width *= scaleX;
        rect.size.height *= scaleY;

        if (rect.containsPoint (touch->getLocation())) {
            targetAnis.push_back (ani);
        }
    }

    return targetAnis;
}

bool Live2dUserCtrl::isAnimationIndexForNormal (const int animationIndex) {
    auto iter = m_mapl2dGestureAnimation.begin();

    while (iter != m_mapl2dGestureAnimation.end()) {
        l2dGestureAnimation* ani = iter++->second;

        if (ani->getType() != GestureType::eClick) {
            continue;
        }

        _dy_cast (aniClick, ani, l2dClickAnimation);

        if (aniClick && aniClick->getAnimationIndex() == animationIndex) {
            return false;
        }
    }

    return true;
}

void Live2dUserCtrl::onDraging (l2dDragAnimation* aniDrag, Touch * touch) {
    Vec2 defaultDir = Vec2::forAngle (aniDrag->getDragAngle() / 180 * 3.14);

    Vec2 startLoc = touch->getStartLocation();
    Vec2 currentLoc = touch->getLocation();

    Vec2 offset = currentLoc - startLoc;
    offset = offset.project (defaultDir);

    float paramPercent = offset.length() / aniDrag->getDragMaxLength();

    float minParam = 0.f, maxParam = 0.f;
    getParameterValueRange (aniDrag->getParamName(), minParam, maxParam);

    bool bDirPostive = abs (Vec2::angle (offset, defaultDir)) < 0.1;
    float param = bDirPostive ? maxParam * paramPercent : minParam * paramPercent;
    setParameterValue (aniDrag->getParamName(), param);
}

void Live2dUserCtrl::onDragEnd (l2dDragAnimation* aniDrag, Touch * touch) {
    resetParameterValue (aniDrag->getParamName());
}

l2dGestureAnimation * Live2dUserCtrl::getGestureAnimationById (const int id) {
    auto findRes = m_mapl2dGestureAnimation.find (id);

    if (findRes == m_mapl2dGestureAnimation.end()) {
        return NULL;
    }

    return findRes->second;
}