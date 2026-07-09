#include "engine/Transform.h"

glm::mat4 Transform::getModelMatrix() const
{
    glm::mat4 model{1.0f};
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);

    return model;
}

glm::vec3 Transform::getForward() const
{
    float rad = glm::radians(rotation.y);
    return glm::vec3(-glm::sin(rad), 0.0f, -glm::cos(rad));
}

glm::vec3 Transform::getRight() const
{
    float rad = glm::radians(rotation.y);
    return glm::vec3(glm::cos(rad), 0.0f, -glm::sin(rad));
}
