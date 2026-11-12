#pragma once

#include <vector>
#include "../../Graphics/Camera.h"
#include "../CSG/CSG.h"
#include "../Object/Object.h"

class RayCast {
    public:
        std::vector<unsigned char> Render(const std::vector<Object>&, const Camera&, int, int, glm::vec3, bool, const Ray&) const;
};
