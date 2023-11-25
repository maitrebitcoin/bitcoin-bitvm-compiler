
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
	code->init_ex(parent_scope, CodeBloc::InitOption::return_not_required);

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

	// for_increment must be "i++" style
	Statement_Increment* increment_as_increment = for_increment->cast_to_Statement_Increment();
	if (increment_as_increment == nullptr)
		throw Error("Increment statement must be var++");
	if (increment_as_increment->is_increment)
		incr_value = 1;// i++
	else
		incr_value = -1; // i--

	// condition statemetn must be "i<0" style
	TestOperation* test_operation = for_condition->cast_to_TestOperationr();
	if (test_operation == nullptr)
		throw Error("For condition must be var < literal");
	if (increment_as_increment->is_increment) {
		// < or <= only
		if (   test_operation->operation != BinaryOperation::Operator::op_test_lower
			&& test_operation->operation != BinaryOperation::Operator::op_test_lower_or_equal)
			throw Error("For condition must be var < literal");
	}
	else {
		// > or >= only
		if (   test_operation->operation != BinaryOperation::Operator::op_test_greater
			&& test_operation->operation != BinaryOperation::Operator::op_test_greater_or_equal)
			throw Error("For condition must be var > literal");
	}
	Literal* litteral_end = test_operation->right_operand->cast_to_literal();
	if (litteral_end == nullptr)
		throw Error("For condition must be var < literal");
	end_value = litteral_end->get_int_value();
	// <= :  loop 1 more time
	if (test_operation->operation == BinaryOperation::Operator::op_test_lower_or_equal)
		end_value++;
	// >= :  loop 1 more time
	if (test_operation->operation == BinaryOperation::Operator::op_test_greater_or_equal)
		end_value--;

	// sanity check on loop values
	if (incr_value == 1 && end_value<=start_value)
		throw Error("For loop : invalid bounds");
	if (incr_value == -1 && end_value >= start_value)
		throw Error("For loop : invalid bounds");

	// init ok
}
// build the circuit for the for statement

void Statement_For::build_circuit(BuildContext& ctx) const {

	// build a new context for the for loop
	BuildContext ctx_for_loop(ctx, BuildContext::Caller::for_statement );
	// init : crate loop var and set it to start value
	for_init->build_circuit(ctx_for_loop);
	Statement_DeclareAndSetVar* for_init_as_declare_var = for_init->cast_to_Statement_DeclareAndSetVar();
	Statement_SetVar& statement_set_var = for_init_as_declare_var->affectation;

	// loop to create all gates
	for (int i = start_value; i != end_value; i += incr_value) {
		// set loop var to current value
		statement_set_var.build_circuit_set_to_int(ctx_for_loop, i);
		// build circuit for the code bloc
		code->build_circuit(ctx_for_loop);
	}

}

