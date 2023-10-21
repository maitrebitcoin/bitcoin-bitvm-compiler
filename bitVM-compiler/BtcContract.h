#pragma once
// code tree after source code parsing

#include <string>
#include <vector>


// type of a variable or function. 
//ex : "bool"
class Type {
public:
	enum class Native {
		undefined,	// not set/ invalide
		bit,		// bool
		int8,		// char
		uint8,		// byte / unsigned char
		int64,		// long long
		uint64,		// unsigned long long
		uint256,	// unsigned __int256
	};
protected:
	Native native_type = Native::undefined;
public:
	// constructor
	Type(Native t) : native_type(t) {}

	// return the size in bits of the type
	int size_in_bit(void) const;

};


class Operand;
// Math expression. ex :"a+2"
class Expression {
protected:
	// type of the result
	Type result_type;
	// opération
	enum class Operation {
		op_and,
		op_or,
		op_not,
	};
	Operation operation;
	// left operand
	Operand* left_operand;
	// right operand
	Operand* right_operand;
	// expression as  string for debug purposes
	std::string expression_for_debug;
};


// base class for a statement. 
// ex "v=a+2" or "return a+2;"
class StatementBase {
protected:

};
// "return" statement
class Statement_Return : public StatementBase {
protected:
	// expression to return
	Expression expression;

};


// represents a function, ex : bool main(bool a, bool b) { return a & b; }
class Function {
public:
	// parameters of the function
	struct Parameter {
		Type type;
		std::string name;
		// constructor
		Parameter(Type t, std::string n) : type(t), name(n) {}
	};
	// Definition of the function
	struct Definition {
		// name of the function
		std::string name;
		// return type of the function
		Type return_type;
		// parameters of the function
		std::vector<Parameter> parameters;
		// constructor
		Definition(Type t, std::string n);
	};

protected:
	// definiton of the function
	Definition definition;
	// body of the function
	std::vector<StatementBase*> body;

public:
	// return the number of bits needed for the input parameters
	int size_in_bit_input(void) const;
	// return the number of bits needed to store the return value
	int size_in_bit_output(void) const;

};




// logical representation of de contract 
class BitContract {

};

