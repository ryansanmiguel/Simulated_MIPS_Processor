#pragma once

#include <iostream>

class Instruction {
protected:
	std::string op;
	int rs;
	int rt;
public:
	Instruction() {
		op = "";
		rs = -1;
		rt = -1;
	}
	Instruction(std::string op, int rs, int rt) {
		this->op = op;
		this->rs = rs;
		this->rt = rt;
	};
	std::string getOP() { return op; };
	int getRS() { return rs; };
	int getRT() { return rt; };
};

class RType : public Instruction {
private:
	int rd;
	int shamt;
	std::string funct;
public:
	RType() {
		Instruction();
		op = "";
		rs = -1;
		rt = -1;
		rd = -1;
		shamt = -1;
		funct = "";
	}
	RType(std::string op,
		  int rs, int rt, int rd,
		  int shamt,
		  std::string funct) {
		
		this->op = op;
		this->rs = rs;
		this->rt = rt;
		this->rd = rd;
		this->shamt = shamt;
		this->funct = funct;
	};
	std::string getOP() { return op; };
	int getRS() { return rs; };
	int getRT() { return rt; };
	int getRD() { return rd; };
	int getShamt() { return shamt; };
	std::string getFunct() { return funct; };
};

class IType : public Instruction {
private:
	std::string key;
	int offset;
public:
	IType() {
		Instruction();
		key = "";
		op = "";
		rs = -1;
		rt = -1;
		offset = 0;
	}
	IType(std::string key,
		  std::string op,
		  int rs, int rt,
		  int offset) {
		
		this->key = key;
		this->op = op;
		this->rs = rs;
		this->rt = rt;
		this->offset = offset;
	};
	std::string getKey() { return key; };
	std::string getOP() { return op; };
	int getRS() { return rs; };
	int getRT() { return rt; };
	int getOffset() { return offset; };
};
