
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"

// constructor
Statement_DeclareVar::Statement_DeclareVar(int line, Type* type, std::string name) 
	: Statement(line)
	, VariableDefinition(type, name) 
{}


// init a statmenet
void Statement_DeclareVar::init(CodeBloc* parent_bloc) {
	// if variable namae already used
	const VariableDefinition* existing_variable_definition = parent_bloc->find_variable_by_name(name);
	if (existing_variable_definition != nullptr)
		throw Error("Variable already declared : ", name);
	// declare the variable type
	parent_bloc->declare_local_variable(*this);

}

// build the circuit for the declaration statement
void Statement_DeclareVar::build_circuit(BuildContext& ctx) const {
	// if the variable is already known
	if (ctx.variables.find_by_name(name) != nullptr)
		throw Error("Variable already declared : ", name);
	// declare the variable
	ctx.variables.declare_local_var(type, name);
}
