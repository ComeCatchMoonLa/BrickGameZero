#include<iostream>
#include<Windows.h>
#include<conio.h>
#include<list>
#include<queue>
#include<string>
#include<random>

using namespace std;

struct Position
{
	int x;
	int y;
};

enum Dir
{
	Up,
	Down,
	Left,
	Right,
};

enum KeyBorad
{
	W = 119,
	A = 97,
	S = 115,
	D = 100,
};

const int SCREEN_WIDTH = 40;
const int SCREEN_HEIGHT = 25;

list<Position> snake;
Position food;
Dir dir;
queue<Position> erasureQueue;

HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
bool gameOver = true;

void setPos(int x, short y)
{
	COORD pos = {(short)(x << 1), y};
	SetConsoleCursorPosition(hConsoleOutput, pos);
}

bool outOfBounds(int x, int y)
{
	return x < 0 || x > SCREEN_WIDTH - 1 || y < 0 || y > SCREEN_HEIGHT - 1;
}

bool isSnakeBody(int x, int y)
{
	for (const auto& pos : snake)
		if (x == pos.x && y == pos.y)
			return true;
	return false;
}

void createFood()
{
	int screenSize = SCREEN_WIDTH * SCREEN_HEIGHT;
	int emptyCellCount = screenSize - snake.size();

	std::default_random_engine randomEngine(std::random_device {}());
	std::uniform_int_distribution<int> distCell(0, emptyCellCount - 1);
	int newFoodPos = distCell(randomEngine);

	while (isSnakeBody(newFoodPos / SCREEN_WIDTH, newFoodPos % SCREEN_WIDTH))
		newFoodPos = (newFoodPos + 1) % screenSize;

	food.y = newFoodPos / SCREEN_WIDTH;
	food.x = newFoodPos % SCREEN_WIDTH;
}

void init()
{
	// init cosole
	SetConsoleTitle(L"贪吃蛇");

	std::string command = "mode con cols=" + to_string(SCREEN_WIDTH * 2) + "lines=" + to_string(SCREEN_HEIGHT);
	system(command.c_str());

	CONSOLE_CURSOR_INFO consoleCursorInfo;
	GetConsoleCursorInfo(hConsoleOutput, &consoleCursorInfo);
	consoleCursorInfo.bVisible = false;
	SetConsoleCursorInfo(hConsoleOutput, &consoleCursorInfo);

	// init snake
	snake = {{15, 20}, {16, 20}, {17, 20}};
	dir = Dir::Up;

	// init food
	createFood();
}

void handleGameOver()
{
	gameOver = true;

	// show tip
	setPos(15, 20);
	cout << "游戏结束, 按[空格键]继续";

	while (true)
		if (_kbhit() and _getch() == 32)
			break;
}

void updateLogic()
{
	// handle input
	if (_kbhit())
	{
		KeyBorad c = (KeyBorad)_getch();
		switch (c)
		{
			case KeyBorad::W:
			{
				if (dir != Dir::Up && dir != Dir::Down)
				{
					dir = Dir::Up;
				}
			}
			break;
			case KeyBorad::A:
			{
				if (dir != Dir::Left && dir != Dir::Right)
				{
					dir = Dir::Left;
				}
			}
			break;
			case KeyBorad::S:
			{
				if (dir != Dir::Up && dir != Dir::Down)
				{
					dir = Dir::Down;
				}
			}
			break;
			case KeyBorad::D:
			{
				if (dir != Dir::Left && dir != Dir::Right)
				{
					dir = Dir::Right;
				}
			}
			break;
		}
	}

	// handle snake move
	Position newHeadPosition = snake.front();
	switch (dir)
	{
		case Dir::Up:
			newHeadPosition.y -= 1; break;
		case Dir::Down:
			newHeadPosition.y += 1; break;
		case Dir::Left:
			newHeadPosition.x -= 1; break;
		case Dir::Right:
			newHeadPosition.x += 1; break;
	}
	snake.push_front(newHeadPosition);


	// handle eat food
	if (snake.front().x == food.x && snake.front().y == food.y)
	{
		createFood();
	}
	else
	{
		erasureQueue.emplace(snake.back().x, snake.back().y);
		snake.pop_back();
	}

	// handle snake dead
	if (outOfBounds(snake.front().x, snake.front().y))
		handleGameOver();
	for (auto it = ++snake.begin(); it != snake.end(); ++it)
		if ((*it).x == snake.front().x && (*it).y == snake.front().y)
			handleGameOver();
}

void updateRender()
{
	// render snake
	bool isHead = true;
	for (const auto& pos : snake)
	{
		setPos(pos.x, pos.y);
		std::cout << (isHead ? "□" : "■");
		isHead = false;
	}

	// render food
	setPos(food.x, food.y);
	cout << "●";

	// render empty
	while (!erasureQueue.empty())
	{
		setPos(erasureQueue.front().x, erasureQueue.front().y);
		cout << "  ";
		erasureQueue.pop();
	}
}

int main()
{
	while (true)
	{
		if (gameOver)
		{
			system("cls");
			init();
			gameOver = false;
		}

		if (!gameOver)
		{
			updateLogic();
			updateRender();
			Sleep(1000 / 5);
		}
	}

	cin.get();
}
