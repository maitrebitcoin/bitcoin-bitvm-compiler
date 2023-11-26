// var++ ou var-- statement

#include "Program.h"
#include "Error.h"
#include "BinaryOperation.h"

// constructor
Statement_Increment::Statement_Increment(int line, std::string name, bool increment) 
	: Statement(line), var_name(name), is_increment(increment) {

	// build a "var++" ou "var--" expression
	BinaryOperation::Operator op = is_increment ? BinaryOperation::Operator::op_add : BinaryOperation::Operator::op_sub;
	_1 = new Literal(TypeBasic(Type::Native::int8), "1");
	Expression_Variable* var_expression = new Expression_Variable(var_name);
	incr_expression = new BinaryOperation(op, var_expression , _1);

}

// get return type
const Type& Statement_Increment::get_type(void) const {
	assert(type != nullptr);
	return *type;
}


// visit all expression ueed in the statement
void Statement_Increment::visit_Expression(std::function<void(Expression& expr)> visitor) const {
	assert(incr_expression != nullptr);
	visitor(*incr_expression);
}


//  assignment statement init
void Statement_Increment::init(Scope& parent_scope) {

	// opérand Variable init
	// get the variable type by namae
	const VariableDefinition* variable_defintion = parent_scope.find_variable_by_name(var_name);
	if (variable_defintion == nullptr)
		throw Error("Variable not found", var_name);
	// set the type
	assert(variable_defintion->type != nullptr);
	type = variable_defintion->type;
	// type must int
	if (!type->is_integer())
		throw Error("Var type invalid for operator ++ or -- ", var_name);

	// change litteal type
	_1->type = *type->cast_to_TypeBasic();

	// init expression
	incr_expression->init(parent_scope);
}

// build the circuit for the assignment statement
Statement::NextAction Statement_Increment::build_circuit(BuildContext& ctx) const {
	// get the variable type by name
	ScopeVariable* var = ctx.variables.find_by_name(var_name);
	if (var == nullptr)
		throw Error("Unknonwn variable : ", var_name);

	// build the R expression
	std::vector<Connection*> expression_value = incr_expression->build_circuit(ctx);
	// connect the output of the expression to current value of the variable
	var->set_value(expression_value);
	return NextAction::Continue;
}

