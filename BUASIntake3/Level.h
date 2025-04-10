#pragma once
#include "LDtkLoader/Project.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics.hpp"
#include <vector>

class Level
{
public:

	ldtk::Project loadProject(const std::string& filepath);
	sf::Vector2f getStartPosition() const;
	Level(const std::string& filepath, sf::RenderWindow& window);
	void draw(sf::RenderTarget& image);
	void Resize(sf::RenderWindow& window);
	const sf::View& getGameView() const { return gameView; }

	const ldtk::Level& getLevel() const { return level; }
	const ldtk::World& getWorld() const { return world; }

	bool isWalkingGround(int gridX, int gridY) const;
	int getWalkingGroundCellSize() const;

	sf::Vector2i getGridSize() const;

	const std::vector<sf::Vector2f>& getEnemyPositions() const { return enemyPositions; }

private:
	ldtk::Project project;
	const ldtk::World& world; //Using pointers because World can't be default contructed
	const ldtk::Level& level; //Using pointers because Level can't be default contructed

	std::vector<sf::Vector2f> enemyPositions;
	
	sf::Sprite tile_sprite;
	sf::Texture tileset_texture;
	sf::View gameView;
	float zoomFactor = 1.0f;
	float baseWidth, baseHeight;
};