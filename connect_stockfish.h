#pragma once

#include<windows.h>
#include<iostream>
#include<string>
#include<stdio.h>


STARTUPINFO sti ={ 0 };

SECURITY_ATTRIBUTES sats={ 0 };
PROCESS_INFORMATION pi={ 0 };
HANDLE pipin_w, pipin_r, pipout_w, pipout_r;
BYTE buffer[4096];
DWORD writ, excode, read, available;




void ConnectToEngine(char* path)
{
	pipin_w = pipin_r = pipout_w = pipout_r = NULL;

	sats.nLength = sizeof(sats);

	sats.bInheritHandle = TRUE;

	sats.lpSecurityDescriptor = NULL;

	CreatePipe(&pipout_r, &pipout_w, &sats, 0);

	CreatePipe(&pipin_r, &pipin_w, &sats, 0);

	sti.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	sti.wShowWindow = SW_HIDE;
	sti.hStdInput = pipin_r;
	sti.hStdOutput = pipout_w;
	sti.hStdError = pipout_w;

	CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &sti, &pi);

}

__declspec(safebuffers) std::string getNextMove(std::string position)
{
	std::string str;
	position = "position startpos moves " + position + "\ngo\n";

	WriteFile(pipin_w, position.c_str(), position.length(), &writ, NULL);

	Sleep(500);

	PeekNamedPipe(pipout_r, buffer, sizeof(buffer), &read, &available, NULL);
	// считує інформацію з труби в буфер при цьому труба не очищається
	do
	{

		ZeroMemory(buffer, sizeof(buffer));

		if (!ReadFile(pipout_r, buffer, sizeof(buffer), &read, NULL) || !read)//считує інформацію в вказаний мною буфер зі стокфіша
		{
			break;
		}

		buffer[read] = 0;

		str += (char*)buffer;//записати все шо считуємо з труби в буфер а потім в строку


	} while (read >= sizeof(buffer));//повторювати поки труба не зробиться пустою


	int n = str.find("bestmove");//саме пілс я ьцого слова в трубі йде найвдаліша комбінація (хід)

	int c = str.find("none");

	int d = str.find("ponder (none)");

	if (c != -1)
	{
		return "none";
	}

	if (n != -1)
	{
		return str.substr(n + 9, 4);//счиатаи саму комбінацію
	}

	return "error";

}





void CloseConnection()
{
	WriteFile(pipin_w, "quit\n", 5, &writ, NULL);
	if (pipin_w != NULL)
	{
		CloseHandle(pipin_w);
	}

	if (pipin_r != NULL)
	{
		CloseHandle(pipin_r);
	}

	if (pipout_w != NULL)
	{
		CloseHandle(pipout_w);
	}

	if (pipout_r != NULL)
	{
		CloseHandle(pipout_r);
	}
	

	if (pi.hProcess != NULL)
	{
		CloseHandle(pi.hProcess);
	}

	if (pi.hThread != NULL)
	{
		CloseHandle(pi.hThread);
	}

	TerminateProcess(pi.hProcess, 1);
}


void loadIcon(sf::RenderWindow& window)
{

	auto icon = sf::Image();
	if (!icon.loadFromFile("chessparts/chess.png"))
	{
		std::cout << "Icon wasnt uploaded!\n";
	}


	window.setIcon(256,256, icon.getPixelsPtr());
}


#define SIZE_OF_FIGURE 56

#define ALL_FIGURES 32

sf::Sprite figures[ALL_FIGURES];//кількість усхі фігур

std::string position = "";

sf::Vector2f offset(28, 28);


int board[8][8]{//кодовк позначення всіх фігур їх розташування і значення
	-1,-2,-3,-4,-5,-3,-2,-1,
	-6,-6,-6,-6,-6,-6,-6,-6,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 6, 6, 6, 6, 6, 6, 6, 6,
	 1, 2, 3, 4, 5, 3, 2, 1,
};


sf::Vector2f toCoord(char a, char b)//це функція для того щоб повертати двухвимірне розташування фігури судячи з ходу(симовлуів типу d2d4)
{
	int x = int(a) - 97;
	int y = 7 - int(b) + 49;

	return sf::Vector2f(x * SIZE_OF_FIGURE, y * SIZE_OF_FIGURE);
}


void fmove(std::string str)// преревіряє наявність інших фігур в клітинці, якщо таке фігура(вірніше позиція якоїсь із фігур в тій клітинці є видалити і нову фігуру закрпіти за цією клітинкою)
{
	sf::Vector2f OldPos = toCoord(str[0], str[1]);

	sf::Vector2f NewPos = toCoord(str[2], str[3]);


	for (int i = 0; i < ALL_FIGURES; i++)
	{
		if (figures[i].getPosition() == NewPos)//якщо позиція щойно переміщеної фігури дорівнює позиції якоїсь старої фігури то перемістити таку фігури за межі видимості
		{
			figures[i].setPosition(-100, -100);
		}
	}

	for (int i = 0; i < ALL_FIGURES; i++)
	{
		if (figures[i].getPosition() == OldPos)// замінити ту вігуру яка тільки шо переміслитась в нову клітинку з якої попередньо все видалеено
		{
			figures[i].setPosition(NewPos);
		}
	}

	//рокіровка

	if (str == "e1g1")//Рокіровка зі сторони короля БІЛІ фігури
		if (position.find("e1") == -1) 
			fmove("h1f1");

	if (str == "e8g8") //Рокіровка зі сторони короля Чорні фігури 
		if (position.find("e8") == -1) 
			fmove("h8f8");
	
	if (str == "e1c1") //Рокіровка з сторони королеви(довга рокировка ) БІЛІ фігури
		if (position.find("e1") == -1) 
			fmove("a1d1");
	
	if (str == "e8c8") //Рокіровка з сторони королеви(довга рокировка ) Чорні фігури
		if (position.find("e8") == -1) 
			fmove("a8d8");
}


void loadPosition()//функція для завантаження всіх фігур(їх текстур) на шахматну доску
{
	int k = 0;//count of all figures
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int n = board[i][j];//n фігура
			if (!n)// якщо n=0, то це значить шо це пуста клітинка на яку загружати текстуру не треба
			{
				continue;
			}

			int x = abs(n) - 1;
			int y = n > 0 ? 1 : 0;//тут вибирається чи темна чи світла фігура

			figures[k].setTextureRect(sf::IntRect(SIZE_OF_FIGURE * x, SIZE_OF_FIGURE * y, SIZE_OF_FIGURE, SIZE_OF_FIGURE));//вирізати з текстури прямокутник розміром 56*56 і в залежності від х і у просуватись вправо і вниз(вибір конкретної фігури з малюнка)
			figures[k].setPosition(SIZE_OF_FIGURE * j, SIZE_OF_FIGURE * i);//встановлює місце розташуваня фыгури від лівого верхнбого краю і аж до низу справ
			k++;
		}
	}


	for (int i = 0; i < position.length(); i += 5)
	{
		fmove(position.substr(i, 4));
	}
}

std::string note(sf::Vector2f& figure_move)
{
	std::string move_f = "";
	move_f += char(figure_move.x / SIZE_OF_FIGURE + 'a');//взнати букву ходу про горизонталі
	// на шахмітінй дошці букви вверху знаходяться від а до h; відповідно в нас кожна клітинка 56 на 56


	move_f += char(7 - (figure_move.y / SIZE_OF_FIGURE) + 49);//розділяєм поточну координату на розмір фігури шоб взнати кілтинку по вертикалі
	// і додаєм аски символ щоб отримати символьне значення числа

	return move_f;
}

void i_know_whats_like_to_lose(sf::RenderWindow& window)
{
	sf::Texture loss;
	loss.loadFromFile("chessparts/loss.jpg");

	sf::Sprite l(loss);

	while (window.isOpen())// головний ігорвий цикл
	{
		sf::Event event;

		sf::Vector2i pos = sf::Mouse::getPosition(window);//простий клас вектор для того щоб отримвти координати миші відносно користувацького вікна
		//getPosition відповідно повертає поточну позиції миші у вказаному в парамтрах вікні


		while (window.pollEvent(event))// у створеному нами вікні можуть вдібуватись різні події як от нажаття клавіши чи рухання мишкою і тд
		{

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					goto back;
				}
			}
		}

		window.clear();//в ОС Віндовс для того щоб намалювати щось треба взяти управління конектсом устройства тоді все стерти а тоді вивестти знове зображення

		window.draw(l);// ось де потрібна наша тектура шахматної дошки

		window.display();

	}

back:
	std::cout << "Out from loss window\n";

}


int play_vs_comp(sf::RenderWindow& window)
{
	position = "";//обнулити позицію

	//sf це пространстов имен всіх функцій бібліотеки SFML

	sf::Texture t1, t2;

	t1.loadFromFile("chessparts/figures.png");

	t2.loadFromFile("chessparts/chessboard.jpg");

	sf::Sprite s(t1);//претворення просто картинки шахматноъ доски на реальну текстуру яка буде використовуватись в грі.
	// Це важливо оксліьки якщо використовувати саму побітову текстуру то швидкість обробки значно збільшується

	sf::Sprite chessboard(t2);

	sf::Vector2f OldMove, NewMove;

	std::string move;
	/*char move[CHAR_MAX];*/

	for (int i = 0; i < 32; i++)
	{
		figures[i].setTexture(t1);
	}

	loadPosition();

	bool isMove = false;
	float dx = 0, dy = 0;
	int n = 0;

	loadIcon(window);

	while (window.isOpen())// головний ігорвий цикл
	{
		sf::Event event;

		sf::Vector2i pos = sf::Mouse::getPosition(window);//простий клас вектор для того щоб отримвти координати миші відносно користувацького вікна
		//getPosition відповідно повертає поточну позиції миші у вказаному в парамтрах вікні


		while (window.pollEvent(event))// у створеному нами вікні можуть вдібуватись різні події як от нажаття клавіши чи рухання мишкою і тд
		{
			if (event.type == event.Closed)//в данному випадку event походить на цикл оброботки сообщений в Виндовс.
			{// і можна провести паралелі між функцією PeekMessage та повідомленнями msg.message
				//window.close();
				
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					PlaySound("chesssounds/return_to_menu.wav", NULL, SND_ASYNC);
					goto here;
				}
			}


			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::BackSpace)
				{
					if (position.length() > 6)//якшо вже всі попередні ходи повернуті тобто строа вже не містить символів, то поверненяя назад просто не відбувається
					{
						position.erase(position.length() - 5, 5);
					}

					loadPosition();

				}
			}

			//пересовування фігур

			if (event.type == sf::Event::MouseButtonPressed)// якщо тип події яка відбувлась у вікні дорівнює коду того що кнопка миші була натиснута
			{
				if (event.key.code == sf::Mouse::Left)//якщо код кнорки який був натиснутий дорівнює коду лівої кнопки миші
				{
					for (int i = 0; i < 32; i++)
					{
						if (figures[i].getGlobalBounds().contains(pos.x, pos.y))//якщо миша знаходиться в межах створеної нами текстури
						{
							isMove = true;
							n = i;
							dx = pos.x - figures[i].getPosition().x;//присвоъти координатам тектури нову координаті, а сам ті в які ми текстури пересуваємо
							dy = pos.y - figures[i].getPosition().y;
							OldMove = figures[i].getPosition();
						}
					}
				}
			}


			if (event.type == sf::Event::MouseButtonReleased)// якщо кнопку миші відпустили то престати рухати тектуру
			{
				if (event.key.code == sf::Mouse::Left)
				{
					isMove = false;

					sf::Vector2f p = figures[n].getPosition() + sf::Vector2f(SIZE_OF_FIGURE / 2, SIZE_OF_FIGURE / 2);//діхнатись поточну позицію фігури яка переміщена

					NewMove = sf::Vector2f(SIZE_OF_FIGURE * int(p.x / SIZE_OF_FIGURE), SIZE_OF_FIGURE * int(p.y / SIZE_OF_FIGURE));//привести до типу int позицію щоб фігура знаходилась в межах умовних клітинок(розмір 56)


					move = note(OldMove) + note(NewMove);

					//треба зробити перевірку на легальні рухи

					

					if (OldMove != NewMove)
					{
						PlaySound("chesssounds/pawn_move.wav", NULL, SND_ASYNC);
						fmove(move);//подивитись чи в тій клітинці куди ми зробли хід нема інших фігур якшо є видалити і замінити на нову (останню фігуру яка туди походила)
						position += move + " ";
						std::cout << move << std::endl;
						figures[n].setPosition(NewMove);
						
					}
					else
					{
						std::cout << "FIgure didnt move!\n";
						continue;
					}
				}

				// підключення ИИ шахмат

				Sleep(500);

				move = getNextMove(position);

				OldMove = toCoord(move[0], move[1]);
				NewMove = toCoord(move[2], move[3]);

				for (int i = 0; i < ALL_FIGURES; i++)
				{
					if (figures[i].getPosition() == OldMove)
					{
						n = i;
					}
				}

				for (int k = 0; k < 50; k++)
				{
					sf::Vector2f p = NewMove - OldMove;

					figures[n].move(p.x / 50, p.y / 50);

					window.draw(chessboard);

					//for (int i = 0; i < ALL_FIGURES; i++) figures[i].move(offset);

					for (int i = 0; i < ALL_FIGURES; i++)
					{
						window.draw(figures[i]);
					}

					window.draw(figures[n]);

					//for (int i = 0; i < ALL_FIGURES; i++) figures[i].move(-offset);

					window.display();

				}

				//Sleep(500);
				PlaySound("chesssounds/pawn_move.wav", NULL, SND_ASYNC);
				fmove(move);
				std::cout << move << std::endl;
				position += move + " ";
				figures[n].setPosition(NewMove);
				

				std::string checkmate = getNextMove(position);//якщо після ходу комп'ютера більше нема кращіх ходів тоді гравець програв
				if (checkmate == "none")
				{
					i_know_whats_like_to_lose(window);
					std::cout << "You lost!";
					return 2;//code for lost
				}

			}

			if (isMove)
			{
				figures[n].setPosition(pos.x - dx, pos.y - dy);// присвохти текстурі нові координати
			}

		}

		/// тут відбувається все малювання
		window.clear();//в ОС Віндовс для того щоб намалювати щось треба взяти управління конектсом устройства тоді все стерти а тоді вивестти знове зображення

		window.draw(chessboard);// ось де потрібна наша тектура шахматної дошки


		for (int i = 0; i < 32; i++)
		{
			window.draw(figures[i]);
		}

		window.draw(figures[n]);

		window.display();//відобразити завповенене функцію window.draw вікно window
	}

here:
	//якщо користувач нажимає escape
	std::cout << "To main menu\n";
	//CloseConnection();
	return 1;//code for escape

}

int comp_vs_comp(sf::RenderWindow& window)
{

	position = "";//обнулити позицію

	Sleep(500);

	//sf це пространстов имен всіх функцій бібліотеки SFML

	sf::Texture t1, t2;

	t1.loadFromFile("chessparts/figures.png");

	t2.loadFromFile("chessparts/chessboard.jpg");

	sf::Sprite s(t1);//претворення просто картинки шахматноъ доски на реальну текстуру яка буде використовуватись в грі.
	// Це важливо оксліьки якщо використовувати саму побітову текстуру то швидкість обробки значно збільшується

	sf::Sprite chessboard(t2);

	sf::Vector2f OldMove, NewMove;

	std::string move;
	/*char move[CHAR_MAX];*/

	for (int i = 0; i < 32; i++)
	{
		figures[i].setTexture(t1);
	}

	loadPosition();

	bool isMove = false;
	float dx = 0, dy = 0;
	int n = 0;

	loadIcon(window);

	while (window.isOpen())// головний ігорвий цикл
	{
		sf::Event event;

		sf::Vector2i pos = sf::Mouse::getPosition(window);//простий клас вектор для того щоб отримвти координати миші відносно користувацького вікна
		//getPosition відповідно повертає поточну позиції миші у вказаному в парамтрах вікні


		while (window.pollEvent(event))// у створеному нами вікні можуть вдібуватись різні події як от нажаття клавіши чи рухання мишкою і тд
		{
			switch (event.type)
			{
			case  event.Closed://в данному випадку event походить на цикл оброботки сообщений в Виндовс.
			{// і можна провести паралелі між функцією PeekMessage та повідомленнями msg.message
				//window.close();
				window.close();
				exit(1);
			}break;
			case sf::Event::KeyPressed:
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
				{
					PlaySound("chesssounds/return_to_menu.wav", NULL, SND_ASYNC);
					goto here;
					break;
				}
				}
			}
			}
		}
			Sleep(500);

			move = getNextMove(position);

			OldMove = toCoord(move[0], move[1]);
			NewMove = toCoord(move[2], move[3]);

			for (int i = 0; i < ALL_FIGURES; i++)
			{
				if (figures[i].getPosition() == OldMove)
				{
					n = i;
				}
			}

			for (int k = 0; k < 50; k++)
			{
				sf::Vector2f p = NewMove - OldMove;

				figures[n].move(p.x / 50, p.y / 50);

				window.draw(chessboard);

				//for (int i = 0; i < ALL_FIGURES; i++) figures[i].move(offset);

				for (int i = 0; i < ALL_FIGURES; i++)
				{
					window.draw(figures[i]);
				}

				window.draw(figures[n]);

				//for (int i = 0; i < ALL_FIGURES; i++) figures[i].move(offset);

				window.display();

			}

			Sleep(500);
			
			fmove(move);
			std::cout << move << std::endl;
			position += move + std::string(" ");
			figures[n].setPosition(NewMove);
			PlaySound("chesssounds/pawn_move.wav", NULL, SND_ASYNC);
		

			std::string checkmate = getNextMove(position);//якщо після ходу комп'ютера більше нема кращіх ходів тоді гравець програв
			if (checkmate == "none")
			{
				i_know_whats_like_to_lose(window);
				std::cout << "Comp lost!";
				return 2;//code for loss
			}

		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{

		}*/


		//if (isMove)
		//{
		//	figures[n].setPosition(pos.x - dx, pos.y - dy);// присвохти текстурі нові координати
		//}



		/// тут відбувається все малювання
		window.clear();//в ОС Віндовс для того щоб намалювати щось треба взяти управління конектсом устройства тоді все стерти а тоді вивестти знове зображення

		window.draw(chessboard);// ось де потрібна наша тектура шахматної дошки


		for (int i = 0; i < 32; i++)
		{
			window.draw(figures[i]);
		}

		window.draw(figures[n]);

		window.display();//відобразити завповенене функцію window.draw вікно window
	}

here:
	std::cout << "To main menu\n";
	//CloseConnection();
	return 1;//code for escape
}


