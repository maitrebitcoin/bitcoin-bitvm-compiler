
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"

//  assignment statement init
void Statement_SetVar::init(Scope& parent_scope) {
	expression->init(parent_scope);

	// get the variable type by name
	const VariableDefinition* var = parent_scope.find_variable_by_name(var_name);
	if (var != nullptr) {
		// change type of litterals to match the variable type
		Type* var_type = var->type;
		expression->change_all_litterals_type(*var_type);
	}

}

// build the circuit for the assignment statement
void Statement_SetVar::build_circuit(BuildContext& ctx) const {
	// get the variable type by name
	ScopeVariable* var = ctx.variables.find_by_name(var_name);
	if (var == nullptr)
		throw Error("Unknonwn variable : ", var_name);

	// check variable type
	if (!var->type->is_same_type(expression->get_type()))
		throw Error("Type mismatch : ", var_name);

	// build the R expression
	std::vector<Connection*> expression_value = expression->build_circuit(ctx);
	// connect the output of the expression to current value of the variable
	var->set_value(expression_value);
}