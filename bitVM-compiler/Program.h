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
	Type(void) {}
	Type(Native t) : native_type(t) {}
	Type(const Type& source ) : native_type(source.native_type) {}

	// return the size in bits of the type
	int size_in_bit(void) const;

};

// opéran din a expression. "a" or 123
class Operand {

};
// ex: 123
class Literal : public Operand {
public:
	// type of the literal
	Type type;
	// value of the literal
	std::string value;
public:
	// constructor
	Literal(Type t, std::string v) : type(t), value(v) {}
};
class Variable : public Operand {
public:
	// type of the variable
	Type type;
	// name of the variable
	std::string name;
public:
	// constructor
	Variable(std::string n) : type(Type::Native::undefined), name(n) {}
};

class BinaryOperation;
class Expression {
protected:
	// type of the result
	Type result_type;
public:

};

// Simple expression, ex : "a" or "123"
class SimpleExpression : public Expression {
protected:
	// if the expression a variable or littéral
	Operand* operand = nullptr;
public:
	// ex : a
	SimpleExpression(Operand* op) : operand(op) {}
};

// Math expression. ex :"a+2"
class BinaryOperation : public Expression {
public:
	// type of the result
	Type result_type;
	// opération : & + - * / % | & ^ 
	//  types
	enum class Operator {
		op_and,
		op_or,
	};
	Operator operation;
	// left operand
	Operand* left_operand;
	// right operand
	Operand* right_operand;
	// expression as string for debug purposes
	//std::string expression_for_debug;
public:
	// constructor
	BinaryOperation(Operator op, Operand* left, Operand* right);

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
	// 1 parameters of the function
	struct Parameter {
		Type type;
		std::string name;
		// constructor
		Parameter(Type t, std::string n) : type(t), name(n) {}
	};
	// all the parameters of the function
	struct AllParameter {
		std::vector<Parameter> parameters;
		// constructor : init with 1 parameter
		AllParameter(Parameter& p0) {
			parameters.push_back(p0); 
		}
		// add 1 parameter
		void add(Parameter& p) {
			parameters.push_back(p);
		}
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
		Definition(Type t, std::string n, Function::AllParameter* all_params);
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

