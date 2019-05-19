#include "simulator.h"

using namespace std;

int main() {
	while (1) {
		Sim sim = Sim();
		sim.run();

		char cont = 'N';
		cout << "Would you like to run another simulation? (Enter 'Y' or 'N') ";
		cin >> cont;

		if (cont == 'N')
			break;
	}
	return 0;
}
