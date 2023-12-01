
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"

// constructor
Statement_DeclareVar::Statement_DeclareVar(int line, Type* type, std::string name) 
	: Statement(line)
	, VariableDefinition(type, name) 
{}


// init a statmenet
void Statement_DeclareVar::init(Scope& parent_scope) {
	// if variable namae already used
	const VariableDefinition* existing_variable_definition = parent_scope.find_variable_by_name(name);
	if (existing_variable_definition != nullptr)
		throw Error("Variable already declared : ", name);
	// if the type is just a name, get the real type
	if (!type->is_complete())	{
		const TypeStruct* real_type = parent_scope.find_struct_by_name(type->get_name());
		if (real_type == nullptr)
			throw Error("Unknown type : ", type->get_name());
		type = const_cast<TypeStruct *>(real_type);
	}

	// declare the variable type
	parent_scope.declare_local_variable(*this);

}

// build the circuit for the declaration statement
BuildContext::NextAction  Statement_DeclareVar::build_circuit(BuildContext& ctx) const {
	// if the variable is already known
	if (ctx.variables().find_by_name(name) != nullptr)
	{
		// in a for loop, the variable can be declared twice, just ignore the second declaration
		if (ctx.is_in_for_loop())
			return BuildContext::NextAction::Continue;

		throw Error("Variable already declared : ", name);
	}
	// declare the variable
	ctx.variables().declare_local_var(type, name);
	return BuildContext::NextAction::Continue;

}
