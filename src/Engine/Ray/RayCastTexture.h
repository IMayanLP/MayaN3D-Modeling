
#pragma once

#include <glad/glad.h>
#include <vector>

class RayCastTexture {
    public:
        RayCastTexture() = default;
        ~RayCastTexture();

        bool Initialize(int, int);
        void Update(const std::vector<unsigned char>&);
        bool Resize(int, int);
        void Destroy();

        GLuint GetTexture() const;
        int GetWidth() const;
        int GetHeight() const;

    private:
        GLuint texture = 0;
        int width = 320;
        int height = 180;
};
