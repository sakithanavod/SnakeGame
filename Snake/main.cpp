#include <iostream>
#include <Windows.h>
#include <sstream>
#include <thread>
#include <list>
#include <chrono>
#include "main.h"

using namespace std::chrono_literals;

//initialize console/window variables
const int SCREEN_WIDITH = 120;
const int SCREEN_HEIGHT = 30;
const int HORIZONTAL_OFFSET = 20;
const int VERTICAL_OFFSET = 5;
static wchar_t* screen = new wchar_t[SCREEN_WIDITH * SCREEN_HEIGHT];

//enum to set snake move direction
enum EDirection
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

//point objects defines x,y cordinates in the screen buffer
struct Point 
{
	int m_X{};
	int m_Y{};

	Point(int x, int y) :m_X(x),m_Y(y)
	{
	}

	Point() 
	{

	}

	//copy contructer to determine two points are equals/unequals
	bool operator==(const Point& other) 
	{
		return (m_X == other.m_X) && (m_Y == other.m_Y) ? true : false;
	}
};

//food class creates an object which can be consumed by snake
class Food
{
private:
	Point m_CurrentPosiiton; //gives currrent position of the spawned food
public:
	Food() 
	{
		MoveFood(); //initial position update for food
	}

	void MoveFood()
	{
		//determining a random location within boundries to spawn food
		//rand()%(max-min+1)+min;
		m_CurrentPosiiton.m_X = rand() % (SCREEN_WIDITH - 2 * HORIZONTAL_OFFSET) + HORIZONTAL_OFFSET+1;
		m_CurrentPosiiton.m_Y = rand() % (SCREEN_HEIGHT- 3*VERTICAL_OFFSET +1) + VERTICAL_OFFSET;

		//if the determined positon is already have a character then determine again
		if (screen[m_CurrentPosiiton.m_X + m_CurrentPosiiton.m_Y * SCREEN_WIDITH] != L' ') { MoveFood(); }

	}

	//draws food to screen
	void DrawFood() const
	{
		screen[m_CurrentPosiiton.m_X+ m_CurrentPosiiton.m_Y*SCREEN_WIDITH] = L'%';
	}

	//getter to get current postion of food
	Point GetCurrenPos() const
	{
		return m_CurrentPosiiton;
	}
};

//snake class creates an snake object which user can control
class Snake 
{
private:
	unsigned char m_Size = 5; //size of the snake
	Point m_DefaultPosition{ 60,12 }; //initial start positon of snake
	std::list<Point> m_SnakeBody; //snake body represented as a list of points
	wchar_t snakeArt = L'O'; //snake art for drawing snake

public:
	Snake(unsigned char size) : m_Size(size)
	{
		//constrcuter automatically determines snake body positions
		for (int i = 0; i < m_Size; i++) 
		{
			m_SnakeBody.push_back({ m_DefaultPosition.m_X+i,m_DefaultPosition.m_Y});

		}

	}

	//used to update snake art
	void ChangeSnakeArt(const wchar_t& art)
	{
		snakeArt = art;
	}

	//draws snake body in to screen
	void DrawSnake() const
	{
		for (const Point &point : m_SnakeBody) 
		{
			screen[point.m_X + SCREEN_WIDITH * point.m_Y ] = snakeArt;
		}

	}

	//Updates snakes body after eating food
	void IncreaseSize()
	{
		m_Size++;
		m_SnakeBody.push_back({ GeTailPos().m_X+1,GeTailPos().m_Y });
	}

	//Handles movement of snake based on player inputs
	void MoveSnake(const EDirection& direction) 
	{
		switch (direction)
		{
		case UP:
			m_SnakeBody.push_front({ m_SnakeBody.front().m_X, m_SnakeBody.front().m_Y - 1 });
			m_SnakeBody.pop_back();
			break;
		case DOWN:
			m_SnakeBody.push_front({ m_SnakeBody.front().m_X, m_SnakeBody.front().m_Y + 1 });
			m_SnakeBody.pop_back();
			break;
		case LEFT:
			m_SnakeBody.push_front({ m_SnakeBody.front().m_X - 1, m_SnakeBody.front().m_Y });
			m_SnakeBody.pop_back();
			break;
		case RIGHT:
			m_SnakeBody.push_front({ m_SnakeBody.front().m_X + 1, m_SnakeBody.front().m_Y });
			m_SnakeBody.pop_back();
			break;
		}

	}

	//check if snake hits its own body
	bool HitSelf() const
	{
		for(auto i= m_SnakeBody.begin();i!=m_SnakeBody.end();i++)
		{
			if(m_SnakeBody.begin()!=i)
			{
				if(GetHeadPos()==*i)
				{
					return true;
				}
			}
		}
		
		return false;
	}

	//helper to get snake head coordinates
	Point GetHeadPos() const
	{
		return m_SnakeBody.front();
	}

	//helper to get snake tail coordinates
	Point GeTailPos() const
	{
		return m_SnakeBody.back();
	}
};


//to draw level borders
void DrawLevel(wchar_t* screen)
{
	//Draw top & bottom horizontal line
	for (int i = 0; i < (SCREEN_WIDITH - HORIZONTAL_OFFSET * 2); i++)
	{
		screen[SCREEN_WIDITH * 4 + HORIZONTAL_OFFSET + i] = L'_';
		screen[SCREEN_WIDITH * 20 + HORIZONTAL_OFFSET + i] = L'_';
	}


	//Draw vertical left & right line
	for (int i = VERTICAL_OFFSET - 1; i <= SCREEN_HEIGHT - VERTICAL_OFFSET * 2; i++)
	{
		screen[SCREEN_WIDITH * i + HORIZONTAL_OFFSET] = L'|';
		screen[SCREEN_WIDITH * i + HORIZONTAL_OFFSET * 5] = L'|';
	}


}

void ClearScreen()
{
	//Clear screen
	for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDITH; i++)
	{
		screen[i] = L' ';
	}
}

void DrawInfo(const int& score)
{

	//Draw Stats & Border
	for (int i = 0; i < SCREEN_WIDITH; i++)
	{
		screen[i] = L'=';
		screen[SCREEN_WIDITH * 2 + i] = L'=';
	}

	wsprintf(&screen[SCREEN_WIDITH + 3], L"Verison:1                                       Saki Games - SNAKE!!                                     SCORE: %d",score);
}

void DrawEndScreen()
{
	wsprintf(&screen[23*SCREEN_WIDITH + 45], L"GAME OVER - PRESS SPACE TO RESTART");
}

int main()
{
	// Create Screen Buffer
	for (int i = 0; i < SCREEN_WIDITH * SCREEN_HEIGHT; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	

	while (1) {

		Snake snake = Snake(5);
		Food food = Food();
		bool isDead{};
		int score{};
		EDirection snakeDirection = EDirection::LEFT;

		while (!isDead)
		{
			//Timing & input
			auto t1 = std::chrono::system_clock::now();
			while ((std::chrono::system_clock::now() - t1)<200ms)
			{
				if (GetAsyncKeyState(VK_LEFT) && snakeDirection != EDirection::RIGHT)
				{
					snakeDirection = EDirection::LEFT;
				}
				else if (GetAsyncKeyState(VK_RIGHT) && snakeDirection != EDirection::LEFT)
				{
					snakeDirection = EDirection::RIGHT;

				}
				else if (GetAsyncKeyState(VK_UP) && snakeDirection != EDirection::DOWN)
				{
					snakeDirection = EDirection::UP;

				}
				else if (GetAsyncKeyState(VK_DOWN) && snakeDirection != EDirection::UP)
				{
					snakeDirection = EDirection::DOWN;
				}
			}
			//Game Logic
			snake.MoveSnake(snakeDirection);


			//Colision detection
			if (snake.GetHeadPos() == food.GetCurrenPos())
			{
				score++;
				food.MoveFood();
				snake.IncreaseSize();
			}

			//Colision detection with self
			isDead = snake.HitSelf();


			//Coliision detection with boundry
			for (int i = 0; i < (SCREEN_WIDITH - HORIZONTAL_OFFSET * 2); i++)
			{
				int snakeCor = snake.GetHeadPos().m_X + SCREEN_WIDITH * snake.GetHeadPos().m_Y;

				if (((SCREEN_WIDITH * 4 + HORIZONTAL_OFFSET + i) == (snakeCor)) ||
					((SCREEN_WIDITH * 20 + HORIZONTAL_OFFSET + i) == (snakeCor)))
				{
					isDead = true;
				}
			}

			for (int i = VERTICAL_OFFSET - 1; i <= SCREEN_HEIGHT - VERTICAL_OFFSET * 2; i++)
			{
				int snakeCor = snake.GetHeadPos().m_X + SCREEN_WIDITH * snake.GetHeadPos().m_Y;

				if (((SCREEN_WIDITH * i + HORIZONTAL_OFFSET) == (snakeCor)) ||
					((SCREEN_WIDITH * i + HORIZONTAL_OFFSET * 5) == (snakeCor)))
				{
					isDead = true;
				}
			}

			//Draw stuff to screen
			ClearScreen();
			DrawInfo(score);
			DrawLevel(screen);

			//check for dead condition
			if (isDead)
			{
				DrawEndScreen();
				snake.ChangeSnakeArt(L'X');

			}

			//draws snake and food to screen
			snake.DrawSnake();
			food.DrawFood();



			//Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, SCREEN_WIDITH * SCREEN_HEIGHT, { 0,0 }, &dwBytesWritten);
		}

		//wait till space bar input to restart game
		while (GetAsyncKeyState(VK_SPACE) == 0);
	}
	return 0;
}

