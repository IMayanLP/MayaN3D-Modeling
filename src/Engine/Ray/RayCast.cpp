#include "RayCast.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Ray.h"
#include "SMC.h"

std::vector<unsigned char> RayCast::Render(const std::vector<Object>& objects, const Camera& camera, int width, int height, glm::vec3 baseColor, bool useDebugRay, const Ray& debugRay) const {
    if (width <= 0 || height <= 0) return {};

    std::vector<unsigned char> pixels(width * height * 3);

    for (std::size_t i = 0; i < pixels.size(); i += 3) {
        pixels[i] = 77;
        pixels[i + 1] = 77;
        pixels[i + 2] = 77;
    }

    glm::mat4 inverseView = glm::inverse(camera.GetViewMatrix());

    glm::vec3 cameraPosition = camera.GetPosition();
    glm::vec3 forward = glm::normalize(glm::vec3(inverseView * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
    glm::vec3 right = glm::normalize(glm::vec3(inverseView * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::vec3(inverseView * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    const float scale = 0.41421356f; // para 45.0f graus de FOV igual a camera padrao e é isso

    glm::vec3 lightDirection = glm::normalize(glm::vec3(-1.0f, 2.0f, 1.0f));

    // Raios de debug
    std::vector<SMCInterval> intervalosDebug;

    if (useDebugRay) {
        for (const Object& obj : objects) {
            if (obj.type != ObjectType::CSG) continue;

            std::vector<SMCInterval> intervals = SMC::Classify(obj.csg, debugRay);

            for (SMCInterval interval : intervals) {
                intervalosDebug.push_back(interval);
            }
        }

        if (!intervalosDebug.empty()) {
            if (intervalosDebug.size() == 1) {
                intervalosDebug.back().end.intersection.t = intervalosDebug.back().start.intersection.t + 10.0f;
            } else if (!std::isfinite(intervalosDebug.back().end.intersection.t)) {
                intervalosDebug.back().end.intersection.t = intervalosDebug.back().start.intersection.t + 1.0f;
            }
        }

        std::cout << "Debug Intervals (" << intervalosDebug.size() << "):" << std::endl;

        for (const SMCInterval& interval : intervalosDebug) {
            std::string intervalClass = interval.classification == SMCClassification::IN ? "IN" : "OUT";

            std::cout << "Start: " << interval.start.intersection.t
                      << " | End: " << interval.end.intersection.t
                      << " | Classification: " << intervalClass << std::endl;
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float posX = (2.0f * (x + 0.5f) / width) - 1.0f;
            float posY = 1.0f - (2.0f * (y + 0.5f) / height);

            glm::vec3 direction = glm::normalize(forward + right * (posX * aspectRatio * scale) + up * (posY * scale));
            Ray ray(cameraPosition, direction);

            // Procurar a interseção mais próxima entre todos os objetos
            RayIntersection closestIntersection;
            float nearestT = std::numeric_limits<float>::infinity();
            bool hit = false;

            for (const Object& obj : objects) {
                if (obj.type != ObjectType::CSG) continue;

                std::vector<RayIntersection> intersectionsInBoundingBox =
                    SMC::IntersectRayInCell(ray, obj.octree.GetCell());

                if (intersectionsInBoundingBox.empty()) continue;

                std::vector<SMCInterval> intervals = SMC::Classify(obj.csg, ray);

                for (const SMCInterval& interval : intervals) {
                    if (interval.classification != SMCClassification::IN) continue;

                    const RayIntersection& intersection = interval.start.intersection;

                    if (intersection.t <= 1e-4f || !std::isfinite(intersection.t) || intersection.t >= nearestT || glm::length(intersection.normal) < 1e-6f) {
                        continue;
                    }

                    nearestT = intersection.t;
                    closestIntersection = intersection;
                    hit = true;
                }
            }

            float tDebugRay = 0.0f;
            bool rayIntersectsDebugRay = false;
            if (useDebugRay) rayIntersectsDebugRay = Ray::RaysIntersect(ray, debugRay, 0.01f, tDebugRay);

            std::size_t pixelIndex = (y * width + x) * 3;

            // Pixel sem interseção
            if (!hit) {
                if (useDebugRay && rayIntersectsDebugRay) {
                    for (const SMCInterval& interval : intervalosDebug) {
                        float fim = std::isfinite(interval.end.intersection.t)
                            ? interval.end.intersection.t
                            : interval.start.intersection.t + 1.0f;

                        if (tDebugRay >= interval.start.intersection.t && tDebugRay <= fim) {
                            if (interval.classification == SMCClassification::ON ||
                                interval.classification == SMCClassification::IN) {
                                pixels[pixelIndex] = 0;
                                pixels[pixelIndex + 1] = 255;
                                pixels[pixelIndex + 2] = 0;
                            } else {
                                pixels[pixelIndex] = 255;
                                pixels[pixelIndex + 1] = 0;
                                pixels[pixelIndex + 2] = 0;
                            }
                        }
                    }
                }

                continue;
            }

            // Iluminação
            glm::vec3 normal = glm::normalize(closestIntersection.normal);

            float diffuse = std::max(glm::dot(normal, lightDirection), 0.0f);
            float intensity = 0.15f + 0.85f * diffuse;

            pixels[pixelIndex] = 255.0f * baseColor.r * intensity;
            pixels[pixelIndex + 1] = 255.0f * baseColor.g * intensity;
            pixels[pixelIndex + 2] = 255.0f * baseColor.b * intensity;

            // Sobreposição do raio de debug
            if (useDebugRay && rayIntersectsDebugRay) {
                for (const SMCInterval& interval : intervalosDebug) {
                    float fim = std::isfinite(interval.end.intersection.t)
                        ? interval.end.intersection.t
                        : interval.start.intersection.t + 1.0f;

                    if (tDebugRay >= interval.start.intersection.t && tDebugRay <= fim) {
                        if (interval.classification == SMCClassification::ON ||
                            interval.classification == SMCClassification::IN) {
                            pixels[pixelIndex] = 0;
                            pixels[pixelIndex + 1] = 255;
                            pixels[pixelIndex + 2] = 0;
                        } else {
                            pixels[pixelIndex] = 255;
                            pixels[pixelIndex + 1] = 0;
                            pixels[pixelIndex + 2] = 0;
                        }
                    }
                }
            }
        }
    }

    return pixels;
}
