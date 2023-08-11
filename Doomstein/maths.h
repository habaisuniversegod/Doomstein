#include <SFML/System.hpp>
#pragma once

struct Matrix {
	sf::Vector2f a;
	sf::Vector2f b;

	Matrix(float a, float b, float c, float d) {
		this->a = { a, b };
		this->b = { c, d };
	}

	Matrix(sf::Vector2f a, sf::Vector2f b) {
		this->a = a;
		this->b = b;
	}

	friend sf::Vector2f operator*(const Matrix& left, sf::Vector2f vector) {
		return sf::Vector2f(vector.x * left.a.x + vector.y * left.a.y, vector.x * left.b.x + vector.y * left.b.y);
	}

	friend sf::Vector2f operator*(sf::Vector2f vector, const Matrix& right) {
		return sf::Vector2f(vector.x * right.a.x + vector.y * right.a.y, vector.x * right.b.x + vector.y * right.b.y);
	}

	static Matrix makeRotation(float alpha) {
		return Matrix(cos(alpha), -sin(alpha), sin(alpha), cos(alpha));
	}
};

float vectorLength(sf::Vector2f vec) {
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

float vectorLength(sf::Vector3f vec) {
	return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

float dot(sf::Vector2f vec1, sf::Vector2f vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y;
}

float project(sf::Vector2f vec1, sf::Vector2f vec2) {
	return dot(vec1, vec2) / vectorLength(vec1);
}

sf::Vector2f norm(sf::Vector2f vec) {
	return vec / vectorLength(vec);
}

sf::Vector3f norm(sf::Vector3f vec) {
	return vec / vectorLength(vec);
}

float distance(sf::Vector2f vec1, sf::Vector2f vec2) {
	return std::sqrt(std::pow(vec1.x - vec2.x, 2) + std::pow(vec1.y - vec2.y, 2));
}

uint8_t fogColor(float distance) {
	return (uint8_t)(pow(FOG_BASE, distance) * 255.0f);
}

int getRandInt(int l, int h) {
	return l + std::rand() % (h - l);
}

bool isIntersect(sf::Vector2f ray1, sf::Vector2f ray2, sf::Vector2f wall1, sf::Vector2f wall2) {
	static float v1, v2, v3, v4;
	v1 = (wall2.x - wall1.x) * (ray1.y - wall1.y) - (wall2.y - wall1.y) * (ray1.x - wall1.x);
	v2 = (wall2.x - wall1.x) * (ray2.y - wall1.y) - (wall2.y - wall1.y) * (ray2.x - wall1.x);
	v3 = (ray2.x - ray1.x) * (wall1.y - ray1.y) - (ray2.y - ray1.y) * (wall1.x - ray1.x);
	v4 = (ray2.x - ray1.x) * (wall2.y - ray1.y) - (ray2.y - ray1.y) * (wall2.x - ray1.x);
	return (v1 * v2) < 0 && (v3 * v4) < 0;
}

uint8_t floorTexture(uint8_t x, uint8_t y) {
	return x ^ y;
}