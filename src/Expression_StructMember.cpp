
#include "Program.h"
#include "Expression_StructMember.h"
#include "Error.h"
#include "Scope.h"

// constructor
Expression_StructMember::Expression_StructMember(std::string p, std::string m)
	: parent_name(p), member_name(m) 
{}



// get expression type
const Type& Expression_StructMember::get_type(void) const {
	if (member_type == nullptr) {
		// error :type not yet set
		throw Error("Internal error, member type non resolded", member_name);
	}
	return *member_type;
}
// init
void Expression_StructMember::init(Scope& parent_scope) {
	// get the struct parent from name
	const VariableDefinition *parent_definition = parent_scope.find_variable_by_name(parent_name);
	if (parent_definition == nullptr) {
		// error : 'name' not found
		throw Error("identifier not found :", parent_name);
	}
	// must be a struct
	assert(parent_definition->type != nullptr);
	assert(parent_definition->type->is_complete());
	const TypeStruct* parent_type_struct = parent_definition->type->cast_to_TypeStruct();
	if (parent_type_struct == nullptr)	{
		// error : parent is not a structure
		throw Error("not a structure : ", parent_name);
	}

	// get the member type by name
	TypeStruct::Member member = parent_type_struct->get_member_by_name(member_name);
	if (!member.is_valid) {
		// error : member not found
		throw Error("member not found : ", member_name);
	}
	// keep the type and the offset
	member_type = member.type;
	member_offest_in_bit = member.offest_in_bit;
}

// get all gates used by the parent struct variable
std::vector<Connection*> Expression_StructMember::get_all_connexions_full_struct(BuildContext& ctx) const {
	// get the parent variable type by name
	ScopeVariable* var = ctx.variables().find_by_name(parent_name);
	if (var == nullptr)
		return {};
	// if variable not set
	if (!var->is_set())
		return {};
	// return all the bits of the variable
	return var->bits;
}


// build the circuit for the expression
std::vector<Connection*> Expression_StructMember::build_circuit(BuildContext& ctx) {
	// get the parent variable type by name
	ScopeVariable* var = ctx.variables().find_by_name(parent_name);
	if (var == nullptr)
		throw Error("Unknonwn variable : ", parent_name);
	// if variable not set
	if (!var->is_set())
		throw Error("Uninitialized variable : ", parent_name);

	// set outputs to get the value of the part of variable in the struct
	assert(member_type != nullptr);
	int member_size		= member_type->size_in_bit();
	auto begin_member = var->bits.begin() + member_offest_in_bit;
	std::vector < Connection*> bits_member(begin_member, begin_member + member_size);
	assert(bits_member.size() == member_type->size_in_bit());
	return bits_member;

}


