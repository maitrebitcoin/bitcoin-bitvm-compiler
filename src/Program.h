#pragma once
// code tree after source code parsing

#include <string>
#include <vector>
#include <assert.h>	
#include "Type.h"
#include "VariableDefinition.h"
#include "Expression.h"
#include "Expression_StructMember.h"
#include "CodeBloc.h"
#include "Scope.h"

class Function;
class CodeBloc;
class BuildContext;
class Connection;
class BinaryOperation;
class Literal;
class LangageAttributes;

#include "Literal.h"
#include "Expression_Variable.h"
#include "BinaryOperation.h"


// bit shit expression. ex :"a<<3"
class ShiftOperation : public BinaryOperation {
protected:
	// value of the lirreral R agufment
	int nb_sihft = 0;	
public:
	// constructor
	ShiftOperation(Operator op, Expression* left, Expression* right) : BinaryOperation(op, left, right) {};
	// init
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};
// test operator ex : "a==b"
// bit shit expression. ex :"a<<3"
class TestOperation : public BinaryOperation {
public:
	// constructor
	TestOperation(Operator op, Expression* left, Expression* right) : BinaryOperation(op, left, right) {};
	// init
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
	// build the circuit for the "==" expression
	static std::vector<Connection*>  build_circuit_equal(BuildContext& ctx,
		std::vector<Connection*>& in_a,
		std::vector<Connection*>& in_b);

};

// Math with 1 operand. ex :"!2"
class UnaryOperation : public Expression {
public:
	// type of the result
	TypeBasic result_type;
	// opération : | & ^ + - *  
	//  types
	enum class Operator {
		op_not,		  // !a
		op_negate,	  // -a
		op_complement,// ~a
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
	virtual void init(Scope& parent_scope) override;
	// get expression type
	virtual const Type& get_type(void) const override { return result_type; }
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;

	// build the circuit for the not expression
	static std::vector<Connection*> build_circuit_not(BuildContext& ctx, std::vector<Connection*>& inputs);
	// build the circuit for the negate expression
	static std::vector<Connection*> build_circuit_negation(BuildContext& ctx, std::vector<Connection*>& inputs);
};

#include "Statement.h"
#include "Statement_Return.h"
#include "Statement_DeclareVar.h"
#include "Statement_SetVar.h"
#include "Statement_DeclareAndSetVar.h"
#include "Statement_DeclareStruct.h"
#include "Statement_If.h"
#include "Function.h"
#include "Scope.h"

// logical representation of de contract 
class Program : public Scope {
	// structs definitions
	CodeBloc *struct_definitions;
	// body of the function
	std::vector<Function*> functions;
public:

	// add structures to the program as bloc before the main function
	void add_struct_definition(CodeBloc* bloc);
	// add a function to the program
	void add_function(Function* f) ;
	// get a function by name
	Function* find_function_by_name( std::string name ) const;
	// get main function
	Function* main_function(void) const;
	// init program tree, phase 2 of compilation
	void init_and_check_program_tree(void);


	// build a circuit that represents the program
	void build_circuit(class BuildContext &build_context);
};

