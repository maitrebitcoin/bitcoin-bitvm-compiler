#pragma once

#include <vector>
#include <functional>
class Literal;
class BinaryOperation;
class CodeBloc;
class BuildContext;
class Connection;
class Scope;
class Expression_Variable;
class Expression_StructMember;
class TestOperation;

class IVisitExpression {
public:
	// epxression part is a literal.ex : 123
	virtual void onLiteral(Literal&) = 0;
	// epxressison part is a varible. ex : a
	virtual void onVariable(Expression_Variable&) = 0;
	// epxressison part is a varible in a struct. ex : a.b
	virtual void onVariableInStruct(Expression_StructMember&) = 0;

};



// opérandd expression. "a" ,123, or a+b
class Expression {
protected:
	// true if the expression is between parentesis. ex: (a+1)
	bool has_parentesis = false;
public:
	// set if the expression is between parentesis. ex: (a+1)
	void set_parentesis(bool b) { has_parentesis = b; }

	// if the expression is a littrela, return it
	virtual Literal* cast_to_literal(void) { return nullptr; }
	// if the expression is a binairy expression, return it
	virtual BinaryOperation* cast_to_BinaryOperation(void) { return nullptr; }
	// cast to 	TestOperation test_operation = for_condition->cast_to_TestOperationr();
	virtual TestOperation* cast_to_TestOperationr(void)  { return nullptr; }
	// cast to Expression_Variable
	virtual Expression_Variable* cast_to_Expression_Variable(void) { return nullptr; }

	// get expression type
	virtual const Type& get_type(void) const = 0;
	// visit all part used in the Expression
	virtual void visit_epression(IVisitExpression& visitor)  = 0;

	// init
	virtual void init(Scope& parent_scope) = 0;
	// build the circuit for the expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) = 0;
};