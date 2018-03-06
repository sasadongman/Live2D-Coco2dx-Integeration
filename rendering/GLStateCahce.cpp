#include "Local.h"
#include "renderer\ccGLStateCache.h"

void glCache_useProgram (GLuint program) {
    cocos2d::GL::useProgram (program);
}

void glCache_deleteProgram (GLuint program) {
    cocos2d::GL::deleteProgram (program);
}

void glCache_bindTexture2D (GLuint textureId) {
    cocos2d::GL::bindTexture2D (textureId);
}

void glCache_bindTexture2DN (GLuint textureUnit, GLuint textureId) {
    cocos2d::GL::bindTexture2DN (textureUnit, textureId);
}

void glCache_bindTextureN (GLuint textureUnit, GLuint textureId, GLuint textureType) {
    cocos2d::GL::bindTextureN (textureId, textureType);
}

void glCache_deleteTexture (GLuint textureId) {
    cocos2d::GL::deleteTexture (textureId);
}

void glCache_activeTexture (GLenum texture) {
    cocos2d::GL::activeTexture (texture);
}

void glCache_enableVertexAttribs (GLuint flags) {
    cocos2d::GL::enableVertexAttribs (flags);
}

void glCache_blendResetToCache() {
	cocos2d::GL::blendResetToCache();
}