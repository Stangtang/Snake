#include "raylib.h"
#include <iostream>
#include <deque>
#include "raymath.h"
#include <algorithm>

using namespace std;

Color red = { 255, 0, 0, 255 };
Color green = { 0, 252, 0, 255 };

int cellSize = 30;
int cellCount = 25;

const char* restartText = nullptr;
const char* resetText = "Press Space or Any Control Key to Restart";

double lastUpdateTime = 0;

bool eventTriggered(double interval)
{
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}

bool ElementInDeque(Vector2 targetElement, deque<Vector2> deque)
{
	for (Vector2 currElement : deque)
	{
		if (Vector2Equals(currElement, targetElement))
		{
			return true;
		}
	}
	return false;
}

class Snake
{
public:
	deque<Vector2> body = { Vector2{6, 9} , Vector2{5, 9} , Vector2{4, 9} };
	Vector2 direction = { 1, 0 };

	bool addSegment = false;

	void Draw()
	{
		for (Vector2 segment : body)
		{
			float x = segment.x;
			float y = segment.y;
			DrawRectangle(x * cellSize + 1, y * cellSize + 1, cellSize - 2, cellSize - 2, green);
		}
	}

	void Update()
	{
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment) addSegment = false;
		else body.pop_back();
	}

	void Reset()
	{
		float x, y;
		switch (GetRandomValue(1, 4))
		{
		case 1: // right
			x = GetRandomValue(2, cellCount - 4);
			y = GetRandomValue(0, cellCount - 1);
			body = { Vector2{ x, y }, Vector2{ x - 1, y }, Vector2{ x - 2, y } };
			direction = { 1, 0 };
			break;
		case 2: // left
			x = GetRandomValue(3, cellCount - 3);
			y = GetRandomValue(0, cellCount - 1);
			body = { Vector2{ x, y }, Vector2{ x + 1, y }, Vector2{ x + 2, y } };
			direction = { -1, 0 };
			break;
		case 3: // up
			x = GetRandomValue(0, cellCount - 1);
			y = GetRandomValue(3, cellCount - 3);
			body = { Vector2{ x, y }, Vector2{ x, y + 1}, Vector2{ x, y + 2 } };
			direction = { 0, -1 };
			break;
		case 4: // down
			x = GetRandomValue(0, cellCount - 1);
			y = GetRandomValue(2, cellCount - 4);
			body = { Vector2{ x, y }, Vector2{ x, y - 1}, Vector2{ x, y - 2 } };
			direction = { 0, 1 };
			break;
		}
	}
};

class Food 
{
public:
	Vector2 position;

	Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
	{
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);

		while (ElementInDeque(Vector2{ x, y }, snakeBody))
		{
			x = GetRandomValue(0, cellCount - 1);
			y = GetRandomValue(0, cellCount - 1);
		}
		return Vector2{ x, y };
	}

	Food(deque<Vector2> snakeBody)
	{
		position = GenerateRandomPos(snakeBody);
	}

	void Draw()
	{
		DrawRectangle(position.x * cellSize, position.y * cellSize, cellSize, cellSize, red);
	}
};

class Game
{
public:
	Snake snake = Snake();
	Food food = Food(snake.body);

	bool isRunning = true;

	int score = 0;

	Sound eatSound, wallSound;

	Game()
	{
		InitAudioDevice();
		eatSound = LoadSound("Sounds/eat.mp3");
		wallSound = LoadSound("Sounds/wall.mp3");
	}

	~Game()
	{
		UnloadSound(eatSound);
		UnloadSound(wallSound);
		CloseAudioDevice();
	}

	void Draw()
	{
		food.Draw();
		snake.Draw();
	}

	void Update()
	{
		if (isRunning) 
		{
			snake.Update();
			CheckFoodCollision();
			CheckEdgeCollision();
			CheckBodyCollision();
		}
	}

	void CheckFoodCollision()
	{
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score++;
			PlaySound(eatSound);
		}
	}

	void CheckEdgeCollision()
	{
		if (snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == cellCount || snake.body[0].y == -1)
			GameOver();
	}

	void CheckBodyCollision()
	{
		deque<Vector2> headlessBody = snake.body;
		headlessBody.pop_front();

		if (ElementInDeque(snake.body[0], headlessBody))
			GameOver();
	}

	void GameOver()
	{
		PlaySound(wallSound);

		snake.Reset();
		food.position = food.GenerateRandomPos(snake.body);

		isRunning = false;

		restartText = "Game Over";
	}
};

int main() 
{
	InitWindow(cellSize * cellCount, cellSize * cellCount, "Snake");
	SetTargetFPS(60);

	Game game = Game();

	while (WindowShouldClose() == false)
	{
		if (eventTriggered(0.1))
		{
			game.Update();
		}

		if ((IsKeyPressed(KEY_SPACE)))
		{
			game.isRunning = true;
			restartText = nullptr;
			game.score = 0;
		}

		if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && game.snake.direction.y != 1) 
		{
			game.snake.direction = { 0, -1 };
			game.isRunning = true;
			restartText = nullptr;
			game.score = 0;
		}
		if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && game.snake.direction.y != -1) 
		{
			game.snake.direction = { 0, 1 };
			game.isRunning = true;
			restartText = nullptr;
			game.score = 0;
		}
		if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && game.snake.direction.x != 1)
		{
			game.snake.direction = { -1, 0 };
			game.isRunning = true;
			restartText = nullptr;
			game.score = 0;
		}
		if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && game.snake.direction.x != -1)
		{
			game.snake.direction = { 1, 0 };
			game.isRunning = true;
			restartText = nullptr;
			game.score = 0;
		}
			
		BeginDrawing();

		ClearBackground(BLACK);
		game.Draw();

		if (restartText)
		{
			DrawText(restartText, GetScreenWidth() / 2 - MeasureText(restartText, 60) / 2, GetScreenHeight() / 6 - 60, 60, WHITE);
			DrawText(resetText, GetScreenWidth() / 2 - MeasureText(resetText, 30) / 2, GetScreenHeight() / 6 - 30 + 60, 30, WHITE);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}