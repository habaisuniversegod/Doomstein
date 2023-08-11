#include <vector>
#include <string>
#pragma once

std::vector<std::wstring> loadingHints = { 
L"Игра - это игра, так что нахуй пошел!",
L"Чтобы пойти вперёд, нажмите W",
L"Чтобы пойти назад, нажмите S", 
L"Чтобы пойти влево, нажмите A", 
L"Чтобы пойти направо, нажмите D", 
L"Чтобы повернуться налево, двиньте мышку влево", 
L"Чтобы повернуться направо, двиньте мышку вправо",
L"Чтобы выйти из игры, нажмите ESCAPE", 
L"Чтобы отсосать пиструн, расстегните мою ширинку"};

std::wstring getRandomHint() {
	std::srand(std::time(0));
	return loadingHints[std::rand() % loadingHints.size()];
}


