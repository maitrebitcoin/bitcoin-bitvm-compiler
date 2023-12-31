
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"
#include "Scope.h"

// op�rand Variable init
void Expression_Variable::init(Scope& parent_scope) {
	// get the variable type by namae
	const VariableDefinition* variable_defintion = parent_scope.find_variable_by_name(name);
	if (variable_defintion == nullptr)
		throw Error("Variable not found", name);
	// set the type
	assert(variable_defintion->type != nullptr);
	type = variable_defintion->type;
}

// get all gates used by the variable
std::vector<Connection*> Expression_Variable::get_all_connexions(BuildContext& ctx) const
{
	// get the variable type by name
	ScopeVariable* var = ctx.variables().find_by_name(name);
	if (var == nullptr)
		return {};
	// if variable not set
	if (!var->is_set())
		return {};
	return var->bits;
}

// build the circuit for the  expression
std::vector<Connection*> Expression_Variable::build_circuit(BuildContext& ctx) {

	// get the variable type by name
	ScopeVariable* var = ctx.variables().find_by_name(name);
	if (var == nullptr)
		throw Error("Unknonwn variable : ", name);
	// if variable not set
	if (!var->is_set())
		throw Error("Uninitialized variable : ", name);

	// set outputs to get the value of the variable
	return var->bits;

}