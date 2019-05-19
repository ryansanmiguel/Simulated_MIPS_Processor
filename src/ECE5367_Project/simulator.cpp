#include "simulator.h"

using namespace std;

// ___Init: non-zero initial memory/register values
// ___Init[numMemData][2]
// ___Init[i][j]
// i: index in memory/register
// j: data value
Sim::Sim() {
	for (int i = 0; i < 250; i++)
		memory[i] = 0;

	for (int i = 0; i < 32; i++)
		registers[i] = 0;
	
	instructions = vector<string>();
	string instReg = "";
	PC = 0;
	cycleNum = 1;
	instNum = 1;
}

void Sim::run() {
	init();

	while (PC < instructions.size()) {
		IF();
		instNum++;
	}

	printRegisters();
	printMemory();
	fout.close();
}

// TODO: Save ofname
void Sim::init() {
	string ifname;
	string ofname;

	cout << "Enter the input and output file names (including the file extensions and separated by a space): ";
	cin >> ifname >> ofname;

	ifstream fin(ifname);
	fout.open(ofname);

	if (fin.is_open()) {
		string line;
		getline(fin, line);

		// Registers Init
		if (line.compare("REGISTERS") == 0)
			while (getline(fin, line)) {
				if (line.compare("MEMORY") == 0)
					break;

				string regName;
				int contents;

				istringstream iss(line);
				iss >> regName >> contents;

				regName.erase(0, 1);
				int regNum = stoi(regName);

				registers[regNum] = contents;
			}
		else
			cout << "ERROR: Initial register contents not found";

		// Memory Init
		if (line.compare("MEMORY") == 0)
			while (getline(fin, line)) {
				if (line.compare("CODE") == 0)
					break;

				int addr;
				int contents;

				istringstream iss(line);
				iss >> addr >> contents;

				if (addr % 4 == 0)
					addr = addr / 4;	// For this simulation, memory is word addressable
				else
					cout << "ERROR: Invalid memory address (" << addr << ")" << endl;

				memory[addr] = contents;
			}
		else
			cout << "ERROR: Initial memory contents not found" << endl;

		// Get Instructions
		if (line.compare("CODE") == 0)
			while (getline(fin, line)) {
				instructions.push_back(line);
			}
		else
			cout << "ERROR: Instructions not found" << endl;
	}
	fin.close();
}

void Sim::IF() {
	fout << "C#" << cycleNum << " ";
	fout << "I" << instNum << "-" << "IF" << endl;

	IR = instructions[PC];
	PC++;

	cycleNum++;
	ID();
}

void Sim::ID() {
	fout << "C#" << cycleNum << " ";
	fout << "I" << instNum << "-" << "ID" << endl;

	string op = IR.substr(0, 6);
	int rs = stoi(IR.substr(6, 5), nullptr, 2); // stoi() converts binary string to int
	int rt = stoi(IR.substr(11, 5), nullptr, 2);

	// R-Type Instruction
	if (op.compare("000000") == 0) {
		int rd = stoi(IR.substr(16, 5), nullptr, 2);
		int shamt = stoi(IR.substr(21, 5), nullptr, 2);
		string funct = IR.substr(26, 6);

		if (funct.compare("100000") != 0 &&
			funct.compare("100010") != 0 &&
			funct.compare("101010") != 0)
			cout << "ERROR: Instruction not recognized" << endl;

		cycleNum++;
		RType inst = RType(op, rs, rt, rd, shamt, funct);
		EX(inst);
	}
	// I-Type Instruction
	else {
		string key = "";
		
		string binOffset = IR.substr(16, 16);
		bool isNegative = (binOffset[0] == '1');
		
		if (isNegative) {
			int len = binOffset.length();
			
			for (int i = 0; i < len; i++)
				(binOffset[i] == '0') ? (binOffset[i] = '1') : (binOffset[i] = '0');

			for (int i = len - 1; i >= 0; i--)
				if (binOffset[i] == '1')
					binOffset[i] = '0';
				else {
					binOffset[i] = '1';
					break;
				}
		}
		
		int offset = stoi(binOffset, nullptr, 2);

		if (isNegative)
			offset *= -1;

		if (op.compare("000100") == 0) // BEQ
			key = "Branch";
		else if (op.compare("000101") == 0) // BNE
			key = "Branch";
		else if (op.compare("001000") == 0) // ADDI
			key = "Reg-Imm ALU";
		else if (op.compare("100011") == 0) // LW
			key = "Mem Ref";
		else if (op.compare("101011") == 0) // SW
			key = "Mem Ref";
		else
			cout << "ERROR: Instruction not recognized" << endl;

		cycleNum++;
		IType inst = IType(key, op, rs, rt, offset);

		EX(inst);
	}
}

void Sim::EX(RType inst) {
	fout << "C#" << cycleNum << " ";
	fout << "I" << instNum << "-" << "EX" << endl;

	string funct = inst.getFunct();
	int rs = registers[inst.getRS()]; // rs represents the contents of the sourse register, not the register number
	int rt = registers[inst.getRT()]; // Likewise, rt represents the contents of the register
	int rd = inst.getRD(); // Whereas rd represents the register number
	int aluOut;

	// Reg-Reg ALU
	if (funct.compare("100000") == 0) // ADD
		aluOut = rs + rt;
	else if (funct.compare("100010") == 0) // SUB
		aluOut = rs - rt;
	else // SLT
		(rs < rt) ? (aluOut = 1) : (aluOut = 0);

	cycleNum++;
	WB(rd, aluOut);
}

void Sim::EX(IType inst) {
	fout << "C#" << cycleNum << " ";
	fout << "I" << instNum << "-" << "EX" << endl;

	string key = inst.getKey();
	string op = inst.getOP();
	int rs = registers[inst.getRS()]; // rs represents the contents of the sourse register, not the register number
	int rt = inst.getRT(); // Whereas rt represents the register number
	int offset = inst.getOffset();
	int aluOut;
	bool condition = false;

	if (key.compare("Reg-Imm ALU") == 0) {
		aluOut = rs + offset;
		
		cycleNum++;
		WB(rt, aluOut);
	}
	else if (key.compare("Mem Ref") == 0) {
		aluOut = (rs + offset) / 4;

		cycleNum++;
		MEM(inst, aluOut);
	}
	else {
		rt = registers[rt];
		
		if (op.compare("000100") == 0) {
			if (rs == rt) // BEQ
				PC += offset;
		}
		else // BNE
			if (rs != rt)
				PC += offset;

		cycleNum++;
	}
}

void Sim::MEM(IType inst, int aluOut) {
	fout << "C#" << cycleNum << " ";
	fout << "I" << instNum << "-" << "MEM" << endl;

	string op = inst.getOP();
	int rt = inst.getRT();
	
	if (op.compare("100011") == 0) {
		int val = memory[aluOut]; // LW

		cycleNum++;
		WB(rt, val);
	}
	else {
		memory[aluOut] = registers[rt]; // SW

		cycleNum++;
	}
}

void Sim::WB(int reg, int val) {
	fout << "C#" << cycleNum << " ";
	fout << "I" << instNum << "-" << "WB" << endl;
	
	registers[reg] = val;

	cycleNum++;
}

void Sim::printMemory() {
	fout << "MEMORY" << endl;

	for (int i = 0; i < 250; i++)
		if (memory[i] != 0)
			fout << (i * 4) << " " << memory[i] << endl;
}

void Sim::printRegisters() {
	fout << "REGISTERS" << endl;
	
	for (int i = 1; i < 32; i++)
		if (registers[i] != 0)
			fout << "R" << i << " " << registers[i] << endl;
}

void Sim::printInstructions() {
	cout << endl << "INSTRUCTIONS" << endl;

	vector<string>::iterator inst;
	for (inst = instructions.begin(); inst != instructions.end(); inst++)
		cout << *inst << endl;
}
