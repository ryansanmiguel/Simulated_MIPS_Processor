#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "instruction.h"

class Sim {
private:
	int memory[250];
	int registers[32];
	std::vector<std::string> instructions;
	std::string IR;
	int PC;
	int cycleNum;
	int instNum;
	std::ofstream fout;

	void init();
	void IF();
	void ID();
	void EX(RType inst);
	void EX(IType inst);
	void MEM(IType inst, int aluOut);
	void WB(int reg, int val);
public:
	Sim();
	void run();
	void printMemory();
	void printRegisters();
	void printInstructions();
};
