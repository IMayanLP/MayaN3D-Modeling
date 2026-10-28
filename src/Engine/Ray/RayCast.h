#pragma once

#include <vector>
#include "../../Graphics/Camera.h"
#include "../CSG/CSG.h"

class RayCast {
    public:
        std::vector<unsigned char> Render(const CSG&, const Camera&, int, int, glm::vec3 = glm::vec3(1.0f)) const;
};
