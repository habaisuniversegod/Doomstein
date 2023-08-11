#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "log.h"
#pragma once

#define DSL_SIGNATURE "dsl"
#define DSL_VERSION_SIGNATURE 0xBB
#define DSL_WALL_DESCRIPTION_SIZE 17
#define DSL_HBWD_SIZE 30

struct BinaryData {
	uint8_t* data;
	uint32_t size;
};

struct Deserialized {
	float playerX;
	float playerY;
	float playerZ;
	float playerAngle;
	
	short wallsCount;
	
	float* wallsX1;
	float* wallsX2;
	float* wallsY1;
	float* wallsY2;
	uint8_t* wallsTXID;

	void init() {
		wallsX1 = new float[wallsCount];
		wallsY1 = new float[wallsCount];
		wallsX2 = new float[wallsCount];
		wallsY2 = new float[wallsCount];
		wallsTXID = new uint8_t[wallsCount];
	}

	void cleanup() {
		delete[] wallsX1;
		delete[] wallsX2;
		delete[] wallsY1;
		delete[] wallsY2;
		delete[] wallsTXID;
	}
};

float binaryToFloat(uint8_t* binary) {
	uint32_t asInt = (binary[3] << 24) | (binary[2] << 16) | (binary[1] << 8) | binary[0];
	return *reinterpret_cast<float*>(&asInt);
};

short binaryToShort(uint8_t* binary) {
	return (binary[1] << 8) | binary[0];
}

bool checkStr(const char* string, int stringSize, char* data) {
	for (int i = 0; i < stringSize; i++)
	{
		if (string[i] != data[i]) {
			return false;
		}
	}
	return true;
}

BinaryData readDSL(const char* filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	std::stringstream st;

	st << "Level file '" << filename << "' not found!";
	if (!in) {
		log(NONE, "");
		log(ERROR, st.str());
		return BinaryData{ nullptr, 0 };
	};

	BinaryData dt;
	in.seekg(0, std::ios::end);
	dt.size = in.tellg();
	in.seekg(0);

	dt.data = new uint8_t[dt.size];
	in.read((char*)dt.data, dt.size);

	in.close();

	return dt;
}

Deserialized parseDSL(BinaryData& data) {
	Deserialized d;

	if (!checkStr(DSL_SIGNATURE, 3, (char*)&data.data[0]) || data.data[3] != DSL_VERSION_SIGNATURE) {
		log(ERROR, "Level file err: invalid level file!");
		d.wallsCount = -1;
		return d;
	}

	d.wallsCount = binaryToShort(&data.data[4]);

	if (d.wallsCount < 0 || ((data.size - DSL_HBWD_SIZE) / DSL_WALL_DESCRIPTION_SIZE) < d.wallsCount) {
		log(ERROR, "Level file err: invalid walls count " + _tostr_(d.wallsCount) + "!");
		d.wallsCount = -1;
		return d;
	}

	if (!checkStr("play", 4, (char*)&data.data[6])) {
		log(ERROR, "Level file err: on file position 6 must be 'play'!");
		d.wallsCount = -1;
		return d;
	}

	if (!checkStr("wall", 4, (char*)&data.data[26])) {
		log(ERROR, "Level file err: on file position 26 must be 'wall'!");
		d.wallsCount = -1;
		return d;
	}

	d.init();
	
	d.playerX = binaryToFloat(&data.data[10]);
	d.playerY = binaryToFloat(&data.data[14]);
	d.playerZ = binaryToFloat(&data.data[18]);
	d.playerAngle = binaryToFloat(&data.data[22]);

	log(DEBUG, "Walls on level: " + std::to_string(d.wallsCount));
	log(DEBUG, "PLAYER XYZA: " + std::to_string(d.playerX) + ", " + std::to_string(d.playerY) + ", " + std::to_string(d.playerZ) + ", " + std::to_string(d.playerAngle) + ", ");

	for (short i = 0; i < d.wallsCount; i++)
	{
		d.wallsX1[i] = binaryToFloat(&data.data[DSL_HBWD_SIZE + i * DSL_WALL_DESCRIPTION_SIZE]);
		d.wallsY1[i] = binaryToFloat(&data.data[DSL_HBWD_SIZE + i * DSL_WALL_DESCRIPTION_SIZE + 4]);
		d.wallsX2[i] = binaryToFloat(&data.data[DSL_HBWD_SIZE + i * DSL_WALL_DESCRIPTION_SIZE + 8]);
		d.wallsY2[i] = binaryToFloat(&data.data[DSL_HBWD_SIZE + i * DSL_WALL_DESCRIPTION_SIZE + 12]);
		d.wallsTXID[i] = (uint8_t)data.data[DSL_HBWD_SIZE + i * DSL_WALL_DESCRIPTION_SIZE + 16];
	}

	return d;
}