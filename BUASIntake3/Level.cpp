#include "Level.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Image.hpp"
#include "Player.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <set>


ldtk::Project Level::loadProject(const std::string& filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open file: " + filepath);
	}
	// Read the file content
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	std::cout << "File content:\n" << content << "\n";

	// Load the ldtk Project
	ldtk::Project proj;
	try {
		std::cout << "Attempting to load project...\n";
		proj.loadFromFile(filepath);
		std::cout << "Project loaded successfully.\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Failed to load LDtk file: " << e.what() << "\n";
		throw;
	}

	//Entitie layer
	//const auto& entities = level.getLayer("Entities");
	//const auto& startPos = entities.getEntitiesByName("Start")[0].get();
	//playerStart = { startPos.getPosition().x, startPos.getPosition().y };

	return proj;
}

sf::Vector2f Level::getStartPosition() const
{
	try {
		const auto& entities = level.getLayer("Objects");
		const auto& startEntities = entities.getEntitiesByName("Start");

		if (!startEntities.empty()) {
			const auto& startPos = startEntities[0].get();
			return sf::Vector2f(startPos.getPosition().x + startPos.getSize().x / 2.0f,
								startPos.getPosition().y + startPos.getSize().y / 2.0f);
		}
		return sf::Vector2f(baseWidth / 2, baseHeight / 2);
	}
	catch (const std::exception& e) {
		std::cerr << "Couldnt find startEntity" << e.what() << "\n";
		throw;
	}

	return sf::Vector2f(baseWidth / 2, baseHeight / 2);
}


Level::Level(const std::string& filepath, sf::RenderWindow& window) :
	project(loadProject(filepath)),
	world(project.allWorlds().empty() ? throw std::runtime_error("No worlds found in LDtk project") : project.allWorlds()[0]),
	level(world.allLevels().empty() ? throw std::runtime_error("No levels found in world") : world.allLevels()[0])
{
	//Get level dimensions
	baseWidth = level.size.x;
	baseHeight = level.size.y;

	//Setup game view to center of the window
	gameView.setSize(baseWidth, baseHeight);
	gameView.setCenter(baseWidth / 2, baseHeight / 2);

	std::cout << "World name: " << world.getName() << "\n";
	std::cout << "Level name: " << level.name << ", ID: " << level.iid << "\n";

	//Load tileset
	if (!tileset_texture.loadFromFile("Assets/BG/background.png")) {
		printf("Failed to load tileset texture");
	}
	tile_sprite.setTexture(tileset_texture);

	Resize(window);

	for (const auto& layer : level.allLayers()) {
		// Check if the layer is an entity layer
		if (layer.getType() == ldtk::LayerType::Entities) {
			const auto& entityLayer = layer.allEntities();
			for (const auto& entity : entityLayer) {
				// Access entity properties
				std::cout << "Entity ID: " << entity.iid << std::endl;
				std::cout << "Entity name: " << entity.getName() << std::endl;
				std::cout << "Position: (" << entity.getPosition().x << ", " << entity.getPosition().y << ")" << std::endl;
				std::cout << "Size: (" << entity.getSize().x << ", " << entity.getSize().y << ")" << std::endl;

				for (const auto& field : entity.allFields()) {
					printf("Field ", field.name, "= ", field.type);
				}
			}
		}
	}
}

void Level::draw(sf::RenderTarget& image)
{
	// Set the custom view before drawing
	image.setView(gameView);

	int tile_size = 16;
	//Parse background layer
	const auto& bg_layer = level.getLayer("BG");

	//Keep track of grid positions where we've already drawn a dot
	std::set<std::pair<int, int>> drawn_positions;
	
	for (const auto& tile : bg_layer.allTiles()) {
		// Get the grid position of the tile
		int grid_x = tile.getGridPosition().x;
		int grid_y = tile.getGridPosition().y;

		int tileset_x = (tile.tileId % (tileset_texture.getSize().x / tile_size)) * tile_size;
		int tileset_y = (tile.tileId / (tileset_texture.getSize().x / tile_size)) * tile_size;
		tile_sprite.setTextureRect(sf::IntRect(tileset_x, tileset_y, tile_size, tile_size));
		tile_sprite.setPosition(tile.getGridPosition().x * tile_size, tile.getGridPosition().y * tile_size);
		image.draw(tile_sprite);
	}

	//Displays centerpoint of intGrid layer WalkingGround
	try {
		const auto& walkingLayer = level.getLayer("WalkingGround");
		const int grid_size = walkingLayer.getCellSize();
		//Deepseek solution
		for (int y = 0; y < walkingLayer.getGridSize().y; y++) {
			for (int x = 0; x < walkingLayer.getGridSize().x; x++) {
				const auto& value = walkingLayer.getIntGridVal(x, y).value;

				if (value != -1) {
					float center_X = x * grid_size + grid_size / 2.0f;
					float center_Y = y * grid_size + grid_size / 2.0f;
#ifndef NDEBUG
					sf::CircleShape centerDot(2.0f);
					centerDot.setFillColor(sf::Color::Green);
					centerDot.setPosition(center_X - 2.0f, center_Y - 2.0f);
					image.draw(centerDot);
#endif
				}
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error drawing WalkingGround layer: " << e.what() << "\n";
	}

	//Acces enitity properties
	// Iterate through layers in the level
	for (const auto& layer : level.allLayers()) {
		// Check if the layer is an entity layer
		if (layer.getType() == ldtk::LayerType::Entities) {
			const auto& entityLayer = layer.allEntities();

			for (const auto& entity : entityLayer) {
				sf::Vector2f position = sf::Vector2f(
					static_cast<float>(entity.getPosition().x),
					static_cast<float>(entity.getPosition().y)
				);

				sf::Vector2f size = sf::Vector2f(
					static_cast<float>(entity.getSize().x),
					static_cast<float>(entity.getSize().y)
				);
				// Create a rectangle shape for the entity's bounding box
				sf::RectangleShape rect(size);
				rect.setPosition(position);
#ifndef NDEBUG  //ONLY WHILE DEBUGGING 
				// Set the rectangle's outline to red
				rect.setFillColor(sf::Color::Transparent); // No fill
				rect.setOutlineColor(sf::Color::Red);      // Red outline
				rect.setOutlineThickness(2.0f);            // 2px border thickness
				image.draw(rect);						   // Draw the rectangle	
				// Draw entity center point
				sf::CircleShape centerDot(2.0f);
				centerDot.setFillColor(sf::Color::Cyan);
				centerDot.setPosition(
					position.x + size.x / 2.0f - 2.0f,
					position.y + size.y / 2.0f - 2.0f
				);
				image.draw(centerDot);

#endif			//ONLY WHILE DEBUGGING 
			}
		}	
	}
}

void Level::Resize(sf::RenderWindow& window)
{
	//Get all window and level dimensions
	sf::Vector2u windowSize = window.getSize();
	//Calculate aspect ratios
	float windowAspect = static_cast<float>(windowSize.x) / windowSize.y;
	float levelAspect = baseWidth / baseHeight;

	if (windowAspect > levelAspect) {
		//Make Window fit to height
		gameView.setSize(baseWidth * windowAspect, baseHeight);
	}
	else
	{
		//Make Window fit to width
		gameView.setSize(baseWidth, baseWidth / windowAspect);
	}
	//Center Window
	gameView.setCenter(baseWidth / 2, baseHeight / 2);
}

bool Level::isWalkingGround(int gridX, int gridY) const
{
	//Deepseek solution
	try {
		const auto& walkingLayer = level.getLayer("WalkingGround");
		bool walkable = (walkingLayer.getIntGridVal(gridX, gridY).value != -1);
		std::cout << "Checking tile (" << gridX << "," << gridY << "): "
			<< (walkable ? "Walkable" : "Blocked") << "\n";
		return walkable;
	}
	catch (const std::exception& e) {
		std::cerr << "Error returning WalkingGround layer: " << e.what() << "\n";
		return false; // Invalid if layer/tile not found
	}
}