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

    if (node->type == CSGNodeType::Boolean) {
        return ClassifyBoolean(node, ray);
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

std::vector<SMCInterval> SMC::ClassifyBoolean(const CSGNode* node, const Ray& ray) {
    if (node == nullptr || node->left == nullptr || node->right == nullptr) return {};

    std::vector<SMCInterval> intervalsA = ClassifyNode(node->left.get(), ray);
    std::vector<SMCInterval> intervalsB = ClassifyNode(node->right.get(), ray);

    std::vector<SMCInterval> normalizedA = NormalizeIntervals(intervalsA, intervalsB);
    std::vector<SMCInterval> normalizedB = NormalizeIntervals(intervalsB, intervalsA);

    std::vector<SMCInterval> result;

    if (node->booleanOperation == BooleanOperation::Union) {
        for (std::size_t i = 0; i < normalizedA.size(); ++i) {
            SMCInterval interval = normalizedA[i];

            if (normalizedA[i].classification == SMCClassification::IN || normalizedB[i].classification == SMCClassification::IN) {
                interval.classification = SMCClassification::IN;
            } else interval.classification = SMCClassification::OUT;

            result.push_back(interval);
        }
    }

    else if (node->booleanOperation == BooleanOperation::Intersection) {
        for (std::size_t i = 0; i < normalizedA.size(); ++i) {
            SMCInterval interval = normalizedA[i];

            if (normalizedA[i].classification == SMCClassification::IN && normalizedB[i].classification == SMCClassification::IN) {
                interval.classification = SMCClassification::IN;
            } else interval.classification = SMCClassification::OUT;

            result.push_back(interval);
        }
    }

    else if (node->booleanOperation == BooleanOperation::Difference) {
        for (std::size_t i = 0; i < normalizedA.size(); ++i) {
            SMCInterval interval = normalizedA[i];

            if (normalizedA[i].classification == SMCClassification::IN && normalizedB[i].classification == SMCClassification::OUT) {
                interval.classification = SMCClassification::IN;
            } else interval.classification = SMCClassification::OUT;

            result.push_back(interval);
        }
    }

    result = MergeIntervals(result);

    // invertendo normais vindas de B na diferença
    if (node->booleanOperation == BooleanOperation::Difference) {
        const float e = 1e-5f;

        for (SMCInterval& interval : result) {
            for (const SMCInterval& intervalB : intervalsB) {
                if (std::abs(interval.start.intersection.t - intervalB.start.intersection.t) < e) {
                    interval.start = intervalB.start;
                    interval.start.intersection.normal = -interval.start.intersection.normal;
                    break;
                }
            }

            for (const SMCInterval& intervalB : intervalsB) {
                if (std::abs(interval.end.intersection.t - intervalB.start.intersection.t) < e) {
                    interval.end = intervalB.start;
                    interval.end.intersection.normal = -interval.end.intersection.normal;
                    break;
                }
            }
        }
    }

    return result;
}

std::vector<SMCInterval> SMC::NormalizeIntervals(const std::vector<SMCInterval>& A, const std::vector<SMCInterval>& B) {
    std::vector<SMCIntersection> boundaries;
    std::size_t indexA = 0;
    std::size_t indexB = 0;

    while (indexA < A.size() || indexB < B.size()) {
        if (indexA >= A.size()) boundaries.push_back(B[indexB++].start);
        else if (indexB >= B.size()) boundaries.push_back(A[indexA++].start);
        else {
            float tA = A[indexA].start.intersection.t;
            float tB = B[indexB].start.intersection.t;

            if (tA < tB) boundaries.push_back(A[indexA++].start);
            else if (tB < tA) boundaries.push_back(B[indexB++].start);
            else {
                boundaries.push_back(A[indexA].start);
                ++indexA;
                ++indexB;
            }
        }
    }

    std::vector<SMCInterval> result;
    std::size_t intervalAIndex = 0;

    for (std::size_t i = 0; i < boundaries.size(); ++i) {
        const SMCIntersection& start = boundaries[i];

        SMCIntersection end;

        if (i + 1 < boundaries.size()) end = boundaries[i + 1];
        else {
            end = A.back().end;

            if (B.back().end.intersection.t < end.intersection.t) end = B.back().end;
        }

        while (intervalAIndex + 1 < A.size() && A[intervalAIndex].end.intersection.t <= start.intersection.t) ++intervalAIndex;

        result.push_back({start, end, A[intervalAIndex].classification});
    }

    return result;
}

std::vector<SMCInterval> SMC::MergeIntervals(const std::vector<SMCInterval>& intervals) {
    if (intervals.empty()) return {};

    std::vector<SMCInterval> result;
    result.push_back(intervals[0]);

    for (std::size_t i = 1; i < intervals.size(); ++i) {
        SMCInterval& previous = result.back();
        const SMCInterval& current = intervals[i];

        if (previous.classification == current.classification) previous.end = current.end;
        else result.push_back(current);
    }

    return result;
}