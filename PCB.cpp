#include "PCB.h"

//Drukowanie zawartości pola PCB
void PCB::print() {
	std::cout << "Proces: " << name << ", identyfikator: " << ID << ", rejestry A: " << A << ", B: " << B << ", C: " << C << ", D: " << D << ", command counter: " << commandCounter << std::endl;
}
//Uspyianie procesu - pole blocked uniemożliwia ustawienie stanu ready
void PCB::sleep() {
	//this->state = PCB::processState::waiting;
	//this->blocked = 1;
}
//Budzenie procesu
void PCB::wakeup() {
	//this->blocked = 0;

}
