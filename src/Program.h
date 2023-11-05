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



// ex: 123
class Literal : public Expression {
public:
	// type of the literal
	TypeBasic type;
	// value of the literal is string forme
	std::string value_str;
	// value of the literal in individuals bits
	std::vector<bool> value_bits;
public:
	// constructor
	Literal(TypeBasic t, std::string v) : type(t), value_str(v) {}
	// init
	virtual void init(Scope& parent_scope) override;
	// get Operand type
	virtual const Type& get_type(void) const override  { return  type; }
	// if the expression is a littrela, return it
	virtual Literal* cast_to_literal(void) override  { return this; }
	// value of the literal has int. only for int type
	int get_int_value(void) const;

	// build the circuit for the  expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;

	// convert hex string of any size to array of bits in low endian
	static std::vector<bool> hex_string_to_bits(std::string hex_string);
protected:
	// convert the value of the literal to a vector of bits for bool type
	std::vector<bool> _get_bits_value_bool(std::string str_val) const;
	// convert the value of the literal to a vector of bits for byte type
	// convert to bits in low endian (x86 format)
	// result[0] is the least significant bit, result[7] is the most significant bit
	std::vector<bool> _get_bits_value_int8(std::string str_val) const;
};

#include "Expression_Variable.h"

// Math expression. ex :"a+2"
class BinaryOperation : public Expression {
public:
	// type of the result
	TypeBasic result_type;
	// opération types : | & ^ + - *  
	enum class Operator {
		invalid_operator = -1,
		op_and,
		op_or,
		op_xor,
		op_add,
		op_sub,
		op_left_shift,
		op_right_shift,
		op_test_equal,
		op_test_not_equal,
		op_test_lower,
		op_test_lower_or_equal,
		op_test_greater,
		op_test_greater_or_equal,
		op_mult,
		op_div,
		op_modulo,
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
	// reorg epxresion tree to ensure operator precedence
	// retrun the new root of the expression
	BinaryOperation* reorg_for_precedence( LangageAttributes& language);
	// init
	virtual void init(Scope& parent_scope) override;
	// true is the expression has a higher precedence than the othe expression
	bool has_higher_precedence(const BinaryOperation& other_expression, LangageAttributes& language) const;
	// get expression type
	virtual const Type& get_type(void) const override { return result_type;}
	// if the expression is a binairy expression, return it
	virtual BinaryOperation* cast_to_BinaryOperation(void) override  { return this; }
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
	// build the circuit for the "a+b" expression
	static std::vector<Connection*>  build_circuit_add(BuildContext& ctx,
													   std::vector<Connection*>& in_a,
													   std::vector<Connection*>& in_b);
	// build the circuit for the "a-b" expression
	static std::vector<Connection*>  build_circuit_sub(BuildContext& ctx,
													   std::vector<Connection*>& in_a,
													   std::vector<Connection*>& in_b);
	
};
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
	void build_circuit(class Circuit &circuit_out);
};

