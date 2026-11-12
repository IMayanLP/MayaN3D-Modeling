
#include "RayCastTexture.h"

RayCastTexture::~RayCastTexture() {
    Destroy();
}

bool RayCastTexture::Initialize(int width, int height) {
    if (width <= 0 || height <= 0) return false;

    this->width = width;
    this->height = height;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture != 0;
}

void RayCastTexture::Update(const std::vector<unsigned char>& pixels) {
    if (texture == 0 || pixels.size() != static_cast<size_t>(width * height * 3)) return;

    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool RayCastTexture::Resize(int width, int height) {
    if (width <= 0 || height <= 0 || texture == 0) return false;

    if (this->width == width && this->height == height) return true;

    this->width = width;
    this->height = height;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void RayCastTexture::Destroy() {
    if (texture != 0) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
}

GLuint RayCastTexture::GetTexture() const {
    return texture;
}

int RayCastTexture::GetWidth() const {
    return width;
}

int RayCastTexture::GetHeight() const {
    return height;
}
