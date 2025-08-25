#include <iostream>
#include <vector>
#include <conio.h> // _kbhit, _getch on Windows (MSVC/MinGW)
#ifdef _WIN32
#include <windows.h>
#endif

// Simple 2D grid WASD movement demo
// Player: '@', Wall: '#', Empty: '.'

struct Position {
	int row;
	int col;
};

static void clearScreen() {
	// Works in most terminals on Windows with ANSI enabled (PowerShell supports it)
	std::cout << "\x1b[2J\x1b[H"; // clear + move cursor home
}

static void draw(const std::vector<std::string>& grid, const Position& player) {
	clearScreen();
	for (int r = 0; r < (int)grid.size(); ++r) {
		for (int c = 0; c < (int)grid[r].size(); ++c) {
			if (r == player.row && c == player.col) {
				std::cout << '@';
			} else {
				std::cout << grid[r][c];
			}
		}
		std::cout << "\n";
	}
	std::cout << "\n" << u8"WASD 移动，Q 退出" << "\n";
}

int main() {
	#ifdef _WIN32
	// Switch console to UTF-8 to avoid garbled Chinese text
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);
	#endif
	// Create a small map with some walls
	std::vector<std::string> grid = {
		"##########",
		"#........#",
		"#..##....#",
		"#........#",
		"#....##..#",
		"#........#",
		"##########",
	};

	Position player{1, 1};
	draw(grid, player);

	while (true) {
		// Non-blocking check; if unavailable, you can block on _getch
		int ch = 0;
		if (_kbhit()) {
			ch = _getch();
		} else {
			// Busy-wait minimal; could add Sleep here if desired
			continue;
		}

		char key = static_cast<char>(ch);
		if (key >= 'A' && key <= 'Z') key = static_cast<char>(key - 'A' + 'a');

		if (key == 'q') {
			break;
		}

		Position next = player;
		switch (key) {
			case 'w': next.row -= 1; break;
			case 's': next.row += 1; break;
			case 'a': next.col -= 1; break;
			case 'd': next.col += 1; break;
			default: break; // ignore others
		}

		// Bounds + collision with walls '#'
		if (next.row >= 0 && next.row < (int)grid.size() &&
			next.col >= 0 && next.col < (int)grid[0].size() &&
			grid[next.row][next.col] != '#') {
			player = next;
			draw(grid, player);
		}
	}

	clearScreen();
	std::cout << "已退出。" << std::endl;
	return 0;
}


