#define _CRT_SECURE_NO_WARNINGS

#include<SFML/Graphics.hpp>
#include<time.h>
#include<iostream>

#include "connect_stockfish.h"
#include "menu.h"

#include<Windows.h>

#pragma comment (lib, "Winmm.lib")



// дошка

//void play_vs_comp();




int main()
{
	char machine_name[CHAR_MAX];

	strcpy(machine_name, "stockfish.exe");

	ConnectToEngine(machine_name);
	//Створюю меню для гри

	sf::RenderWindow menuWindow(sf::VideoMode(453,453), "Menu");

	Menu menu(menuWindow.getSize().x, menuWindow.getSize().y);

	loadIcon(menuWindow);

	while (menuWindow.isOpen())
	{
		sf::Event e;

		while (menuWindow.pollEvent(e))
		{
			
			switch (e.type)
			{
			case e.Closed:
			{
				menuWindow.close();
				break;
			}
			case sf::Event::KeyPressed:
			{
				switch (e.key.code)
				{
				case sf::Keyboard::Up:
				{
					PlaySound("chesssounds/list_through.wav", NULL, SND_ASYNC);
					menu.MoveUp();
				}break;
				case sf::Keyboard::Down:
				{
					PlaySound("chesssounds/list_through.wav", NULL, SND_ASYNC);
					menu.MoveDown();
					break;
				}
				case sf::Keyboard::Return:

					PlaySound("chesssounds/select.wav", NULL, SND_SYNC);

					switch(menu.GetPressedItem())
					{
					case 0:
						
						play_vs_comp(menuWindow);
						
						break;

					case 1:
						
						comp_vs_comp(menuWindow);

						break;

					case 2:
						
						menuWindow.close();
						//
						break;
					}
				}
			}break;
			}
		}

		menuWindow.clear();

		menu.drawmenu(menuWindow);

		menuWindow.display();

	}

	CloseConnection();

	return EXIT_SUCCESS;
}