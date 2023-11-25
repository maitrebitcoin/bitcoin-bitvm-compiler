
#include <assert.h>
#include "Program.h"
#include "Error.h"
#include "Gate.h"
#include "Circuit.h"


// constructor
Statement_For::Statement_For(int line, Statement* init, Expression* condition, Statement* increment, CodeBloc* c)
	: Statement(line)
	, for_init(init)
	, for_condition(condition)
	, for_increment(increment)
	, code(c)
{

}
// visit all expression ueed in the statement
void Statement_For::visit_Expression(std::function<void(Expression& expr)> visitor) const {
	visitor(*for_condition);
	code->visit_Expression(visitor);
}

// init for statmenet
void Statement_For::init(Scope& parent_scope) {

	// init statement a expressions
	for_init->init( parent_scope );
	for_condition->init(parent_scope);
	for_increment->init( parent_scope );
	// init blocs of code
	code->init(parent_scope);

	// init statement mut be declaration of a variable set to an literal
	// ex: int i=0
	Statement_DeclareAndSetVar* init_as_declare_var = for_init->cast_to_Statement_DeclareAndSetVar();
	if (init_as_declare_var == nullptr)
		throw Error("For init must be a var declaration = literal");
	// must be a litteral
	Literal* litteral_init = init_as_declare_var->affectation.expression->cast_to_literal();
	if (init_as_declare_var == nullptr)
		throw Error("For init must be a var declaration = literal");
	start_value = litteral_init->get_int_value();

	// condition statemetn must be "i<0" style
	TestOperation* test_operation = for_condition->cast_to_TestOperationr();
	if (test_operation == nullptr)
		throw Error("For condition must be var < literal");
	if (test_operation->operation != BinaryOperation::Operator::op_test_lower)
		throw Error("For condition must be var < literal");
	Literal* litteral_end = test_operation->right_operand->cast_to_literal();
	if (litteral_end == nullptr)
		throw Error("For condition must be var < literal");
	end_value = litteral_end->get_int_value();

	// for_increment must be "i++" style
	Statement_Increment* increment_as_increment = for_increment->cast_to_Statement_Increment();
	if (increment_as_increment == nullptr)
		throw Error("Increment statement must be var++");

	// init ok
}
// build the circuit for the for statement
void Statement_For::build_circuit(BuildContext& ctx) const {

	throw Error("TODO");

}
