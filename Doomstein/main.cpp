#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>

#include "globals.h"
#include "world.h"
#include "asset_manager.h"
#include "SFXLoader.h"
#include "time.h"
#include "loadingHint.h"
#include "mob.h"


TextureLoader textures;
LevelLoader levels;
TextsLoader texts;
ShadersLoader shaders;
SFXLoader sounds;
MusicLoader music;

bool initialized = false;
float progress;

void loadAllAssets() {
	progress = 0.11f;
	textures.init();
	progress = 0.44f;
	levels.init();
	progress = 0.65f;
	texts.init();
	progress = 0.72f;
	shaders.init();
	progress = 0.83f;
	sounds.init();
	progress = 0.90f;
	music.init();
	progress = 0.99f;
	initialized = true;
}

void loadingState(sf::RenderWindow& win, std::thread& service, std::atomic<bool>& isEnd) {
	sf::Text* hintTextObjPtr = nullptr;
	SFPair lastStatePair;
	lastStatePair.tx.create(win.getSize().x, win.getSize().y);
	lastStatePair.tx.update(win);
	lastStatePair.spr.setTexture(lastStatePair.tx);
	lastStatePair.spr.setPosition(0, 0);
	float screenFadeTimer = 0.3f;
	bool screenFade = true;

	while (true) {
		sf::Event event;
		while (win.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				win.close();
				exit(1);
			}
		}
		if (screenFade) {
			screenFadeTimer -= timeStuff::deltaTime;
			if (screenFadeTimer < 0.0f) {
				screenFadeTimer = 0.0f;
				screenFade = false;
				service.detach();
			}
			lastStatePair.spr.setColor({ 255, 255, 255, (uint8_t)(screenFadeTimer / 0.3f * 255) });
		}

		if (isEnd && !screenFade) return;

		win.clear();
		win.draw(lastStatePair.spr);
		win.display();

		updateFPS();
	}
}

void changeMap__(std::atomic<bool>& isEnd, int order, World& world, Player& player) {
	levels.loadLevel(order ? "hub" : "test1", world, player);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	isEnd = true;
}

void gameState(sf::RenderWindow& win) {
	std::srand(std::time(0));

	Player player(sf::Vector3f(0, 0, 0), 0);
	World world(&player);

	player.getCamera().reserveIntersected(world.getWalls().size());
	
	for (int i = 0; i < 10; i++){
		world.addMob(new Mob(sf::Vector3f{ (float)getUniform(-100, 100), (float)getUniform(-100, 100), 1.0f}, 0.0f, TEST_MOB, &textures.otherTextures["pickup_aid"]->tx));
	}

	sf::RenderTexture preFX;
	preFX.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite preFXSpr;
	preFXSpr.setTexture(preFX.getTexture());

	sf::RenderTexture bg;
	bg.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	sf::Sprite bgSpr;
	bgSpr.setTexture(bg.getTexture());

	sf::RenderTexture minimap;
	minimap.create(MINIMAP_SIZE, MINIMAP_SIZE);
	sf::Sprite minimapSpr;
	minimapSpr.setTexture(minimap.getTexture());
	minimapSpr.setScale(1, -1);
	minimapSpr.setPosition(SCREEN_WIDTH - MINIMAP_SIZE - 10, MINIMAP_SIZE + 10);
	minimapSpr.setColor({ 255,255,255,224 });

	sf::RectangleShape vignetteLayer;
	sf::RectangleShape ground_;
	ground_.setFillColor(sf::Color(72, 154, 75));
	ground_.setPosition(0, SCREEN_HEIGHT / 2);
	ground_.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2));
	vignetteLayer.setFillColor(sf::Color::Black);
	vignetteLayer.setSize(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
	sf::Shader* vignetteShader = shaders.getShader("vignette");
	vignetteShader->setUniform("resolution", sf::Glsl::Vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	vignetteShader->setUniform("exp", 1.75f);
	vignetteShader->setUniform("radius", 1.45f);
	vignetteShader->setUniform("damage", 0.0f);
	sf::Shader* floorCeilDraw = shaders.getShader("fcdraw");

	floorCeilDraw->setUniform("screenSize", sf::Glsl::Vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	floorCeilDraw->setUniform("textureSize", FLOOR_CEILING_SIZE);
	floorCeilDraw->setUniform("projFOV", PLAYER_FOV);
	floorCeilDraw->setUniform("wallHeight", WALL_HEIGHT);
	floorCeilDraw->setUniform("projSize", sf::Glsl::Vec2(FOV_WIDTH, FOV_HEIGHT));
	floorCeilDraw->setUniform("floorTex", textures.otherTextures["floor"]->tx);
	floorCeilDraw->setUniform("ceilTex", textures.otherTextures["ceiling"]->tx);
	floorCeilDraw->setUniform("fogBase", FOG_BASE);

	sf::Shader* pixelShader = shaders.getShader("pixelization");
	pixelShader->setUniform("screen_size", sf::Glsl::Vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	pixelShader->setUniform("pixelSize", 1.0f);

	levels.loadLevel("--empty--", world, player);
	texts.addTextObject("debug_info", 12, "pixel", sf::Color::White, { 5, 5 });

	win.setMouseCursorVisible(false);

	float minimapZoom = 1.0f;
	float minimapZoomEase = 1.0f;
	texts.addTextObject("minimap_zoom", 12, "pixel", sf::Color::Black, { SCREEN_WIDTH - MINIMAP_SIZE - 8, 12 });

	initTimer();
	float exitAnimTimer = 0.59f;
	bool isExitAnim = false;

	float screenFadeTimer = 0.3f;
	bool screenFade = false;
	bool crashGame = false;

	preFXSpr.setColor(sf::Color(255, 255, 255, 195));
	music.getMusic("battle1")->play();
	//win.setFramerateLimit(15);
	updateFPS();
	resetFPS();

	while (true) {
		texts.getTextObject("debug_info").setString(std::string(title) + " (" + std::string(version) + ")\n\nFPS: " + std::to_string(timeStuff::fps) +
			" (Max: " + std::to_string(timeStuff::maxfps) + " | Min: " + std::to_string(timeStuff::minfps) +
			")\nPlayer X: " + std::to_string(player.getPositionXY().x) +
			"\nPlayer Y: " + std::to_string(player.getPositionXY().y) +
			"\nPlayer Z: " + std::to_string(player.getPositionZ()) +
			"\nPlayer look: " + std::to_string(player.getAngle()) +
			"\nFOV: " + std::to_string(PLAYER_FOV / PI * 180.0f) + 
			"\n\nWalls on level: " + std::to_string(world.getWalls().size()));
		texts.getTextObject("minimap_zoom").setString(std::to_string(minimapZoomEase) + "x");

		sf::Event event;

		while (win.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				pixelShader->setUniform("TX", preFX.getTexture());
				isExitAnim = true;
			}
			else if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
					case sf::Keyboard::Equal:
						minimapZoom *= minimapZoom < 8 ? 2 : 1;
						break;
					case sf::Keyboard::Dash:
						minimapZoom /= minimapZoom > 0.125 ? 2 : 1;
						break;
					case sf::Keyboard::F1:
					{std::atomic<bool> isEnd = false;
					static bool order = false;
					order = !order;
					std::thread thread(changeMap__, std::ref(isEnd), (int)order, std::ref(world), std::ref(player));
					music.getMusic("battle1")->pause();
					win.setVerticalSyncEnabled(1);
					loadingState(win, thread, isEnd);
					win.setVerticalSyncEnabled(0);
					music.getMusic("battle1")->play();
					screenFade = true; }
						break;
					case sf::Keyboard::F12:
						crashGame = true;
						log(AMOGUS, "Ty sam eto zahotel!");
						break;
					case sf::Keyboard::E:
						player.getCamera().showDamageEffect = true;
						player.getCamera().damageEffectTimer = 0.6f ;
						resetFPS();
						break;
				}
			}
		}

		if (player.getCamera().showDamageEffect) {
			player.getCamera().damageEffect(vignetteShader, timeStuff::deltaTime);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
			PLAYER_FOV += 1 * timeStuff::deltaTime;
			FOV_WIDTH = 2 * std::tan(PLAYER_FOV / 2);
			FOV_HEIGHT = 2 * std::tan(PLAYER_FOV / 2) / ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
			floorCeilDraw->setUniform("projSize", sf::Glsl::Vec2(FOV_WIDTH, FOV_HEIGHT));
			floorCeilDraw->setUniform("projFOV", PLAYER_FOV);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
			PLAYER_FOV -= 1 * timeStuff::deltaTime;
			FOV_WIDTH = 2 * std::tan(PLAYER_FOV / 2);
			FOV_HEIGHT = 2 * std::tan(PLAYER_FOV / 2) / ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
			floorCeilDraw->setUniform("projSize", sf::Glsl::Vec2(FOV_WIDTH, FOV_HEIGHT));
			floorCeilDraw->setUniform("projFOV", PLAYER_FOV);
		}

		if (abs(minimapZoom - minimapZoomEase) > 0.001f)
			minimapZoomEase += (minimapZoom - minimapZoomEase) * timeStuff::deltaTime * 10;
		else
			minimapZoomEase = minimapZoom;

		if (crashGame)
			world.__crash__(timeStuff::deltaTime);

		player.update(timeStuff::deltaTime, world.getWalls());
		player.rotate((float)(Camera::getMouseMove(sf::Vector2i((sf::Vector2i)win.getSize() / 2), win).x) / 1000.0f);
		player.getCamera().update(player.getPosition(), player.getAngle(), PLAYER_HEIGHT, player.moving, timeStuff::deltaTime);
		player.getCamera().rayCast(world.getWalls(), player.getCamera().intersected);
		player.getCamera().updateWallVertices(world.getWalls());
		world.updateMobs(timeStuff::deltaTime);

		floorCeilDraw->setUniform("cameraLook", player.getAngle());
		floorCeilDraw->setUniform("cameraPos", player.getCamera().getPosition());

		preFX.clear(sf::Color(0, 0, 0));
		preFX.draw(bgSpr, floorCeilDraw);
		preFX.draw(player.getCamera().wallLines, &textures.otherTextures["wall_sheet"]->tx);
		world.drawMobs(preFX, player.getCamera());
		player.getCamera().drawOverlay(preFX, textures.otherTextures["w000_overlay"], timeStuff::deltaTime);

		preFX.draw(vignetteLayer, vignetteShader);
		preFX.display();
		if (isExitAnim) {
			pixelShader->setUniform("pixelSize", std::pow(1.0f - (exitAnimTimer / 0.6f), 2.75f) * SCREEN_HEIGHT);
			win.draw(preFXSpr, pixelShader);
			exitAnimTimer = std::max(0.0f, exitAnimTimer - timeStuff::deltaTime);
			if (exitAnimTimer <= 0.0f) {
				win.setMouseCursorVisible(true);
				win.setFramerateLimit(60);
				music.getMusic("battle1")->stop();
				music.getMusic("main")->play();
				return;
			}
		}
		else {
			win.draw(preFXSpr);
		}

		world.drawMinimap(minimap, player.getPositionXY(), (sf::Vector2f)minimap.getSize() / 2.0f, player.getAngle(), MINIMAP_SCALE * minimapZoomEase);
		win.draw(minimapSpr);
		win.draw(texts.getTextObject("minimap_zoom"));

		win.draw(texts.getTextObject("debug_info"));

		if (screenFade) {
			screenFadeTimer -= timeStuff::deltaTime;
			if (screenFadeTimer < 0.0f){
				vignetteLayer.setFillColor(sf::Color::Black);
				screenFadeTimer = 0.3f;
				screenFade = false;

			}
			vignetteLayer.setFillColor(sf::Color(0, 0, 0, (uint8_t)(screenFadeTimer / 0.3f * 255)));
			win.draw(vignetteLayer);
		}

		win.display();

		updateFPS();
	}
}

void menuState(sf::RenderWindow& win) {
	std::thread loadingThread(loadAllAssets);
	loadingThread.detach();

	win.setFramerateLimit(60);

	SFPair* logo = nullptr;
	SFPair* playButton = nullptr;
	SFPair* menuBG = nullptr;

	sf::RectangleShape progressBarBg;
	sf::RectangleShape progressBarLine;
	sf::Text* hintTextObjPtr = nullptr;
	sf::Text* versTextObjPtr = nullptr;

	progressBarBg.setFillColor(sf::Color(31, 31, 31));
	progressBarLine.setFillColor(sf::Color(240, 225, 16));

	progressBarBg.setPosition(0, SCREEN_HEIGHT - 24);
	progressBarLine.setPosition(0, SCREEN_HEIGHT - 24);

	progressBarBg.setSize({ SCREEN_WIDTH, 24 });
	progressBarLine.setSize({ 0, 24 });

	int progressBarAnimTime = 255;
	int logoAnimTime = 0;
	
	bool progressDecay = false;
	bool logoAnim = false;

	float smoothProgress = 0.0f;
	float hintScreenTime = 0;

	initTimer();
	while (true)
	{
		if (initialized && music.getMusic("main")->getStatus() != sf::Music::Playing)
			music.getMusic("main")->play();

		if (initialized) {
			logo = textures.otherTextures["logo"];
			playButton = textures.otherTextures["play_button"];
			menuBG = textures.otherTextures["menu_bg"];

			texts.addTextObject("version", 12, "pixel", sf::Color::White, { 3, 3});
			texts.addTextObject("hint", 28, "pixel2_cyrillic", sf::Color::White, {10, SCREEN_HEIGHT - 65});
			hintTextObjPtr = &texts.getTextObject("hint");
			versTextObjPtr = &texts.getTextObject("version");
			hintTextObjPtr->setString(getRandomHint());
			versTextObjPtr->setString(version);

			logo->spr.setPosition(SCREEN_WIDTH / 2 - logo->tx.getSize().x / 2, 0);
			playButton->spr.setPosition(SCREEN_WIDTH / 2 - playButton->tx.getSize().x / 2, 320);
			menuBG->spr.setPosition(0, 0);

			initialized = false;
			progressDecay = true;
			logoAnim = true;
			progress = 1;
		}

		if (smoothProgress < progress) {
			smoothProgress = std::min(progress, smoothProgress + 0.05f);
		}

		progressBarLine.setSize({smoothProgress * SCREEN_WIDTH, 24});

		if (progressDecay) {
			progressBarAnimTime = std::max(0, progressBarAnimTime - 10);
			progressBarBg.setFillColor(sf::Color(31, 31, 31, progressBarAnimTime));
			progressBarLine.setFillColor(sf::Color(240, 225, 16, progressBarAnimTime));
			if (progressBarAnimTime == 0) {
				progressDecay = false;
			}
		}

		if (logoAnim && logo != nullptr) {
			logoAnimTime = std::min(255, logoAnimTime + 14);
			logo->spr.setColor(sf::Color(255, 255, 255, logoAnimTime));
			playButton->spr.setColor(sf::Color(255, 255, 255, logoAnimTime));
			menuBG->spr.setColor(sf::Color(255, 255, 255, logoAnimTime));
			if (logoAnimTime == 255) {
				logoAnim = false;
			}
		}


		hintScreenTime += timeStuff::deltaTime;
		if (hintTextObjPtr != nullptr) {
			float col = (hintScreenTime < 2.0f) ? (std::min(1.0f, hintScreenTime / 0.3f)) : (std::max(0.0f, std::min(1.0f, -(hintScreenTime - 4.0f) / 0.3f)));
			hintTextObjPtr->setFillColor(sf::Color(255, 255, 255, (uint8_t)(col * 255)));
			if (hintScreenTime > 4.0f) {
				hintScreenTime = 0;
				hintTextObjPtr->setString(getRandomHint());
			}
		}


		sf::Event event;
		while (win.pollEvent(event)) {
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				win.close();
				exit(1);
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					auto mousePos = sf::Mouse::getPosition(win);
					if (playButton != nullptr) {
						if (mousePos.x > playButton->spr.getPosition().x && mousePos.x <= playButton->spr.getPosition().x + playButton->tx.getSize().x &&
							mousePos.y > playButton->spr.getPosition().y && mousePos.y <= playButton->spr.getPosition().y + playButton->tx.getSize().y) {
							win.setFramerateLimit(10000);
							music.getMusic("main")->stop();
							gameState(win);
						}
					}
				}
			}
		}
		win.clear(sf::Color(45, 3, 0));
		if (menuBG != nullptr) win.draw(menuBG->spr);
		win.draw(progressBarBg);
		win.draw(progressBarLine);
		if (hintTextObjPtr != nullptr) win.draw(*hintTextObjPtr);
		if (versTextObjPtr != nullptr) win.draw(*versTextObjPtr);
		if (logo != nullptr) win.draw(logo->spr);
		if (playButton != nullptr) win.draw(playButton->spr);
		win.display();

		updateFPS();
	}
}

int main() {
	sf::RenderWindow win(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), std::string(title) + " (" + std::string(version) + ")", sf::Style::Close);

	menuState(win);
}