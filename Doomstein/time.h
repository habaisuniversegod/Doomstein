#include <SFML/System.hpp>

#pragma once

namespace timeStuff {
	float deltaTime;
	int fps = 0;
	int maxfps = 0;
	int minfps = 100000;
	uint64_t totalElapsed;
	sf::Clock clockt;
}

void initTimer() {
	timeStuff::deltaTime = 0.0f;
	timeStuff::clockt.restart();
}

void resetFPS() {
	timeStuff::fps = 0;
	timeStuff::maxfps = 0;
	timeStuff::minfps = 100000;
}

void updateFPS() {
	using namespace timeStuff;
	deltaTime = clockt.getElapsedTime().asMicroseconds() / 1e6f;
	fps = (int)(1.0f / deltaTime);
	if (fps > maxfps) maxfps = fps;
	if (fps < minfps) minfps = fps;
	totalElapsed += clockt.getElapsedTime().asMicroseconds();
	clockt.restart();
}
