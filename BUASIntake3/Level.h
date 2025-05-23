#pragma once
#include "LDtkLoader/Project.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics.hpp"
#include <vector>

// Forward declarations for faster initialization   
class Player;
class Princess;
class Enemy;
class Objects;

class Level
{
public:

	ldtk::Project loadProject(const std::string& filepath);
	sf::Vector2f getPlayerStartPosition() const;
	sf::Vector2f getPrincessPosition() const; //Just a replica of getPlayerStartPosition()
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
	Enemy* getEnemyAtGrid(int gridX, int gridY) const;

	const std::vector<sf::Vector2f>& getObjectPositions() const { return objectPositions; }
	Objects* getObjectAtGrid(int gridX, int gridY) const;

	void setPlayer(Player* playerRef);
	void setPrincess(Princess* princessRef);
	void setEnemy(Enemy* EnemyRef);
	void setObject(Objects* ObjectRef);

	void updateCollision(float deltaTime);
	void playerTurnCountDown(sf::RenderTarget& window, int countdown);

	//Game UI (should put this Game class)
	void gameWon(sf::RenderTarget& window);
	void gameOver(sf::RenderWindow& window);
	void gamePaused(sf::RenderWindow& window);
	void gameInstructions(sf::RenderWindow& window);

	void resetGameState();
	void clearEnemies() { enemies.clear(); }
	void clearObjects() { objects.clear(); }

	bool getcolldingWithEnemy() const { return colldingWithEnemy; }
	bool isLeftEdgeColliding() const { return leftEdgeIsColliding; }
	bool isRightEdgeColliding() const { return rightEdgeIsColliding; }
	bool isTopEdgeColliding() const { return topEdgeIsColliding; }
	bool isBottomEdgeColliding() const { return bottomEdgeIsColliding; }

	bool hasWon;
	bool hasLost;
	bool reset = false;
	bool enemiesCleared;

private:
	ldtk::Project project;
	const ldtk::World& world; //Using pointers because World can't be default contructed
	const ldtk::Level& level; //Using pointers because Level can't be default contructed
	Player* player = nullptr;
	Princess* princess = nullptr;
	std::vector<Enemy*> enemies;
	std::vector<Objects*> objects;

	std::vector<sf::Vector2f> enemyPositions;
	std::vector<sf::Vector2f> objectPositions;
	
	sf::Font font;
	sf::Text text;
	sf::Sprite tile_sprite;
	sf::Texture tileset_texture;
	sf::View gameView;
	float zoomFactor = 1.0f;
	float baseWidth, baseHeight;

	bool colldingWithEnemy = false;
	bool leftEdgeIsColliding;
	bool rightEdgeIsColliding;
	bool topEdgeIsColliding;
	bool bottomEdgeIsColliding;

};