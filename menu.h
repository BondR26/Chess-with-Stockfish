#pragma once

#include <SFML/Graphics.hpp>

#define MENU_ITEMS 3

class Menu
{

private:

	int selectedItemIndex;

	sf::Font font;

	sf::Text menu[MENU_ITEMS];

public:

	Menu(int width, int height);

	~Menu() {}

	void drawmenu(sf::RenderWindow& window);

	void MoveUp();

	void MoveDown();

	int GetPressedItem()
	{
		return selectedItemIndex;
	}



};