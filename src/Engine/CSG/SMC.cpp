#include "SMC.h"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

std::vector<SMCInterval> SMC::Classify(const CSG& csg, const Ray& ray) {
    return ClassifyNode(csg.GetRoot(), ray);
}

std::vector<SMCInterval> SMC::ClassifyNode(const CSGNode* node, const Ray& ray) {
    if (node == nullptr) return {};

    if (node->type == CSGNodeType::Primitive) {
        return ClassifyPrimitive(node, ray);
    }

    if (node->type == CSGNodeType::Transform) {
        return ClassifyTransform(node, ray);
    }

    return {};
}

std::vector<SMCInterval> SMC::ClassifyPrimitive(const CSGNode* node, const Ray& ray) {
    std::vector<SMCInterval> intervals;

    if (node == nullptr) return intervals;
    if (node->type != CSGNodeType::Primitive || node->primitive == nullptr) return intervals;

    std::vector<SMCIntersection> intersections;
    std::vector<RayIntersection> rayIntersections = node->primitive->IntersectRay(ray);
    
    float e = 1e-4;
    bool intersectOrigin = !rayIntersections.empty() && rayIntersections[0].t <= e;

    for (const RayIntersection& intersection : rayIntersections) {
        intersections.push_back({intersection, SMCClassification::ON});
    }

    
    SMCClassification originClassification = node->primitive->CheckIsInside(ray.GetOrigin());

    if (!intersectOrigin) {
        RayIntersection originIntersection = {0.0f, ray.GetOrigin(), glm::vec3(0.0f)};
        intersections.insert(intersections.begin(), {originIntersection, originClassification});
    }

    
    SMCClassification currentClassification = originClassification;

    if (originClassification == SMCClassification::ON) {
        currentClassification = intersectOrigin && rayIntersections.size() > 1 ? SMCClassification::IN : SMCClassification::OUT;
    }


    for (size_t i = 0; i < intersections.size(); i++) {
        SMCIntersection start = intersections[i];
        SMCIntersection end{};

        if (i + 1 < intersections.size()) {
            end = intersections[i + 1];
        } else {
            end.intersection.t = std::numeric_limits<float>::infinity();
            end.intersection.point = glm::vec3(0.0f);
            end.intersection.normal = glm::vec3(0.0f);
            end.classification = SMCClassification::OUT;
        }

        intervals.push_back({start, end, currentClassification});

        if (i + 1 < intersections.size()) {
            currentClassification = currentClassification == SMCClassification::IN ? SMCClassification::OUT : SMCClassification::IN;
        }
    }

    return intervals;
}

std::vector<SMCInterval> SMC::ClassifyTransform(const CSGNode* node, const Ray& ray) {
    if (node == nullptr || node->left == nullptr) return {};

    if (node->transformType == TransformType::Translation) {
        glm::vec3 localOrigin = ray.GetOrigin() - node->transformValue;

        Ray localRay(localOrigin, ray.GetDirection());

        std::vector<SMCInterval> intervals = ClassifyNode(node->left.get(), localRay);

        for (SMCInterval& interval : intervals) {
            interval.start.intersection.point += node->transformValue;

            if (std::isfinite(interval.end.intersection.t)) {
                interval.end.intersection.point += node->transformValue;
            }
        }

        return intervals;
    }

    if (node->transformType == TransformType::Scale) {
        glm::vec3 scale = node->transformValue;

        if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f) {
            return {};
        }

        glm::vec3 localOrigin = ray.GetOrigin() / scale;
        glm::vec3 localDirection = ray.GetDirection() / scale;

        Ray localRay(localOrigin, localDirection);

        std::vector<SMCInterval> intervals = ClassifyNode(node->left.get(), localRay);

        for (SMCInterval& interval : intervals) {
            interval.start.intersection.point *= scale;

            if (std::isfinite(interval.start.intersection.t)) {
                interval.start.intersection.t = glm::dot(interval.start.intersection.point - ray.GetOrigin(), ray.GetDirection());
            }

            interval.start.intersection.normal = glm::normalize(
                interval.start.intersection.normal / scale
            );

            if (std::isfinite(interval.end.intersection.t)) {
                interval.end.intersection.point *= scale;
                interval.end.intersection.t = glm::dot(interval.end.intersection.point - ray.GetOrigin(), ray.GetDirection());
                interval.end.intersection.normal = glm::normalize(interval.end.intersection.normal / scale);
            }
        }

        std::sort(intervals.begin(), intervals.end(), [](const SMCInterval& a, const SMCInterval& b) {
            return a.start.intersection.t < b.start.intersection.t;
        });

        return intervals;
    }

    if (node->transformType == TransformType::Rotation) {
        glm::vec3 values = node->transformValue;

        glm::mat4 rotation(1.0f);
        rotation = glm::rotate(rotation, values.x, glm::vec3(1.0f, 0.0f, 0.0f));
        rotation = glm::rotate(rotation, values.y, glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = glm::rotate(rotation, values.z, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 inverseRotation = glm::transpose(rotation);

        glm::vec3 localOrigin = glm::vec3(inverseRotation * glm::vec4(ray.GetOrigin(), 1.0f));
        glm::vec3 localDirection = glm::vec3(inverseRotation * glm::vec4(ray.GetDirection(), 0.0f));

        Ray localRay(localOrigin, localDirection);

        std::vector<SMCInterval> intervals = ClassifyNode(node->left.get(), localRay);

        for (SMCInterval& interval : intervals) {
            interval.start.intersection.point = glm::vec3(rotation * glm::vec4(interval.start.intersection.point, 1.0f));
            interval.start.intersection.normal = glm::normalize(glm::vec3(rotation * glm::vec4(interval.start.intersection.normal, 0.0f))            );

            if (std::isfinite(interval.end.intersection.t)) {
                interval.end.intersection.point = glm::vec3(rotation * glm::vec4(interval.end.intersection.point, 1.0f));
                interval.end.intersection.normal = glm::normalize(glm::vec3(rotation * glm::vec4(interval.end.intersection.normal, 0.0f))                );
            }
        }

        return intervals;
    }

    return {};
}