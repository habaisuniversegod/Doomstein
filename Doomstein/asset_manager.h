#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "log.h"
#include "level_file.h"
#include "player.h"
#include "world.h"
#include "types.h"
#pragma once

class TextureLoader {
	std::map<std::string, std::string> texturePaths = 
	{ {"wall_sheet", "assets/textures/walls1_sheet.png"},
		{"floor", "assets/textures/floor.png"},
		{"ceiling", "assets/textures/ceiling.png"},
		{"w000_overlay", "assets/textures/weapons/w000_overlay.png"},
		{"logo", "assets/gui/game_logo.png"},
		{"play_button", "assets/gui/play_button.png"},
		{"menu_bg", "assets/textures/menu_bg.png"},
		{"pickup_aid", "assets/textures/pickups/aid.png"} };

public:
	std::map<std::string, SFPair*> otherTextures;

	TextureLoader() {}

	void init() {
		log(INFO, "Loading textures (" + _tostr_(texturePaths.size()) + " files)...");

		for (auto i = texturePaths.begin(); i != texturePaths.end(); i++) {
			log(INFO, "Loading texture '" + i->first + "' (file: '" + i->second + "')... ", "");
			SFPair* pair = new SFPair;
			pair->loadTexture(i->second);
			otherTextures[i->first] = pair;
			log(NONE, "[DONE] (res: " + _tostr_(pair->tx.getSize().x) + "x" + _tostr_(pair->tx.getSize().y) + ")");
		}

		log(INFO, _tostr_(texturePaths.size()) + " textures successfully loaded!");
	}

	~TextureLoader() {
		for (auto i = texturePaths.begin(); i != texturePaths.end(); i++) {
			if (i->first == "wall_sheet") continue;

			delete otherTextures[i->first];
		}
	}
};


class LevelLoader {
	static struct LevelData {
		sf::Vector3f playerCoords;
		float playerAngle;
		std::vector<Wall> walls;

		LevelData() {
			playerCoords = { 0, 0, 0 };
			playerAngle = 0;
		}
	};

	std::map<std::string, std::string> levelPaths =
	{ {"hub", "assets/levels/level1-1.dsl"},
		{"test1", "assets/levels/20230722-180745-level.dsl"}};

	std::map<std::string, LevelData> levelsData;

public:
	LevelLoader() { }

	void init() {
		int levelsTotal = 0;
		for (std::map<std::string, std::string>::iterator i = levelPaths.begin(); i != levelPaths.end(); i++)
		{
			log(INFO, "Loading level '" + i->first + "' (file: " + i->second + ")...", " ");
			auto rawData = readDSL(i->second.c_str());

			if (rawData.data == nullptr) {
				continue;
			}

			log(NONE, "(size: " + std::to_string(rawData.size) + " bytes)");

			auto parsedData = parseDSL(rawData);

			if (parsedData.wallsCount == -1) {
				log(ERROR, "Level '" + i->second + " cannot be loaded!");
				delete[] rawData.data;
				continue;
			}

			LevelData data;
			data.playerCoords = sf::Vector3f(parsedData.playerX, parsedData.playerY, parsedData.playerZ);
			data.playerAngle = parsedData.playerAngle;

			for (int i = 0; i < parsedData.wallsCount; i++)
			{
				data.walls.push_back(Wall(sf::Vector2f(parsedData.wallsX1[i], parsedData.wallsY1[i]), sf::Vector2f(parsedData.wallsX2[i], parsedData.wallsY2[i]), parsedData.wallsTXID[i]));
			}

			levelsData[i->first] = data;

			delete[] rawData.data;
			parsedData.cleanup();

			levelsTotal++;
		}
		log(INFO, "Loaded " + std::to_string(levelsTotal) + " levels");
	}

	void loadLevel(std::string levelName, World& world, Player& player) {
		if (levelName == "--empty--") {
			player.angle = 0;
			player.position = { 0.0f, 0.0f, 0.0f };
			world.walls.clear();
			return;
		}
		if (levelsData.count(levelName) == 0) {
			log(ERROR, "Level '" + levelName + "' not found or cannot be loaded!");
			LevelData data;
			data.playerAngle = 0;
			data.playerCoords = sf::Vector3f(0, 0, 0);
			data.walls.resize(0);

			player.position = data.playerCoords;
			player.angle = data.playerAngle;
			world.walls = data.walls;
			return;
		}
		LevelData& data = levelsData[levelName];
		player.position = data.playerCoords;
		player.angle = data.playerAngle;
		world.walls = data.walls;
	}
};


class TextsLoader {
	const std::map <std::string, std::string> fontPaths = { {"pixel", "assets/fonts/pixel.ttf"},
															{"pixel2_cyrillic", "assets/fonts/pixel2.ttf"} };
	const std::string defaultFontPath = "C:\\Windows\\fonts\\arial.ttf";

	std::map<std::string, sf::Font> fonts;
	std::map<std::string, sf::Text> textObjects;
	
public:
	TextsLoader() {}

	void init() {
		for (auto i = fontPaths.begin(); i != fontPaths.end(); i++)
		{
			if (!fonts[i->first].loadFromFile(i->second)) {
				log(ERROR, "Font '" + i->second + "' not found! Using default font '" + defaultFontPath + "'");
				fonts[i->first].loadFromFile(defaultFontPath);
			}
			log(INFO, "Font '" + i->second + "' loaded");
		}
	}

	sf::Text& getTextObject(std::string key) {
		return textObjects[key];
	}

	void addTextObject(std::string name, int fontSize, std::string fontName, sf::Color color, sf::Vector2f position, std::string text = "") {
		textObjects[name] = sf::Text();
		sf::Text& txt = textObjects[name];

		txt.setFont(fonts[fontName]);
		txt.setFillColor(color);
		txt.setCharacterSize(fontSize);
		txt.setPosition(position);
		txt.setString(text);
	}
};


class ShadersLoader {
	std::map<std::string, std::string> shadersPaths = { {"vignette", "assets/shaders/vignette.frag"},
		{"fcdraw", "assets/shaders/floor_ceiling_draw.frag"},
		{"pixelization", "assets/shaders/pixel.frag"}};
	std::map<std::string, sf::Shader*> shaders;
	
	bool isAvailable;

public:
	ShadersLoader() {}

	void init() {
		if (!sf::Shader::isAvailable()) {
			log(WARNING, "Shaders is not available!");
			isAvailable = false;
			return;
		}
		else {
			isAvailable = true;
		}

		for (auto i = shadersPaths.begin(); i != shadersPaths.end(); i++)
		{
			sf::Shader* shader = new sf::Shader;
			
			log(INFO, "Loading shader '" + i->first + "' (file: '" + i->second + "')...", " ");
			if (!shader->loadFromFile(i->second, sf::Shader::Fragment)) {
				log(NONE, "");
				log(ERROR, "Shader '" + i->second + "' not found");
				continue;
			}
			log(NONE, "[DONE]");
			shaders[i->first] = shader;
		}
	}

	sf::Shader* getShader(std::string key) {
		if (shaders.count(key) > 0) {	
			return shaders[key];
		}
		log(ERROR, "Cannot load shader '" + key + "'");
		return nullptr;
	}

	~ShadersLoader() {
		for (auto i = shaders.begin(); i != shaders.end(); i++)
		{
			delete shaders[i->first];
		}
	}
};


