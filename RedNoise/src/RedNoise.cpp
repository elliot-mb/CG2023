#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>

using namespace std;

#define WIDTH 320
#define HEIGHT 240

vector<float> interpolateSingleFloats(float from, float to, int numberOfValues){
	if(numberOfValues < 2) throw invalid_argument("interpolateSingleFloats must output at least two values");
	float delta = to - from;
	float step = delta / (numberOfValues - 1);
	vector<float> result = {};
	for(int i = 0; i < numberOfValues; i++) {
		result.push_back(from + (i * step));
	}
	return result;
}

void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

uint32_t pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	return (a << 24) + (r << 16) + (g << 8) + b;
}

void drawGreys(DrawingWindow &window){
	window.clearPixels();
	vector<float> greys = interpolateSingleFloats(255, 0, window.width);
	vector<uint8_t> greyWholes = {};
	for(int i = 0; i < greys.size(); i++){
		greyWholes.push_back(static_cast<uint8_t>(greys[i]));
	}
	for(int y = 0; y < window.height; y++){
		for(int x = 0; x < window.width; x++){
			uint8_t value = greyWholes[x];
			uint32_t colour = pack(255, value, value, value);
			window.setPixelColour(x, y, colour);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	vector<float> result = interpolateSingleFloats(2.2, 8.5, 7);
	for(int i=0; i < result.size(); i++) cout << result[i] << " ";
	cout << endl;

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		//draw(window);
		drawGreys(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
