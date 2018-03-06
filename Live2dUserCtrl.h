#ifndef __LIVE2DUSERCTRL_H__
#define __LIVE2DUSERCTRL_H__

#include "Live2dModel.h"
#include <functional>
#include <map>

#define DevToolApi

enum GestureType {
    eClick,
    eDrag
};

class l2dGestureAnimation {
public:
    static int createId();

    l2dGestureAnimation (const GestureType type, const Rect& rect);
    virtual ~l2dGestureAnimation();

    const GestureType& getType() const;
    const Rect& getRect() const;
    const int& getId() const;
    bool isEnabled() const;
    void setEnabled (const bool bEnable);

protected:
    const GestureType m_type;
    const int m_id;
    const Rect m_rect;
    bool  m_bEnabled;
};

class l2dClickAnimation : public l2dGestureAnimation {
public:
    l2dClickAnimation (const Rect& rect, const int animationIndex);
    const int& getAnimationIndex() const;
protected:
    const int m_animationIndex;
};

class l2dDragAnimation : public l2dGestureAnimation {
public:
    l2dDragAnimation (const Rect& rect, const float dragMaxLength, const float dragAngle, const std::string paramName);
    const float& getDragMaxLength() const;
    const float& getDragAngle() const;
    const std::string& getParamName() const;
protected:
    const float m_dragMaxLength;
    const float m_dragAngle;
    const std::string m_paramName;
};

typedef std::map<int, l2dGestureAnimation*>     Mapl2dGestureAnimation;
typedef Mapl2dGestureAnimation::iterator        Mapl2dGestureAnimationIter;

class Live2dUserCtrl : public Live2dModel {
public:
    typedef std::vector<float>          AnimationTimes;

public:
    ~Live2dUserCtrl();

    static Live2dUserCtrl* create (
        const std::string& modelPath,
        const std::string& jsonPath,
        const std::vector<std::string>& textureNames);

    virtual bool init (
        const std::string& modelPath,
        const std::string& jsonPath,
        const std::vector<std::string>& textureNames);

    virtual void splitAnimations (const std::vector<float>& times);

    virtual int  addClickAnimation (const Rect& rect, const int animationIndex);

    virtual int  addDragAnimation (const Rect& rect, const float dragMaxLength, const float dragAngle, const std::string& paramName);

    virtual void removeAnimation (const int id);

    virtual void removeAllAnimation();

    virtual void setAnimationEnabled (const int id, const bool bEnabled);

    virtual void setAllAnimationEnabled (const bool bEnabled);

    virtual bool isAnimationEnabled (const int id) const;

    virtual void playAnimationByIndex (const int index);

    virtual void playAnimationNormal();

public:
    bool handleTouchBegin (Touch* touch, Event* event);

    void handleTouchMoved (Touch* touch, Event* event);

    void handleTouchEnd (Touch* touch, Event* event);

    void handleTouchCancelled (Touch* touch, Event* event);

public:// only for develop tools.

    DevToolApi Mapl2dGestureAnimation& getAnimationsMap();

    DevToolApi const AnimationTimes& getAnimationTimes() const;

    DevToolApi Rect getGlobalRectForDev();

protected:
    int  getNextNormalIndex();

    void refreshTouchListenerEnable();

    std::vector<l2dGestureAnimation*> getGestureAnimaitionByTouch (const GestureType aniGestureType, Touch* touch);

    bool isAnimationIndexForNormal (const int animationIndex);

    void onDraging (l2dDragAnimation* aniDrag, Touch * touch);

    void onDragEnd (l2dDragAnimation* aniDrag, Touch * touch);

    l2dGestureAnimation* getGestureAnimationById (const int id);

private:
    bool                        m_bDraging;
    int                         m_curAniIndex;
    std::vector<int>            m_targetDragAniIds;
    AnimationTimes              m_times;
    Mapl2dGestureAnimation      m_mapl2dGestureAnimation;
    EventListener*              m_touchListener;
};
#endif // !__LIVE2DUSERCTRL_H__
