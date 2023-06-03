#include "Menu.h"
#include <iostream>
#include <SFML/Audio.hpp>
int Menu::display(sf::RenderWindow& window)
{
	sf::SoundBuffer buffer;
	sf::Sound denied;
	try {
		if (!buffer.loadFromFile("Resources\\denied.ogg")) {
			throw std::runtime_error("Error loading denied sound! 404, not found!");
		}
	}
	catch (std::runtime_error err) { std::cout << err.what(); }
	denied.setBuffer(buffer);

	sf::SoundBuffer buffer2;
	sf::Sound click;
	try {
		if (!buffer2.loadFromFile("Resources\\click.ogg")) {
			throw std::runtime_error("Error loading click sound! 404, not found!");
		}
	}
	catch (std::runtime_error err) { std::cout << err.what(); }
	click.setBuffer(buffer2);

	sf::SoundBuffer buffer3;
	sf::Sound help;
	try {
		if (!buffer3.loadFromFile("Resources\\help.ogg")) {
			throw std::runtime_error("Error loading help sound! 404, not found!");
		}
	}
	catch (std::runtime_error err) { std::cout << err.what(); }
	help.setBuffer(buffer3);

	sf::Font font;
	try {
		if (!font.loadFromFile("Resources\\font.ttf")) {
			throw std::runtime_error("Error loading font! 404, not found!");
		}
	}
	catch (std::runtime_error err) { std::cout << err.what(); }

	sf::Texture background_texture;
	try {
		if (!background_texture.loadFromFile("Resources\\background.png")) {
			throw std::runtime_error("Error loading background texture! 404, not found!");
		}
	}
	catch (std::runtime_error err) { std::cout << err.what(); }
	sf::Sprite background(background_texture);
	background.setScale(
		float(window.getSize().x) / background.getTexture()->getSize().x,
		float(window.getSize().y) / background.getTexture()->getSize().y);

	sf::Event running;
	window.clear();
	bool hflag = true;
	while (window.isOpen()) {
			window.pollEvent(running);
			if (running.type == sf::Event::Closed) {
				window.close();
			}
			if (running.type == running.MouseButtonPressed) {

				if (running.mouseButton.x > 660 && running.mouseButton.x < 1260 && running.mouseButton.y>436 && running.mouseButton.y < 495)
					return 1;
				
				if (running.mouseButton.x > 660 && running.mouseButton.x < 1260 && running.mouseButton.y>508 && running.mouseButton.y < 568) {
					click.play();
					denied.play();
				}

				if (running.mouseButton.x > 660 && running.mouseButton.x < 955 && running.mouseButton.y>652 && running.mouseButton.y < 712) {
					click.play();
					if (hflag) {
						std::cout << "LMB to add start point, LMB again to add end point. LMB to add walls. Space to start" << std::endl;
						hflag = false;
					}
				}

				if (running.mouseButton.x > 968 && running.mouseButton.x < 1260 && running.mouseButton.y>652 && running.mouseButton.y < 712)
					return 0;
			}
			window.draw(background);
			window.display();
	
	}
	window.clear();
	return 0;
}
