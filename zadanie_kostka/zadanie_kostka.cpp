/*==================================================================/
Program obracający kostkę (sześcian) przy użyciu biblioteki OpenCV 	/
Autor: Bartosz Pacuła												/
Styczeń 2019r.														/
===================================================================*/
// zadanie_kostka.cpp
//

#include "pch.h"

#include "kostka.hpp"  //zawiera główne klasy i funkcje obliczeniowe

using namespace kostka;

int main()
{
	double alfa, beta, gamma; //kąty odpowiednio dla osi x, y, z
	char klawisz;
	Mat obraz; //obraz wyjściowy kostki

	Szescian sz; //utworzenie sześcianu

	//wartości początkowe kątów
	alfa = 30.0; //w stopniach (obrót względem osi x) 
	beta = 30.0; //w stopniach (obrót względem osi y)
	gamma = 0.0; //w stopniach (obrót względem osi z)

	o_programie(); //wyświetlenie informacji o programie

	obraz = updateView(sz, alfa, beta, gamma); //aktualizacja obrazu (obrót kostki o kąt alfa, beta lub gamma i utworzenie obrazu; kąty wyrażone w stopniach)
	imshow("Kostka v.1.0 by Bartosz Pacuła -klawisze a-z: obrót wzg. x, s-x: wzg. y, d-c: wzg. z, q: quit", obraz); //początkowe wyświetlenie obrazu

	for(;;)
	{
		klawisz=waitKey(0); //pobiera klawisz sterujący

		switch(klawisz)
		{
			//dodano też duże litery na wypadek, jeśli ktoś ma wciśnięty CapsLock
			case 'q': 
			return 0; break; //wyjście z programu
			
			case 'Q':
			return 0; break; //wyjście z programu

			case 'a': 
			alfa+=5; break;
			
			case 'A':
			alfa+=5; break;

			case 'z': 
			alfa-=5; break;
			
			case 'Z':
			alfa-=5; break;

			case 's': 
			beta+=5; break;
			
			case 'S':
			beta+=5; break;

			case 'x': 
			beta-=5; break;
			
			case 'X':
			beta-=5; break;

			case 'd': 
			gamma+=5; break;
			
			case 'D':
			gamma+=5; break;

			case 'c': 
			gamma-=5; break;
			
			case 'C':
			gamma-=5; break;

			default: break;
		}
		
		obraz = updateView(sz, alfa, beta, gamma); //aktualizacja obrazu (obrót kostki o kąt alfa, beta lub gamma i utworzenie obrazu; kąty wyrażone w stopniach)
		imshow("Kostka v.1.0 by Bartosz Pacuła -klawisze a-z: obrót wzg. x, s-x: wzg. y, d-c: wzg. z, q: quit", obraz);
	}

	return 0;
}
