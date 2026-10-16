#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
    public:
        Shader(const char*, const char*);
        void Use() const;
        void SetMat4(const std::string&, const glm::mat4&) const;
        void SetVec3(const std::string&, const glm::vec3&) const;

    private:
        unsigned int ID;

        static std::string LoadSource(const char*);
        static unsigned int CompileShader(unsigned int, const std::string&);
};