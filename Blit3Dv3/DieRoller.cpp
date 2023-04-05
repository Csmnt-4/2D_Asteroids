#include "DieRoller.h"
#include <iostream>

DiceRoller::DiceRoller() {
	int seedVal;
	std::random_device rd;					// "a good unpredictable sequence starter" (c.)
	seedVal = rd();
	rng.seed(seedVal);
}

void DiceRoller::SeedRNG(unsigned int seedVal) {
	rng.seed(seedVal);						// just replaces the generated seed with a provided one
}

int DiceRoller::Roll1DN(int N) { // now accepts negative values
	if (N == 0) {
		return 0;
	}
	else if (N < 0) {
		std::uniform_int_distribution<int> DNRoll(1, -N);
		return -DNRoll(rng);
	}
	else {
		std::uniform_int_distribution<int> DNRoll(1, N);
		return DNRoll(rng);
	}
}

int DiceRoller::Roll6D3() {					// rolls 6 three-sided dices and sums them
	std::uniform_int_distribution<int> D6Roll(1, 3);
	return D6Roll(rng) + D6Roll(rng) + D6Roll(rng) + D6Roll(rng) + D6Roll(rng) + D6Roll(rng);
}

int DiceRoller::Roll3D6() {					// rolls 3 six-sided dice and sums them 3
	std::uniform_int_distribution<int> D6Roll(1, 6);
	return D6Roll(rng) + D6Roll(rng) + D6Roll(rng);
}

int DiceRoller::Roll4D6DropLowest() {
	std::uniform_int_distribution<int> D6Roll(1, 6);
	int roll[4];
	int sum = 0;
	for (int i = 0; i < 4; i++) {
		roll[i] = D6Roll(rng);
		if (i > 0)
			if (roll[i] > roll[i - 1])
				sum += roll[i];
			else
				sum += roll[i - 1];
	}
	return sum;
}

void DiceRoller::PrintDiceN6(int number) {
	switch (number) {
	case 1: {
		std::cout << "\t#########\n";
		std::cout << "\t#       #\n";
		std::cout << "\t#   0   #\n";
		std::cout << "\t#       #\n";
		std::cout << "\t#########\n";
	}
		  break;
	case 2: {
		std::cout << "\t#########\n";
		std::cout << "\t# 0     #\n";
		std::cout << "\t#       #\n";
		std::cout << "\t#     0 #\n";
		std::cout << "\t#########\n";
	}
		  break;
	case 3: {
		std::cout << "\t#########\n";
		std::cout << "\t# 0     #\n";
		std::cout << "\t#   0   #\n";
		std::cout << "\t#     0 #\n";
		std::cout << "\t#########\n";
	}
		  break;
	case 4: {
		std::cout << "\t#########\n";
		std::cout << "\t# 0   0 #\n";
		std::cout << "\t#       #\n";
		std::cout << "\t# 0   0 #\n";
		std::cout << "\t#########\n";
	}
		  break;
	case 5: {
		std::cout << "\t#########\n";
		std::cout << "\t# 0   0 #\n";
		std::cout << "\t#   0   #\n";
		std::cout << "\t# 0   0 #\n";
		std::cout << "\t#########\n";
	}
		  break;
	case 6: {
		std::cout << "\t#########\n";
		std::cout << "\t# 0 0 0 #\n";
		std::cout << "\t#       #\n";
		std::cout << "\t# 0 0 0 #\n";
		std::cout << "\t#########\n";
	}
		  break;
	default: {
		std::cout << "\t#########\n";
		std::cout << "\t#       #\n";
		std::cout << "\t#       #\n";
		std::cout << "\t#       #\n";
		std::cout << "\t#########\n";
	}
		   break;
	}
}