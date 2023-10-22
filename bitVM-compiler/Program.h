#pragma once
// code tree after source code parsing

#include <string>
#include <vector>
#include <assert.h>	

class Function;
class CodeBloc;
class BuildContext;

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
	// type is defineds ?
	bool is_defined(void) const { return native_type != Native::undefined; }
	// compare
	bool is_same_type(const Type& other) const {
		return native_type == other.native_type;
	}

};

// opérandd expression. "a" ,123, or a+b
class Expression {
protected:

public:
	// init
	virtual void init(CodeBloc* parent_bloc) = 0;
	// get expression type
	virtual const Type& get_type(void) = 0;
	// build the circuit for the expression
	virtual void build_circuit(BuildContext& ctx) = 0;
};

// ex: 123
class Literal : public Expression {
public:
	// type of the literal
	Type type;
	// value of the literal
	std::string value;
public:
	// constructor
	Literal(Type t, std::string v) : type(t), value(v) {}
	// init
	virtual void init(CodeBloc* parent_bloc) override {}
	// get Operand type
	virtual const Type& get_type(void) override  { return  type; }
	// build the circuit for the  expression
	virtual void build_circuit(BuildContext& ctx) override;

};
class Variable : public Expression {
public:
	// type of the variable
	Type var_type;
	// name of the variable
	std::string var_name;
public:
	// constructor
	Variable(std::string n) : var_type(Type::Native::undefined), var_name(n) {}
	// init
	virtual void init(CodeBloc* parent_bloc) override;
	// get Operand type
	const Type& get_type(void) { assert(var_type.is_defined()); return var_type; }
	// build the circuit for the  expression
	virtual void build_circuit(BuildContext& ctx) override;
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
	Expression* left_operand;
	// right operand
	Expression* right_operand;
	// expression as string for debug purposes
	//std::string expression_for_debug;
public:
	// constructor
	BinaryOperation(Operator op, Expression* left, Expression* right);
	// init
	virtual void init(CodeBloc* parent_bloc) override;
	// get expression type
	virtual const Type& get_type(void) override { return result_type;}
	
	// build the circuit for the binairy expression
	void build_circuit(BuildContext& ctx);
};



// base class for a statement. 
// ex "v=a+2" or "return a+2;"
class Statement {
public:
	// init a statmenet
	virtual void init(CodeBloc* parent) {}

	virtual bool is_return(void) const { return false; }
};
// "return" statement
class Statement_Return : public Statement {
protected:
	// expression to return
	Expression *expression;
public:
	// constructor
	Statement_Return(Expression* e) : expression(e) {}
	// init a statmenet
	virtual void init(CodeBloc* parent) override;

	virtual bool is_return(void) const { return true; }
	// get return type
	const Type& get_type(void) const { return expression->get_type(); }

	// build the circuit for the return statelebt
	void build_circuit(BuildContext& ctx );
};

class CodeBloc {
protected:
	// code statements
	std::vector<Statement*> statements;
	Function* parent_function = nullptr;
public:
	// constructor
	CodeBloc(Statement *first_statement) { statements.push_back(first_statement);  }
	// add a statement
	void add_statement(Statement* s) { statements.push_back(s); }
	// init a bloc
	void init(Function* parent_function);
	// get the parent function
	Function* get_parent_function(void) { return parent_function; }
	// find a variable by name
	const Type* find_variable_by_name(std::string name) const;
	// get the return statement of the bloc
	Statement_Return* get_return_statement(void) const;

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
	CodeBloc *body;

public:
	// constructor
	Function(Definition* def, CodeBloc* body);
	// init a function
	void init(void);
	// get the name of the function
	std::string get_name(void) const { return definition.name; }
	// get the return type of the function
	const Type &get_return_type(void) const { return definition.return_type; }
	// return the number of bits needed for the input parameters
	int size_in_bit_input(void) const;
	// return the number of bits needed to store the return value
	int size_in_bit_output(void) const;
	// find a parameter by name
	const Type* find_parameter_by_name(std::string name) const;
	// build a circuit that represents the fuidl
	void build_circuit(class Circuit& circuit);

};




// logical representation of de contract 
class Program {
	// body of the function
	std::vector<Function*> functions;
public:
	// add a function to the program
	void add_function(Function* f) ;
	// get a function by name
	Function* find_function_by_name( std::string name ) const;
	// get main function
	Function* main_function(void) const;
	// init program tree, phase 2 of compilation
	void init_and_check_program_tree(void);

	// build a circuit that represents the program
	void build_circuit(class Circuit &circuit_out);
};

