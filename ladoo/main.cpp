#include <iostream>
#include <SFML/Graphics.hpp>
#include <sstream>

const float GRAVITY = 1.8f;
const float DAMPING = 0.8f;
const float BALL_RADIUS = 55.0f;

struct Ball {
	sf::CircleShape shape;
	sf::Vector2f velocity;
	int id;
	bool isDragging = false;

	sf::Text text;

	Ball(float x, float y, int id, sf::Font& font): text(font){
		shape.setRadius(BALL_RADIUS);
		shape.setFillColor(sf::Color::Red);
		shape.setPosition({ x, y });
		this->id = id;

		text.setFont(font);
		text.setCharacterSize(12);

		updateText();
		//text.setString(std::to_string(id));

		text.setOrigin(text.getGlobalBounds().size / 2.0f);
		updateTextPosition();
	}

	void updateText() {
		text.setString("ID: " + std::to_string(id) +
			"\nVx: " + std::to_string((int)velocity.x) +
			" Vy: " + std::to_string((int)velocity.y));
	};

	void updateTextPosition() {
		sf::Vector2f pos = shape.getPosition();
		text.setPosition({ pos.x + BALL_RADIUS, pos.y + BALL_RADIUS });
	}
};

int main(){
	unsigned int width = 800;
	unsigned int height = 600;

	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode({ width, height }), "Bouncy");
	window->setFramerateLimit(60);

	sf::Font font;

	if (!font.openFromFile("Assets/Fonts/Poppins-Black.ttf"))
	{
		std::cerr << "ERROR::COULD NOT LOAD FILE::Assets\Fonts\Poppins-Black.ttf" << std::endl;
		return -1;
	}

	sf::Text ballsCount(font);

	ballsCount.setFillColor(sf::Color::White);
	ballsCount.setCharacterSize(18);

	std::vector<Ball> balls;
	Ball* selectedBall = nullptr;

	while (window->isOpen()) {

		//1. Handle Events - Process user input
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
					if(!alreadyBall) balls.emplace_back(mousePosition.x - BALL_RADIUS, mousePosition.y - BALL_RADIUS, balls.size(),font);
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

		//2. Game Logic

		if (selectedBall && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
			sf::Vector2f mouseP = static_cast<sf::Vector2f>(mousePos);
			selectedBall->shape.setPosition({ mouseP.x - BALL_RADIUS, mouseP.y - BALL_RADIUS });
			selectedBall->velocity = { 0.0f, 0.0f };
			selectedBall->updateTextPosition();
			selectedBall->updateText();
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
				ball.updateTextPosition();
				ball.updateText();
			}
		}

		//check collision with boundries
		for (auto& ball : balls) {
				sf::Vector2f pos = ball.shape.getPosition();
				sf::Vector2f velocity = ball.velocity;

				//Check collision with left boundry
				if (pos.x <= 0.0f) {
					pos.x = 0.0f;
					velocity.x = -velocity.x * DAMPING;
				}

				//Check collision with right boundry
				else if (pos.x + 2 * BALL_RADIUS >= width) {
					pos.x = width - 2 * BALL_RADIUS;
					velocity.x = -velocity.x * DAMPING;
				}

				//Check collision with the top boundry
				if (pos.y <= 0.0f) {
					pos.y = 0.0f;
					velocity.y = -velocity.y * DAMPING;
				}

				//Check collision with bottom boundry
				//else if (pos.y + 2 * BALL_RADIUS >= height) {
				//	pos.y = height- 2 * BALL_RADIUS;
				//	velocity.y = -velocity.y * DAMPING;
				//}

				ball.shape.setPosition(pos);
				ball.velocity = velocity;
				ball.updateTextPosition();
				ball.updateText();
		}

		//Check if balls are colliding each other
		for (int i = 0; i < balls.size(); ++i) {
			for (int j = i+ 1; j < balls.size(); ++j) {
				//If shape bound are close to each other, log the id
				Ball& a= balls[i];
				Ball& b = balls[j];

				sf::Vector2f aPos = a.shape.getPosition();
				sf::Vector2f bPos = b.shape.getPosition();

				sf::Vector2f delta = bPos - aPos;

				float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);

				if ((distance < (BALL_RADIUS * 2)) && distance > 0.0f) {

					sf::Vector2f normal = delta / distance;

					sf::Vector2f relativeVelocity = b.velocity - a.velocity;

					float speed = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

					if (speed > 0) {
						std::cout << "Collision between: " << a.id << " and " << b.id << std::endl;
						a.shape.setFillColor(sf::Color::Green);
						b.shape.setFillColor(sf::Color::Blue);
						a.velocity -= normal * (speed / 2) * DAMPING;
						b.velocity += normal * (speed / 2) * DAMPING;
					}
				}
				a.shape.setFillColor(sf::Color::Red);
				b.shape.setFillColor(sf::Color::Red);

			}
		}

		//3. Clean the screen
		//Render - remove everything from previous frame, called at the beginning of the frame
		window->clear(sf::Color::Black);

		std::stringstream ss;
		ss << "Total Balls: " << balls.size() << "";
		ballsCount.setString(ss.str());

		// 4. Draw Objects
		//Drawing here - order matters
		for (const auto& ball : balls) {
			window->draw(ball.shape);
			window->draw(ball.text);
		}
		window->draw(ballsCount);

		// 5. Dispaly the Updated frame
		window->display();
	}

	delete window;
	return 0;
}