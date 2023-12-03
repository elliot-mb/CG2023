//
// Created by elliot on 02/12/23.
//

#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "TextureMap.h"

//wrapper class for TextureMap which has a helper method to convert from latitude and longitude to pixel coordinates
//sec(0) = 1 for our standard latitude of 0
class EnvMap {
public:
    explicit EnvMap(std::string fileName);

    //gets a pixel from lattitude and longitude by cylindrical equal-area projection formulae
    uint32_t getPixel(float lat, float lng);

private:
    constexpr static const float SEC_STD_LAT = 1.0;
    constexpr static const float COS_STD_LAT = 1.0;
    constexpr static const float STD_LONG = M_PI;

    int width;
    int height;
    TextureMap t;
};
