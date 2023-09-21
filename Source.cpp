#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "pista.h"
#include "Entity.h"
#include "Avatar.h"
#include "Beast.h"

using namespace std;
using namespace chrono_literals;


int i, j;
int length, width;
int x, y;

int main() {
	int maxP;								//Maximum amount of players(Werewolfs,Vampires)
	int countV;								//countW = Alive Vampires
	int countW;								//countV = Alive Werewolfs
	srand((unsigned)time(NULL));
	
	do {
		std::cout << "Enter Dimetions" << std::endl;
		std::cout << "Please use numbers highr than 6" << std::endl;
		std::cout << "Enter Length" << std::endl;
		std::cin >> length;
		std::cout << "Enter Width" << std::endl;
		std::cin >> width;
	} while (length < 6 || width < 6);

	maxP = (length * width) / 30;			
	countV = maxP;							
	countW = maxP;							
	pista pista(length, width);				//Constractor creates map with random tree and lakes 
	Beast* werewolf = new Beast[countW];	//Creating array for werewolf
	for (i = 0; i < countV; i++) {
		werewolf[i].kind = 'W';
		werewolf[i].enter(pista);			//Werewolfs random entering map 
	}
	Beast* vampire = new Beast[countV];		//Creating array for vampires
	for (i = 0; i < countW; i++) {
		vampire[i].kind = 'V';				
		vampire[i].enter(pista);			//Vampires random entering map 

	}
	Avatar avatar;
	std::cout << "Choose side Vampires or Werewolfs" << std::endl;
	std::cout << "Press V for Vampires" << std::endl << "Press W for Werewols" << std::endl;
	std::cin >> avatar.team;
	while (avatar.team != 'V' && avatar.team != 'W') {
		std::cout << "Please chose V Or W"<<std::endl;
		std::cin >> avatar.team;		
	}
	avatar.enter(pista);						//Avatar entering map

	std::cout << "\t" << "Istructions" << std::endl;
	std::cout << "Move Using the arrows on your keybord" << std::endl;
	std::cout << "Press shift if you want yo use the potion" << std::endl;
	std::cout << "Press the spacebar if you want to Pause the game" << std::endl;
	std::cout << "Press Enter to Begin" << std::endl;
	getchar();

	pista.clearmap();
	pista.printmap();
	while (countW > 0 && countV > 0) {		//while at least 1 of each kind is alive
		this_thread::sleep_for(1s);			
		avatar.A_move(pista);				//Reading keybord state avatar moves with arrows

		for (i = 0; i < maxP; i++) {		
			if (werewolf[i].health > 0) {	//only for the alive werewolfs
				switch (werewolf[i].neighboor(pista)) {  //is an unit next to this one?
				case 0:										//No one is around so move randomly
					werewolf[i].move(pista, (rand() % 4));
					break;
				case 2:									//unit under this
					x = werewolf[i].length + 1;
					y = werewolf[i].width;
					break;
				case 4:									//unit on the left side
					x = werewolf[i].length;
					y = werewolf[i].width - 1;
					break;
				case 6:									//unit on the rigth
					x = werewolf[i].length;
					y = werewolf[i].width + 1;
					break;								
				case 8:									//unit above
					x = werewolf[i].length - 1;
					y = werewolf[i].width;
					break;
				}
				if (pista.map[x][y] == 'V') {			//if the unit is hostile
					for (j = 0; j < maxP; j++) {
						if (vampire[j].length == x && vampire[j].width == y) { //find the hostile unit in the array  
							werewolf[i].hit(&vampire[j]);						//attempt to attck
							if (vampire[j].health <= 0) {						//If hostile usit is dead
								pista.death(vampire[j].length, vampire[j].width); //remove unit from th map
								countV--;											//Decreasing the total 
							}
						}
					}
				}
				else if (pista.map[x][y] == 'W') {							//if the unit is frindly
					for (j = 0; j < maxP; j++) {							//find frindly ynit in the array
						if (werewolf[i].health > werewolf[j].health) {		//Need healing ?
							if (rand() % 2)									
								werewolf[i].heal(&werewolf[j]);				//healing teamate
						}
					}
				}
			}
		}
		// Same process for the Vampires
		for (i = 0; i < maxP; i++) {
			if (vampire[i].health > 0) {
				switch (vampire[i].neighboor(pista)) {
				case 0:
					vampire[i].move(pista, (rand() % 4));
					break;
				case 2:
					x = vampire[i].length + 1;
					y = vampire[i].width;
					break;
				case 4:
					x = vampire[i].length;
					y = vampire[i].width - 1;
					break;
				case 6:
					x = vampire[i].length;
					y = vampire[i].width + 1;
					break;
				case 8:
					x = vampire[i].length - 1;
					y = vampire[i].width;
					break;
				}
				if (pista.map[x][y] == 'W') {
					for (j = 0; j < maxP; j++) {
						if (werewolf[j].length == x && werewolf[j].width == y) {
							vampire[i].hit(&werewolf[j]);
							if (werewolf[j].health <= 0) {
								pista.death(werewolf[j].length, werewolf[j].width);
								countW--;
							}
						}
					}
				}
				else if (pista.map[x][y] == 'V') {
					for (j = 0; j < maxP; j++) {
						if (vampire[i].health > vampire[j].health) {
							if (rand() % 2)
								vampire[i].heal(&vampire[j]);
						}
					}
				}

			}
		}

		this_thread::sleep_for(300ms);
		if (avatar.potion>0) {							//any potions left?
			if (avatar.team = 'W' && pista.day == 0)	//if suporting during Werelofs during the day
				avatar.PotionUse(werewolf, maxP);		//Read keypord for potion
			else if (avatar.team = 'V' && pista.day == 1) // same for vamparies
				avatar.PotionUse(vampire, maxP);
		}
		pista.clearmap();									//Clear screen
		this_thread::sleep_for(200ms);						
		avatar.pause(countW, countV);						//read keybord for pausing
		pista.printmap();									//printig updated map
	}
	//Loop End
	pista.clearmap();
	if (countV > countW) //Who won?
		cout << "VAMPIRES WON";
	else
		cout << "WEREWOLF WON";
	//Deleting allocated memory
	delete[] werewolf;
	delete[] vampire;
	for (i = 0; i < length; i++)
		delete[] pista.map[i];
	delete pista.map;

}
