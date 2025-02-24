
// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// standard
#include <iostream>
#include <sstream>
#include <array>

int main(int argc, char* args[])
{
	//
	// 1. lépés: inicializáljuk az SDL-t
	//

	// Állítsuk be a hiba Logging függvényt.
	SDL_LogSetPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_ERROR);
	// a grafikus alrendszert kapcsoljuk csak be, ha gond van, akkor jelezzük és lépjünk ki
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		// irjuk ki a hibat es termináljon a program
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[SDL initialization] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
	}

	// Miután az SDL Init lefutott, kilépésnél fusson le az alrendszerek kikapcsolása.
	// Így akkor is lefut, ha valamilyen hiba folytán lépünk ki.
	std::atexit(SDL_Quit);

	// hozzuk létre az ablakunkat
	const int WindowW = 800, WindowH = 600;
	SDL_Window* win = nullptr;
	win = SDL_CreateWindow("SDL EVENTS!",	// az ablak fejléce
		100,								// az ablak bal-felső sarkának kezdeti X koordinátája
		100,								// az ablak bal-felső sarkának kezdeti Y koordinátája
		WindowW,							// ablak szélessége
		WindowH,							// és magassága
		SDL_WINDOW_SHOWN);					// megjelenítési tulajdonságok


	// ha nem sikerült létrehozni az ablakot, akkor írjuk ki a hibát, amit kaptunk és lépjünk ki
	if (win == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Window creation] Error during the SDL initialization: %s", SDL_GetError());
		return 1;
	}

	//
	// 3. lépés: hozzunk létre egy renderelőt, rajzolót
	//

	SDL_Renderer* ren = nullptr;
	ren = SDL_CreateRenderer(	win,// melyik ablakhoz rendeljük hozzá a renderert
								-1,	// melyik indexű renderert inicializáljuk
									// a -1 a harmadik paraméterben meghatározott igényeinknek megfelelő első renderelőt jelenti
								SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);	// az igényeink, azaz
																						// hardveresen gyorsított és vsync-et beváró
	if (ren == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[Renderer creation] Error during the creation of an SDL renderer: %s", SDL_GetError());
		SDL_DestroyWindow(win);
		return 1;
	}

	//
	// 4. lépés: indítsuk el a fõ üzenetfeldolgozó ciklust
	// 

	bool quit = false;	// véget kell-e érjen a program futása?
	SDL_Event ev;		// feldolgozandó üzenet ide kerül

	Sint32 mouseX = 0, mouseY = 0; // egér X és Y koordinátái

	while (!quit)
	{
		// amíg van feldolgozandó üzenet dolgozzuk fel mindet:
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (ev.key.keysym.sym == SDLK_ESCAPE) quit = true;
				break;
			case SDL_MOUSEMOTION:
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
				break;
			case SDL_MOUSEBUTTONUP:
				// egérgomb felengedésének eseménye; a felengedett gomb a ev.button.button -ban található
				// a lehetséges gombok a következõek: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, 
				//		SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN
				break;
			}
		}

		// töröljük a hátteret fehérre
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);

		// aktuális rajzolási szín legyen zöld és rajzoljunk ki egy vonalat
		SDL_SetRenderDrawColor(ren,	// renderer címe, aminek a rajzolási színét be akarjuk állítani
			0,		// piros
			255,	// zöld
			0,		// kék
			255);	// átlátszatlanság

		SDL_RenderDrawLine(ren,	// renderer címe, ahová vonalat akarunk rajzolni
			0, 0, // vonal kezdõpontjának (x,y) koordinátái
			mouseX, mouseY);// vonal végpontjának (x,y) koordinátái

		// definiáljunk egy (mouseX, mouseY) középpontó, tengelyekkel párhuzamos oldalú
		// 20x20-as négyzetet:
		SDL_Rect cursor_rect;
		cursor_rect.x = mouseX - 10;
		cursor_rect.y = mouseY - 10;
		cursor_rect.w = 20;
		cursor_rect.h = 20;
		// legyen a kitöltési szín piros
		SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
		SDL_RenderFillRect(ren, &cursor_rect);

		// 1. feladat: az eltelt idõ függvényében periodikusan nõjjön és csökkenjen
		//    az egérmutató középpontjával kirajzolt négyszög

		// 2. feladat: ha a user a bal egérgombot nyomja meg akkor a téglalap színe váltson pirosra,
		//    ha a jobb egérgombot, akkor kékre

		// 3. beadható feladat: rajzolj ki egy 50 sugarú körvonalat az egérmutató köré!
		// segítség: használd a SDL_RenderDrawLines()-t

		// jelenítsük meg a backbuffer tartalmát
		SDL_RenderPresent(ren);
	}

	//
	// 5. lépés: lépjünk ki
	//

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);

	return 0;
}
