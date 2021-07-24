#include "menu.h"
#include <iostream>

Menu::Menu(int width, int height)
{
	if (!font.loadFromFile("BRLNSR.TTF"))
	{
		std::cout << "Font wasnt uploaded\n";
	}

	std::string s_menu[3] = {"Player vs Computer", "Computer vs Computer", "Exit"};


	for (int i = 0; i < MENU_ITEMS; i++)
	{
		menu[i].setFont(font);
		if (i == 0)
		{
			menu[i].setFillColor(sf::Color::White);
		}
		else
		{
			menu[i].setFillColor(sf::Color::Red);
		}
		menu[i].setString(s_menu[i]);
		menu[i].setPosition(sf::Vector2f(width / 4, height / 4 * (i + 1)));
	}

	selectedItemIndex = 0;
}

void Menu::drawmenu(sf::RenderWindow& window)
{
	for (int i = 0; i < MENU_ITEMS; i++)
	{
		window.draw(menu[i]);
	}
}


void Menu::MoveUp()
{
	if (selectedItemIndex - 1 >=0)
	{
		menu[selectedItemIndex].setFillColor(sf::Color::Red);
		selectedItemIndex--;
		menu[selectedItemIndex].setFillColor(sf::Color::White);
	}
}

void Menu::MoveDown()
{
	if (selectedItemIndex + 1 < MENU_ITEMS)
	{
		menu[selectedItemIndex].setFillColor(sf::Color::Red);
		selectedItemIndex++;
		menu[selectedItemIndex].setFillColor(sf::Color::White);
	}
}