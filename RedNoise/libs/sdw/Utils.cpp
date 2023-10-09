#include <algorithm>
#include <sstream>
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

float Utils::min(float a, float b){
	if (a >= b) return a;
	return b;
}

float Utils::max(float a, float b){
	if (a <= b) return a;
	return b;
}


//accidentally put this in my utils name space
vector<std::string> Utils::split(const std::string &line, char delimiter) {
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
	if(numberOfValues < 2) throw invalid_argument("interpolateSingleFloats must output at least two values");
	float delta = to - from;
	float step = delta / (numberOfValues - 1);
	vector<float> result = {};
	for(int i = 0; i < numberOfValues; i++) {
		result.push_back(from + (i * step));
	}
	return result;
}

// //throws if too few steps are requested 
// std::vector<vec2> Utils::interpolateVec2(vec2 from, vec2 to, int steps, int stepsY = -1){
// 	//stepsY is optional, and set to -1 only when we generate 'steps' number of 
// 	//steps for both axes.
// 	if(steps < 2 || (stepsY != -1 && stepsY < 2)) throw invalid_argument("interpolateVec2 must output at least two values");
// 	vector<float> xs = Utils::interpolateSingleFloats(from.x, to.x, steps);
// 	vector<float> ys = Utils::interpolateSingleFloats(from.y, to.y, steps);
// 	vector<vec2> result = {};
// }

std::vector<vec3> Utils::interpolateThreeElementValues(vec3 from, vec3 to, int numberOfValues){
	if(numberOfValues < 2) throw invalid_argument("interpolateThreeElementValues must output at least two values");
	vector<float> xs = Utils::interpolateSingleFloats(from.x, to.x, numberOfValues);
	vector<float> ys = Utils::interpolateSingleFloats(from.y, to.y, numberOfValues);
	vector<float> zs = Utils::interpolateSingleFloats(from.z, to.z, numberOfValues);
	vector<vec3> result = {};
	for(int i = 0; i < numberOfValues; i++){
		result.push_back(vec3(xs[i], ys[i], zs[i]));
	}
	return result;
}

uint32_t Utils::pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	return (a << 24) + (r << 16) + (g << 8) + b;
}