
#include "Program.h"
#include "Error.h"


// constructor
Statement_DeclareStruct::Statement_DeclareStruct(int line, std::string name, CodeBloc* all_members)
	: Statement(line)
	, struct_type(name) 	// init name
{
	// init members
	for (Statement* statement : all_members->statements) {
		// must be a "int i;" ype statement
		Statement_DeclareVar* statement_declare_var = statement->cast_to_Statement_DeclareVarr();
		if (statement_declare_var == nullptr)
			throw Error("Invalid struct member declaration");
		// add the member
		_add_member(statement_declare_var->type, statement_declare_var->name);
	}
}
// add a member to the struct
void Statement_DeclareStruct::_add_member(Type* type, std::string name) {
	VariableDefinition new_member(type,name);
	struct_type.members.push_back(new_member);
}
// init a statmenet
void  Statement_DeclareStruct::init(Scope& parent_scope) {
	// check if the struct name is already used
	if (parent_scope.find_struct_by_name(struct_type.name) != nullptr)
		throw Error("Struct name already used : ", struct_type.name);
	// add the struct to the parent bloc
	parent_scope.declare_struct(this->struct_type);
}
// build the circuit for the declaration statement
void Statement_DeclareStruct::build_circuit(BuildContext& ctx) const {
	// nothind to do
}

