#include <algorithm>
#include <sstream>
#include "Utils.h"
#include <vector>
#include "glm/glm.hpp"
#include <fstream>
#include <iostream>

float Utils::max(float a, float b){
	if (a >= b) return a;
	return b;
}

float Utils::min(float a, float b){
	if (a <= b) return a;
	return b;
}

std::vector<std::string> Utils::split(const std::string &line, char delimiter) {
	auto haystack = line;
	std::vector<std::string> tokens;
	size_t pos;
	while ((pos = haystack.find(delimiter)) != std::string::npos) {
		tokens.push_back(haystack.substr(0, pos));
		haystack.erase(0, pos + 1);
	}
	// Push the remaining chars onto the vector
	tokens.push_back(haystack);
	return tokens;
}

std::vector<float> Utils::interpolateSingleFloats(float from, float to, int numberOfValues){
	if(numberOfValues < 1) throw std::invalid_argument("interpolateSingleFloats must output at least one value");
    if(numberOfValues == 1) return std::vector<float>{to};
	float delta = to - from;
	float step = delta / static_cast<float>((numberOfValues - 1));
    std::vector<float> result = {};
	for(int i = 0; i < numberOfValues; i++) {
		result.push_back(from + (static_cast<float>(i) * step));
	}
	return result;
}

 //throws if too few steps are requested
 std::vector<glm::vec2> Utils::interpolateTwoElementValues(glm::vec2 from, glm::vec2 to, int steps){
 	if(steps < 1) throw std::invalid_argument("interpolateTwoElementValues must output at least two values");
     std::vector<float> xs = Utils::interpolateSingleFloats(from.x, to.x, steps);
     std::vector<float> ys = Utils::interpolateSingleFloats(from.y, to.y, steps);
     std::vector<glm::vec2> result = {};
     for(size_t i = 0; i < xs.size(); i++){
         result.push_back(glm::vec2(xs[i], ys[i]));
     }

     return result;
 }

std::vector<glm::vec3> Utils::interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues){
	if(numberOfValues < 2) throw std::invalid_argument("interpolateThreeElementValues must output at least two values");
	std::vector<float> xs = Utils::interpolateSingleFloats(from.x, to.x, numberOfValues);
    std::vector<float> ys = Utils::interpolateSingleFloats(from.y, to.y, numberOfValues);
    std::vector<float> zs = Utils::interpolateSingleFloats(from.z, to.z, numberOfValues);
    std::vector<glm::vec3> result = {};
	for(int i = 0; i < numberOfValues; i++){
		result.push_back(glm::vec3(xs[i], ys[i], zs[i]));
	}
	return result;
}

uint32_t Utils::pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	return (a << 24) + (r << 16) + (g << 8) + b;
}

std::string Utils::fileAsString(std::string& filename){
    std::ifstream inputStream(filename, std::ifstream::binary);
    if(!inputStream.good()) throw std::invalid_argument("Utils::fileAsString(): file '"+ filename + "' not found"); //invalid_argument("ModelLoader::load(): model file not found");
    char c = 'c'; //get value
    std::string bytes;
    while(c != EOF){
        c = static_cast<char>(inputStream.get() % 255);
        if(c != EOF) { bytes += c; }
    }
    return bytes;
}

glm::mat3 Utils::pitch(float angle){
    if(angle < 0) angle += M_PI_2f * 4;
    float cTx = glm::cos(angle + (M_PI_2f * 4));
    float sTx = glm::sin(angle + (M_PI_2f * 4));
    return glm::mat3({1,   0,    0},
                     {0, cTx, -sTx},
                     {0, sTx,  cTx});
}

glm::mat3 Utils::yaw(float angle){
    if(angle < 0) angle += M_PI_2f * 4;
    float cTy = glm::cos(angle + (M_PI_2f * 4));
    float sTy = glm::sin(angle + (M_PI_2f * 4));
    return glm::mat3({cTy,  0, sTy},
                     {0,    1,   0},
                     {-sTy, 0, cTy});
}

//if i give no up, they get to assume that my up is that of the world origin (see default argument in defn.)
// noRoll flag tells us if we want to allow roll in the resulting transformation (do we keep the x axis level)
glm::mat3 Utils::rotateMeTo(glm::vec3 direction, glm::vec3 myUp, bool canRoll){
    glm::vec3 dirNorm = glm::normalize(direction);
    glm::mat3 orientation = glm::mat3({0, 0, 0},
                                      {0, 0, 0},
                                      {0, 0, 0});
    glm::vec3 xDir = glm::cross(myUp, dirNorm);
//    if(!canRoll) {
//      //project the vector onto the plane and maintain length (y component zero)
//      float mag = glm::length(xDir);
//      xDir.y = 0;
//      float shortMag = glm::length(xDir);
//      xDir = (mag / shortMag) * xDir; //scale it up again
//    }
    xDir = glm::normalize(xDir);
    glm::vec3 yDir = glm::cross(dirNorm, xDir);
    yDir = glm::normalize(yDir);
    // then we try to roll it back if we're not allowed to roll
    if(!canRoll) {
        //rotate by theta (angle the x axis is away from the origin xz plane) around the direction axis

        //generate some vector in the same direction as x but flat to the plane
        float mag = glm::length(xDir);
        glm::vec3 xFlatDir = {xDir.x, 0.0, xDir.z};
        float shortMag = glm::length(xFlatDir);
        xFlatDir = xFlatDir * shortMag;
        //calculate how rotated around our z axis we are
        float theta = glm::asin(glm::length(glm::cross(xDir, xFlatDir)) / (mag * shortMag));
        std::cout << theta << std::endl;
//      //project the vector onto the plane and maintain length (y component zero)

    }
    //column 1
    orientation[0].x = xDir.x;
    orientation[0].y = yDir.x;
    orientation[0].z = dirNorm.x;
    //column 2
    orientation[1].x = xDir.y;
    orientation[1].y = yDir.y;
    orientation[1].z = dirNorm.y;
    //column 3
    orientation[2].x = xDir.z;
    orientation[2].y = yDir.z;
    orientation[2].z = dirNorm.z;

    return orientation;
}
