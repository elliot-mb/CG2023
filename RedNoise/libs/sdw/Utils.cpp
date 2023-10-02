#include <algorithm>
#include <sstream>
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

vector<std::string> split(const std::string &line, char delimiter) {
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

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues){
	if(numberOfValues < 2) throw invalid_argument("interpolateSingleFloats must output at least two values");
	float delta = to - from;
	float step = delta / (numberOfValues - 1);
	vector<float> result = {};
	for(int i = 0; i < numberOfValues; i++) {
		result.push_back(from + (i * step));
	}
	return result;
}

std::vector<vec3> interpolateThreeElementValues(vec3 from, vec3 to, int numberOfValues){
	if(numberOfValues < 2) throw invalid_argument("interpolateThreeElementValues must output at least two values");
	vector<float> xs = interpolateSingleFloats(from.x, to.x, numberOfValues);
	vector<float> ys = interpolateSingleFloats(from.y, to.y, numberOfValues);
	vector<float> zs = interpolateSingleFloats(from.z, to.z, numberOfValues);
	vector<vec3> result = {};
	for(int i = 0; i < numberOfValues; i++){
		result.push_back(vec3(xs[i], ys[i], zs[i]));
	}
	return result;
}

uint32_t pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	return (a << 24) + (r << 16) + (g << 8) + b;
}