#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>

class Entity
{
public:
	//Return the position and scaling
	~Entity() = default;
	Entity(const sf::Vector2f& pos);

	void draw();
	void setPosition(const sf::Vector2f& pos);
	const sf::Vector2f getPosition();
private:
	sf::Transform transform;
};

