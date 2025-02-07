#include <iostream>
#include <SFML/Graphics.hpp>

const float GRAVITY = 1.8f;
const float DAMPING = 0.8f;
const float BALL_RADIUS = 55.0f;

struct Ball {
	sf::CircleShape shape;
	sf::Vector2f velocity;
	bool isDragging = false;

	Ball(float x, float y) {
		shape.setRadius(BALL_RADIUS);
		shape.setFillColor(sf::Color::Red);
		shape.setPosition({ x, y });
	}
};

int main(){
	
	unsigned int width = 800;
	unsigned int height = 600;

	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({ width, height }), "Bouncy");
	window->setFramerateLimit(60);
	
	std::vector<Ball> balls;
	Ball* selectedBall = nullptr;

	while (window->isOpen()) {
		while (const std::optional event = window->pollEvent()){
			if (event->is < sf::Event::Closed>()) {
				window->close();
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
					window->close();
				}
			}
			if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				sf::Vector2i mousePosition = mouseButtonPressed->position;
				sf::Vector2f mousePositionFloat = static_cast<sf::Vector2f>(mousePosition);
				if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
					bool alreadyBall = false;
					for (auto& ball : balls) {
						if (ball.shape.getGlobalBounds().contains(mousePositionFloat)) {
							selectedBall = &ball;
							ball.isDragging = true;
							alreadyBall = true;
							break;
						}
					}
					if(!alreadyBall) balls.emplace_back(mousePosition.x - BALL_RADIUS, mousePosition.y - BALL_RADIUS);
				}else if (mouseButtonPressed->button == sf::Mouse::Button::Right) {
					for (auto it = balls.begin(); it != balls.end(); ++it) {
						if (it->shape.getGlobalBounds().contains(mousePositionFloat)) {
							balls.erase(it);
							break;
						}
					}
				}
			}
			if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (selectedBall) {
					selectedBall->isDragging = false;
					selectedBall = nullptr;
				}
			}
		}

		if (selectedBall && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
			sf::Vector2f mouseP = static_cast<sf::Vector2f>(mousePos);
			selectedBall->shape.setPosition({ mouseP.x - BALL_RADIUS, mouseP.y - BALL_RADIUS });
			selectedBall->velocity = { 0.0f, 0.0f };
		}

		//Add gravity to the balls
		for (auto& ball : balls) {
			if (!ball.isDragging) {
				ball.velocity.y += GRAVITY;
				sf::Vector2f pos = ball.shape.getPosition();
				pos += ball.velocity;

				if (pos.y + 2 * BALL_RADIUS > window->getSize().y) {
					pos.y = window->getSize().y - 2 * BALL_RADIUS;
					ball.velocity.y = -ball.velocity.y * DAMPING;
				}

				ball.shape.setPosition(pos);
			}
		}

		//Render
		window->clear(sf::Color::Black);

		//Drawing here
		for (const auto& ball : balls) {
			window->draw(ball.shape);
		}

		window->display();
	}

	delete window;
	return 0;
}