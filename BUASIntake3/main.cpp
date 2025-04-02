#include <fstream>
#include <iostream>  // For std::cerr and std::cout
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Level.h"
#include "Enemy.h"
#include "LDtkLoader/Project.hpp"

//void testJsonLoad(const std::string& filepath) {  
//  try {  
//      std::ifstream file(filepath, std::ios::in); // Correct initialization of ifstream  
//      if (!file.is_open()) {  
//          std::cerr << "Failed to open file: " << filepath << "\n";  
//          return;  
//      }  
//
//      using json = nlohmann::json; // Initialize jsonData to avoid uninitialized local variable error
//      json jsonData = json::object();  
//      std::cout << "JSON loaded successfully.\n";  
//  }  
//  catch (const std::exception& e) {  
//      std::cerr << "Error loading JSON: " << e.what() << "\n";  
//  }  
//}  

int main() {  
  sf::RenderWindow window(sf::VideoMode(800, 600), "Save the princess");  
  Player player;  
  Enemy enemy;
  Level level("Assets/BG/Levels.ldtk");  
  sf::Clock clock;  

  sf::Vector2i startPos = level.getStartPosition();
  player.setGridPosition(startPos.x, startPos.y);

  while (window.isOpen()) {  
      sf::Event event;  
      while (window.pollEvent(event)) {  
          if (event.type == sf::Event::Closed)  
              window.close();  
      }  

      float deltaTime = clock.restart().asSeconds();  

      window.clear(sf::Color::Black);  
      level.draw(window);  
      
      player.update(deltaTime);  
      player.draw(window);  
      enemy.update(deltaTime);
      enemy.draw(window);
      
      window.display();  
  }  

  return 0;  
}
