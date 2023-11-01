
#include "Program.h"
#include "Expression_StructMember.h"
#include "Error.h"

// constructor
Expression_StructMember::Expression_StructMember(std::string p, std::string m)
	: parent_name(p), member_name(m) 
{}


// get expression type
const Type& Expression_StructMember::get_type(void) const {
	if (type == nullptr) {
		// error :type not yet set
		throw Error("Internal error, type non resolded");
	}
	return *type;
}
// init
void Expression_StructMember::init(CodeBloc* parent_bloc) {
	// get the struct parent from name
	const VariableDefinition *parent_definition = parent_bloc->find_variable_by_name(parent_name);
	if (parent_definition == nullptr) {
		// error : 'name' not found
		throw Error("identifier not found :", parent_name);
	}
	// must be a struct
	assert(false); //TODO
	TypeStruct* parent_type_struct = nullptr;// parent_definition->type.cast_to_TypeStruct();
	if (parent_type_struct == nullptr)	{
		// error : parent is not a structure
		throw Error("not a structure : ", parent_name);
	}

	//TODO

}

// build the circuit for the expression
std::vector<Connection*> Expression_StructMember::build_circuit(BuildContext& ctx) {
	// get the parent variable type by name
	VarBuild* parent_var = ctx.variables.find_by_name(parent_name);
	if (parent_var == nullptr)
		throw Error("Unknonwn variable : ", parent_name);

	// get the var in the struct by name
	VarBuild* var = parent_var->find_member_by_name(member_name);

	// if variable not set
	if (!var->is_set())
		throw Error("Uninitialized variable : ", member_name);
	// set outputs to get the value of the variable
	return var->bits;

}


