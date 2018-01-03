#pragma once
#include <iostream>
#include <string>
#include <list>
#include "PageTable.h"
#include <queue>
#include "ExchangeFile.h"
#include <stack>
class RAM
{
private:
public:

	char *ram = new char[256];
	Ramka *framesTable = new Ramka;
	std::string *processNameInFrame = new std::string[16]; //NO NIBY SPOKO ZMIENNA, ALE POWINIENEM TO CHYBA POBIERA� OD ADAMA
	std::queue<int> FIFO;
	ExchangeFile exchangeFile;
	//tabela stron
	std::vector<PageTable> pageTables;
	std::stack<int> freeFrames;

	// konstruktor, ustawienie podstawowych parametrow
	RAM() {
		for (int i = 0; i < 16; i++) {
			processNameInFrame[i] = "";
			for (int j = 0; j < 16; j++)
				ram[i * j] = ' ';
			freeFrames.push(i);
			inicjalizacja_tabeli_ramek();
		}
	}

	//operacje na ramkach

	void inicjalizacja_tabeli_ramek() //inicjalizuje liste ramek aby zawiera�a odpowiednie wskazania na komorki pamieci
	{
		for (int i = 0; i < 16; i++)
		{
			Ramka ramka;
			ramka.adres_poczatkowy = i * 16;
			ramka.adres_koncowy = i * 16 + 16 - 1;
			ramka.nr_ramki = i;
			framesTable[i] = ramka;
		}
	}

	std::string odczytaj_ramk�(int ramka) //odczytuje ramk� o zadanym numerze i zwraca jej zawarto�� w stringu
	{
		char *bufor = new char[16]; //tworz� bufor na odczytane dane

		for (int i = 0; i < 16; i++)
		{
			bufor[i] = ram[ramka * 16 + i]; //odczytuj� ca�� ramk�
		}

		std::string *zwracanie = new string(bufor); //tworz� stringa z tablicy char�w (by�y problemy z funkcj� ToString)
		return bufor; //zwracam stringa
	}

	void zapisz_ramk�(int numer_ramki, string znaki) //zapisuje znaki w konkretnej ramce (odpowiadaj�cej dostarczonemu segmentowi); nie zmienia bitu u�ywania na true; jest to zmieniane przy wyszukiwaniu wolnej ramki
	{
		int adres = numer_ramki * 16;

		for (int i = 0; i < znaki.size(); i++)
		{
			ram[adres + i] = znaki[i];
		}
	}


	//przydzielanie pami�ci

	int ramka_ze_stosu()
	{
		int ramka = freeFrames.top();
		freeFrames.pop();
		framesTable[ramka].bit_odniesienia = true;
		return ramka;
	}

	void zwolnij_ramk�(int ramka)
	{
		freeFrames.push(ramka); //oznaczam ramk� jako woln�
								//z kolejki FIFO musz� teraz usun�� t� ramk�:
		std::queue<int> bufor; //tworz� tymczasow� kolejk� pomocnicz�
		while (FIFO.size() > 0)
		{
			if (FIFO.front() != ramka)
				bufor.push(FIFO.front());
			FIFO.pop();
		}
		// odwracam kolejk�
		while (FIFO.size() > 0)
		{
			FIFO.push(bufor.front());
			bufor.pop();
		}
	}

	int ktora_ramke_zwolnic()
	{
		int pierwsza_ramka = FIFO.front();
		if (framesTable[pierwsza_ramka].bit_modyfikacji == 0 && framesTable[pierwsza_ramka].bit_odniesienia == 0)
			return pierwsza_ramka;
		else
		{
			framesTable[pierwsza_ramka].bit_odniesienia = 0;
			FIFO.push(pierwsza_ramka);
			FIFO.pop();
			while (FIFO.front() != pierwsza_ramka)
			{
				if (framesTable[FIFO.front()].bit_odniesienia == 0 && framesTable[FIFO.front()].bit_modyfikacji == 0) return FIFO.front();
				else
				{
					framesTable[FIFO.front()].bit_odniesienia = 0;
					FIFO.push(FIFO.front());
					FIFO.pop();
				}
			}
			while (FIFO.front() != pierwsza_ramka)
			{
				if (framesTable[FIFO.front()].bit_odniesienia == 0) return FIFO.front();
				else
				{
					framesTable[FIFO.front()].bit_odniesienia = 0;
					FIFO.push(FIFO.front());
					FIFO.pop();
				}
			}
		}
	}

	void strona_w_ramke(int nrStrony, std::string procName)
	{
		if (freeFrames.size() <= 0)
			zwolnij_ramk�(ktora_ramke_zwolnic());
		else
			for (int i = 0; i < pageTables.size(); i++)
			{
				if (pageTables[i].processName == procName)
				{
					pageTables[i].framesNumber[nrStrony] = freeFrames.top();
					pageTables[i].inRAM[nrStrony] = true;
					freeFrames.pop();
				}
			}

	}

	std::string getCommand(int programCounter, std::string processName, PageTable pt)
	{
		int jakaszmiennaktoraprzechowujeidprocesu;
		int pageIndex;
		if ((programCounter + 1) % 16 == 0) pageIndex = ((programCounter + 1) / 16) - 1;
		else pageIndex = ((programCounter + 1) / 16);

		if (pageTables[jakaszmiennaktoraprzechowujeidprocesu].inRAM[pageIndex])
		{
			return odczytaj_ramk�(pageTables[jakaszmiennaktoraprzechowujeidprocesu].getPositionInRam);
		}

		else strona_w_ramke(pageIndex, pageTables[jakaszmiennaktoraprzechowujeidprocesu].processName);
	}

	//char getCommand(int programCounter, std::string processName) {
	//	// szukany znak
	//	char search;
	//	// index tablicy stron szukanego procesu w kolekcji pagetables
	//	int pageTableIndex = -1;
	//	// szukanie tablicy stron z odpowiednim procesem
	//	for (int i = 0; i < pageTables.size(); i++) {
	//		if (pageTables[i].processName == (processName))
	//			pageTableIndex = i;
	//	}

	//	// wyznaczenie nr strony ktora musi byc w ramie przed zwroceniem
	//	// odpowiedniego znaku
	//	int pageNumber = programCounter / 16;

	//	// sprawdzenie w ktorej ramce sa dane, zwraca -1 gdy nie ma w ramie
	//	// odpowiedniej strony
	//	int framePosition = pageTables[pageTableIndex].getPositionInRam(pageNumber);

	//	// zaladowanie strony do ramu jesli jej tam nie ma
	//	if (framePosition == -1) {
	//		std::string dane = exchangeFile.getFrame(processName, pageNumber);

	//	}
	//}

	// usuwanie danego procesu z pamieci
	void deleteProcessData(std::string processName) {

		// usuniecie danych z pamieci ram
		// ??? Do poprawienia
		for (int i = 0; i < 16; i++) {
			if (processNameInFrame[i] == (processName)) {
				FIFO.push(i);
				for (int j = 0; j < pageTables.size(); j++) {
					if (pageTables[j].processName == (processName))
						pageTables.erase[j];
				}

				for (int j = 0; j < 16; j++)
					ram[i * 16 + j] = ' ';
			}
		}

		// usuniecie danych z pliku wymiany
		/*exchangeFile.deleteData(processName, int ilosc_stron, int indexstrony);*/
	}

	// wypisanie procesow bedacych w pamieci
	void writeProcessesNamesInRam() {
		for (int i = 0; i <pageTables.size(); i++)
			std::cout << pageTables[i].processName;
	}

	void memoryContent(int adres, int rozmiar) //coutowa� wszystko
	{
		for (adres; adres < adres + rozmiar; adres++)
		{
			std::cout << adres[ram];
		}
	}

};