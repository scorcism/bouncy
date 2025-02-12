#pragma once
#include <SFML/Graphics.hpp>

const float GRAVITY = 1.8f;
const float DAMPING = 0.8f;
const float BALL_RADIUS = 55.0f;

struct Ball {
	sf::CircleShape shape;
	sf::Vector2f velocity;
	int id;
	bool isDragging ;

	Ball(float x, float y, int id);
};


class Game 
{

private:
	sf::RenderWindow* window;
	sf::VideoMode videoMode;


	sf::Font font;

	sf::Text scoreText;

	bool endGame;
	unsigned int height;
	unsigned int width;

	std::vector<Ball> balls;
	Ball* selectedBall;

	void initWindow();
	void initVariable();
	void initFont();
	void initText();


public:
	Game();

	virtual ~Game();

	const bool running() const;
	const bool getEndGame() const;
	
	void pollEvent();
	void renderBalls(sf::RenderTarget& target);
	void renderScore(sf::RenderTarget& target);
	void update();
	void render();
};