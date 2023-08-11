#include <iostream>
#pragma once

#define _tostr_(x) (std::to_string(x))

enum LogType {
	NONE, INFO, DEBUG, WARNING, ERROR, AMOGUS
};

void log(LogType type, std::string message, std::string eol = "\n") {
	std::string nameOfType;

	switch (type) {
	case INFO:
		nameOfType = "[INFO] ";
		break;
	case DEBUG:
		nameOfType = "[DEBUG] ";
		break;
	case WARNING:
		nameOfType = "[WARNING] ";
		break;
	case ERROR:
		nameOfType = "[ERROR] ";
		break;
	case AMOGUS:
		nameOfType = "[AMOGUS] ";
		break;
	case NONE:
		break;
	}

	std::cout << nameOfType << message << eol;
}

void crash(std::string message) {
	log(ERROR, message);
	exit(-1);
}