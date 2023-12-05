//
// Created by elliot on 04/12/23.
//

#include "NormalMap.h"
#include "TextureMap.h"
NormalMap::NormalMap() = default;
NormalMap::NormalMap(std::string fileName) {
    this->texture = TextureMap(fileName);
    for(int y = 0; y < this->texture.height; y++){
        this->norms.push_back({});
        for(int x = 0; x < this->texture.width; x++){
            uint32_t px = this->texture.pixel(x, y);
            Colour temp = Utils::unpack(px);
            glm::vec3 norm = glm::normalize(glm::vec3({temp.red - 128, temp.green - 128, temp.blue - 128}));
            this->norms[y].push_back(norm);
        }
    }

}

glm::vec3 &NormalMap::getNormal(int x, int y) {
    return this->norms[y][x];
}

size_t NormalMap::getWidth() const {
    return this->texture.width;
}

size_t NormalMap::getHeight() const {
    return this->texture.height;
}
