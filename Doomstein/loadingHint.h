#include <vector>
#include <string>
#pragma once

std::vector<std::wstring> loadingHints = { 
L"���� - ��� ����, ��� ��� ����� �����!",
L"����� ����� �����, ������� W",
L"����� ����� �����, ������� S", 
L"����� ����� �����, ������� A", 
L"����� ����� �������, ������� D", 
L"����� ����������� ������, ������� ����� �����", 
L"����� ����������� �������, ������� ����� ������",
L"����� ����� �� ����, ������� ESCAPE", 
L"����� �������� �������, ����������� ��� �������"};

std::wstring getRandomHint() {
	std::srand(std::time(0));
	return loadingHints[std::rand() % loadingHints.size()];
}


