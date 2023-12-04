//
// Created by elliot on 02/12/23.
//

#include "EnvMap.h"

EnvMap::EnvMap(std::string fileName) {
    this->t = TextureMap(fileName);
    this->width = static_cast<int>(this->t.width);
    this->height = static_cast<int>(this->t.height);
}

// lat and long must be [-PI/2, PI/2], [0, 2PI] respectively
uint32_t EnvMap::getPixel(float lat, float lng) {
    if(lng > M_PI * 2) lng -= M_PI * 2;
    if(lng < 0) lng += M_PI * 2;
    if(lat > M_PI_2) lat = M_PI_2;
    if(lat < -M_PI_2) lat = -M_PI_2;
    //if(lat > M_PI / 2 || lat < M_PI / -2) throw std::runtime_error("EnvMap::getPixel: latitude is not in the proper interval [-PI/2, PI/2]");
    float x = ((lng - EnvMap::STD_LONG) / static_cast<float>(M_PI * 2)) * EnvMap::COS_STD_LAT * static_cast<float>(this->width);
    float y = (((glm::sin(lat) * EnvMap::SEC_STD_LAT) + 1) / 2 * static_cast<float>(this->height));
    return t.pixel(static_cast<int>(glm::floor(x)), static_cast<int>(glm::floor(y)));
}
