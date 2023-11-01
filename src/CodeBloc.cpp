
#include "CodeBloc.h"
#include "Program.h"
#include "Error.h"

// find a variable by name
const VariableDefinition* CodeBloc::find_variable_by_name(std::string var_name) const {
	// is it a fiunction parameter ?
	const VariableDefinition * function_param = parent_function->find_parameter_by_name(var_name);
	if (function_param != nullptr)
		return function_param;
	// is it a local variable ?
	for (const VariableDefinition& var_i : local_variables)
	{
		if (var_i.name == var_name)
			return &var_i;
	}
	// not found
	return nullptr;
}
// declare a local variable
void CodeBloc::declare_local_variable(const VariableDefinition &def) {
	// add the variable to the list
	local_variables.push_back(VariableDefinition{ def });
}
// find a struct type by name
const TypeStruct* CodeBloc::find_struct_by_name(std::string name) const {
	for (const TypeStruct& struct_i : know_types) {
		if (struct_i.name == name)
			return &struct_i;
	}
	// not found
	return nullptr;
}
// get the return statement of the bloc
Statement_Return* CodeBloc::get_return_statement(void) const {
	// always last in bloc
	Statement* last_statement = statements.back();
	return dynamic_cast<Statement_Return*>(last_statement);
}

// init a bloc
void CodeBloc::init(Function* parent) {
	// init parent function
	parent_function = parent;
	// init statemtents
	for (Statement* statement_i : statements) {
		try {
			statement_i->init(this);
		}
		catch (Error& e) {
			// add the line number
			e.line_number = statement_i->num_line;
			throw e;
		}
	}
	// check statements :
	if (statements.size() == 0)
		throw Error("Empty function");
	// last statement must be a return
	Statement* last_statement = statements.back();
	if (!last_statement->is_return())
		throw Error("Last statement must be a return");
}