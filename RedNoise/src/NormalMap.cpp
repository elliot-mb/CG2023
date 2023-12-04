//
// Created by elliot on 04/12/23.
//

#include "NormalMap.h"
#include "TextureMap.h"
NormalMap::NormalMap() = default;
NormalMap::NormalMap(std::string fileName) {
    TextureMap t = TextureMap(fileName);
    for(int y = 0; y < t.height; y++){
        this->norms.push_back({});
        for(int x = 0; x < t.width; x++){
            uint32_t px = t.pixel(x, y);
            Colour temp = Utils::unpack(px);
            glm::vec3 norm = glm::normalize(glm::vec3{temp.red, temp.green, temp.blue});
            this->norms[y].push_back(norm);
        }
    }

}

glm::vec3 &NormalMap::norm(int x, int y) {
    return this->norms[y][x];
}

size_t NormalMap::getWidth() const {
    return this->t.width;
}

size_t NormalMap::getHeight() const {
    return this->t.height;
}
