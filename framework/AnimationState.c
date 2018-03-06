/*
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at http://live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */


#include "live2d/include/Live2DCubismFramework.h"


// -------- //
// REQUIRES //
// -------- //

#include "Local.h"


// -------------- //
// IMPLEMENTATION //
// -------------- //

void csmInitializeAnimationState (csmAnimationState* state, float duration) {
    // Validate argument.
    Ensure (state, "\"state\" is invalid.", return);

    state->Duration = duration;

    csmResetAnimationState (state);
}

void csmResetAnimationState (csmAnimationState* state) {
    // Validate argument.
    Ensure (state, "\"state\" is invalid.", return);

    state->Time = 0.0f;

    state->bLoop = 1;

    state->bPlayEnable = 1;

    state->TimeAniStart = 0.f;

    state->TimeAniEnd = state->Duration;
}

int  csmSetTimeAniStart (csmAnimationState* state, float time) {
    if (time < 0.f) {
        return 0;
    }

    state->TimeAniStart = time;

    if (state->Time < state->TimeAniStart) {
        state->Time = state->TimeAniStart;
    }

    return 1;
}

int  csmSetTimeAniEnd (csmAnimationState* state, float time) {
    if (time > state->Duration) {
        return 0;
    }

    state->TimeAniEnd = time;

    if (state->Time > state->TimeAniEnd) {
        state->Time = state->TimeAniEnd;
    }
}

void csmSetAniEnabled (csmAnimationState* state, int bEnabled) {
    state->bPlayEnable = bEnabled;
}

void csmSetLoopEnabled (csmAnimationState* state, int bLoop) {
    state->bLoop = bLoop;
}

int csmSetCurrentAnimationTime (csmAnimationState* state, float time) {
    if (time >= 0.f && time <= state->Duration) {
        state->Time = time;
        return 1;
    }

    return 0;
}

int csmUpdateAnimationState (csmAnimationState* state, const float timeDelta) {
    // Validate argument.
    Ensure (state, "\"state\" is invalid.", return);

    static const int bPlayEndToCallFunc = 1;

    if (state->bPlayEnable == 0) {
        return !bPlayEndToCallFunc;
    }

    if (state->Time < state->TimeAniStart) {
        state->Time = state->TimeAniStart;
        return !bPlayEndToCallFunc;
    }

    if (state->Time > state->TimeAniEnd) {
        if (state->bLoop) {
            state->Time = state->TimeAniStart;
        } else {
            state->Time = state->TimeAniEnd;
        }

        return !bPlayEndToCallFunc;
    }

    if (state->Time == state->TimeAniEnd) {
        if (state->bLoop) {
            state->Time = state->TimeAniStart + timeDelta;
        }

        return !bPlayEndToCallFunc;
    }

    state->Time += timeDelta;

    if (state->Time < state->TimeAniEnd) {
        return !bPlayEndToCallFunc;
    }

    state->Time = state->TimeAniEnd;
    return bPlayEndToCallFunc;
}
