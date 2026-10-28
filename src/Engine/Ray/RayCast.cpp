#include "RayCast.h"

#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Ray.h"
#include "SMC.h"

std::vector<unsigned char> RayCast::Render(const CSG& csg, const Camera& camera, int width, int height, glm::vec3 baseColor) const {
    if (width <= 0 || height <= 0) return {};

    std::vector<unsigned char> pixels(width * height * 3);

    for (std::size_t i = 0; i < pixels.size(); i += 3) {
        pixels[i] = 70;
        pixels[i + 1] = 70;
        pixels[i + 2] = 80;
    }

    glm::mat4 inverseView = glm::inverse(camera.GetViewMatrix());

    glm::vec3 cameraPosition = camera.GetPosition();
    glm::vec3 forward = glm::normalize(glm::vec3(inverseView * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    glm::vec3 right = glm::normalize(glm::vec3(inverseView * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::vec3(inverseView * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    const float scale = 0.41421356f; // para 45.0f graus de FOV igual a camera padrao e é isso

    glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, 2.0f, 1.0f));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float posX = (2.0f * (x + 0.5f) / width) - 1.0f;
            float posY = 1.0f - (2.0f * (y + 0.5f) / height);

            glm::vec3 direction = glm::normalize(forward + right * (posX * aspectRatio * scale) + up * (posY * scale));

            Ray ray(cameraPosition, direction);
            std::vector<SMCInterval> intervals = SMC::Classify(csg, ray);

            const RayIntersection* hit = nullptr;
            float nearestT = std::numeric_limits<float>::infinity();

            for (const SMCInterval& interval : intervals) {
                const RayIntersection& intersection = interval.start.intersection;

                if (intersection.t <= 1e-4f || !std::isfinite(intersection.t) || intersection.t >= nearestT || glm::length(intersection.normal) < 1e-6f) {
                    continue;
                }

                nearestT = intersection.t;
                hit = &interval.start.intersection;
            }

            if (hit == nullptr) continue;

            glm::vec3 normal = glm::normalize(hit->normal);

            float diffuse = std::max(glm::dot(normal, lightDirection), 0.0f);
            float intensity = 0.15f + 0.85f * diffuse;

            std::size_t pixelIndex = (y * width + x) * 3;

            pixels[pixelIndex] = 255.0f * baseColor.r * intensity;
            pixels[pixelIndex + 1] = 255.0f * baseColor.g * intensity;
            pixels[pixelIndex + 2] = 255.0f * baseColor.b * intensity;
        }
    }

    return pixels;
}
