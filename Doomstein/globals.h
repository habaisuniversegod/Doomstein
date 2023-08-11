#pragma once
#include <cmath>
#define PI 3.1415

class LevelLoader;
class World;

const char title[] = "Doomstein: Peredyornal (0.2_1)";

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;
float PLAYER_FOV = PI / 2;
float FOV_WIDTH = 2 * std::tan(PLAYER_FOV / 2);
float FOV_HEIGHT = 2 * std::tan(PLAYER_FOV / 2) / ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
const float MIN_RAY_LENGTH = 0.0025f;
const float RENDER_DISTANCE = 250.0f;
const int COLLISION_RAYS = 20;
const int MINIMAP_SIZE = 180;
const float MINIMAP_SCALE = 4.0f;
const int WALL_TEXTURE_SIZE = 512;
const float WALL_HEIGHT = 3.75f;
const float FLOOR_CEILING_SIZE = 8.0f;
const float PLAYER_HEIGHT = 1.73f;
const float PLAYER_SIZE = 0.3f;
const float FOG_BASE = 0.987f;
const float FREE_FALL = -9.8f;
const float MAX_FALL_SPEED = 300.0f;
const float PLAYER_JUMP_ACCELERATION = 4.0f;

