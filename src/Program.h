#pragma once
// code tree after source code parsing

#include <string>
#include <vector>
#include <assert.h>	

class Function;
class CodeBloc;
class BuildContext;
class Connection;

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
	Native get_native_type(void) const { return native_type; }

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
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) = 0;
};

// ex: 123
class Literal : public Expression {
public:
	// type of the literal
	Type type;
	// value of the literal is string forme
	std::string value_str;
	// value of the literal in individuals bits
	std::vector<bool> value_bits;
public:
	// constructor
	Literal(Type t, std::string v) : type(t), value_str(v) {}
	// init
	virtual void init(CodeBloc* parent_bloc) override;
	// get Operand type
	virtual const Type& get_type(void) override  { return  type; }
	// build the circuit for the  expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;

	// convert hex string to array of bits in low endian
	static std::vector<bool> hex_string_to_bits(std::string hex_string);
protected:
	// convert the value of the literal to a vector of bits for bool type
	std::vector<bool> _get_bits_value_bool(std::string str_val) const;
	// convert the value of the literal to a vector of bits for byte type
	// convert to bits in low endian (x86 format)
	// result[0] is the least significant bit, result[7] is the most significant bit
	std::vector<bool> _get_bits_value_int8(std::string str_val) const;
};
class VariableDefinition {
public:
	// type of the variable
	Type var_type;
	// name of the variable
	std::string var_name;
};

class VariableExpression : public VariableDefinition, public Expression {
public:
	// constructor
	VariableExpression(std::string n) {
		var_type = Type::Native::undefined;
		var_name = n;
	}
	// init
	virtual void init(CodeBloc* parent_bloc) override;
	// get Operand type
	const Type& get_type(void) { assert(var_type.is_defined()); return var_type; }
	// build the circuit for the  expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};
// Math expression. ex :"a+2"
class BinaryOperation : public Expression {
public:
	// type of the result
	Type result_type;
	// opération : | & ^ + - *  
	//  types
	enum class Operator {
		op_and,
		op_or,
		op_xor,
		op_add,
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
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
protected:
	// build the circuit for the "a+b" expression
	std::vector<Connection*> _build_circuit_add(BuildContext& ctx,
												std::vector<Connection*>& in_a,
												std::vector<Connection*>& in_b);
};
// Math with 1 operand. ex :"!2"
class UnaryOperation : public Expression {
public:
	// type of the result
	Type result_type;
	// opération : | & ^ + - *  
	//  types
	enum class Operator {
		op_not,
	};
	Operator operation;
	//  operand
	Expression*operand;
	// expression as string for debug purposes
	//std::string expression_for_debug;
public:
	// constructor
	UnaryOperation(Operator op, Expression* exp);
	// init
	virtual void init(CodeBloc* parent_bloc) override;
	// get expression type
	virtual const Type& get_type(void) override { return result_type; }
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};



// base class for a statement. 
// ex "v=a+2" or "return a+2;"
class Statement {
public:
	// lgne number in the source code
	int num_line = 0;
public:
	// constructor
	Statement(int line) : num_line(line) {}
	// init a statmenet
	virtual void init(CodeBloc* parent) {}
	virtual bool is_return(void) const { return false; }

	// build the circuit for the return statelebt
	virtual void build_circuit(BuildContext& ctx) const = 0;
};
// "return" statement
class Statement_Return : public Statement {
protected:
	// expression to return
	Expression *expression;
public:
	// constructor
	Statement_Return(int line, Expression* e) : Statement(line), expression(e) {}
	// init a statmenet
	virtual void init(CodeBloc* parent) override;

	virtual bool is_return(void) const { return true; }
	// get return type
	const Type& get_type(void) const { return expression->get_type(); }

	// build the circuit for the return statement
	virtual void build_circuit(BuildContext& ctx ) const override;
};
// int i => declaration of a variable statement
class Statement_DeclareVar : public Statement {
protected:
	// type of the variable
	Type var_type;
	// name of the variable
	std::string var_name;
public:
	// constructor
	Statement_DeclareVar(int line, Type *type, std::string name) : Statement(line), var_type(*type), var_name(name){}
	// get return type
	const Type& get_type(void) const { return var_type; }
	// init a statmenet
	virtual void init(CodeBloc* parent) override;
	// build the circuit for the declaration statement
	virtual void build_circuit(BuildContext& ctx) const override;
};
// set a variable statement
class Statement_SetVar :  public Statement{
protected:
	// name of the variable
	std::string var_name;
	Expression* expression;
public:
	// constructor
	Statement_SetVar(int line, std::string name, Expression* op) : Statement(line), var_name(name), expression(op) {}
	// get return type
	const Type& get_type(void) const { return expression->get_type(); }
	// init a statmenet
	virtual void init(CodeBloc* parent) override;
	// build the circuit for the declaration statement
	virtual void build_circuit(BuildContext& ctx) const override;
};
// déclare and set a variable statement
class Statement_DeclareAndSetVar : public Statement  {
protected:
	Statement_DeclareVar declaration;
	Statement_SetVar	 affectation;

public:
	// constructor
	Statement_DeclareAndSetVar(int line, Type* type, std::string varname, Expression* expr) :
		Statement(line),
		declaration(line, type, varname),
		affectation{line, varname, expr}
	{}
	// get return type
	const Type& get_type(void) const { return declaration.get_type(); }
	// init a statmenet
	virtual void init(CodeBloc* parent) override;
	// build the circuit for the declaration statement
	virtual void build_circuit(BuildContext& ctx) const override;
};

class CodeBloc {
public:
	// code statements
	std::vector<Statement*> statements;
	Function* parent_function = nullptr;
	// declared local variables
	std::vector<VariableDefinition> local_variables;

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
	// declare a local variable
	void declare_local_variable(Type& type, std::string name);

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

