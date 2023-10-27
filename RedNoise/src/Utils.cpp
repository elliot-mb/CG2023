#include <algorithm>
#include <sstream>
#include "Utils.h"
#include <vector>
#include "glm/glm.hpp"
#include <fstream>

float Utils::max(float a, float b){
	if (a >= b) return a;
	return b;
}

float Utils::min(float a, float b){
	if (a <= b) return a;
	return b;
}


//accidentally put this in my utils name space
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

glm::mat3 Utils::rotateX(float angle){
    if(angle < 0) angle += M_PI_2f * 4;
    float cTx = glm::cos(angle + (M_PI_2f * 4));
    float sTx = glm::sin(angle + (M_PI_2f * 4));
    return glm::mat3({1,   0,    0},
                     {0, cTx, -sTx},
                     {0, sTx,  cTx});
}

glm::mat3 Utils::rotateY(float angle){
    if(angle < 0) angle += M_PI_2f * 4;
    float cTy = glm::cos(angle + (M_PI_2f * 4));
    float sTy = glm::sin(angle + (M_PI_2f * 4));
    return glm::mat3({cTy,  0, sTy},
                     {0,    1,   0},
                     {-sTy, 0, cTy});
}

//if i give no up, they get to assume that my up is that of the world origin (see default argument in defn.)
glm::mat3 Utils::rotateMeTo(glm::vec3 direction, glm::vec3 myUp ){
    glm::mat3 transform = glm::mat3({0, 0, 0},
                                    {0, 0, 0},
                                    {0, 0, 0});
    glm::vec3 xDir = glm::cross(myUp, direction);
    xDir = glm::normalize(xDir);
    glm::vec3 yDir = glm::cross(direction, xDir);
    yDir = glm::normalize(yDir);
    //column 1
    transform[0].x = xDir.x;
    transform[0].y = yDir.x;
    transform[0].z = direction.x;
    //column 2
    transform[1].x = xDir.y;
    transform[1].y = yDir.y;
    transform[1].z = direction.y;
    //column 3
    transform[2].x = xDir.z;
    transform[2].y = yDir.z;
    transform[2].z = direction.z;

    return transform;
}
