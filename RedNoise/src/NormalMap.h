//
// Created by elliot on 04/12/23.
//

#pragma once

#include <string>
#include <vector>
#include "glm/vec3.hpp"
#include "TextureMap.h"

class NormalMap {
public:
    NormalMap();
    explicit NormalMap(std::string fileName);
    glm::vec3& getNormal(int x, int y);
    size_t getWidth() const;
    size_t getHeight() const;
private:
    TextureMap texture;
    std::vector<std::vector<glm::vec3>> norms;
};
