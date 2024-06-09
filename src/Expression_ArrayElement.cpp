
#include "Expression_ArrayElement.h"
#include "Error.h"
#include "Scope.h"
#include "TypeArray.h"
#include "Literal.h"

// constructor
Expression_ArrayElement::Expression_ArrayElement(std::string name, Expression* index_)
	: array_name(name)
	, index(index_) {
	assert(index != nullptr);
}

// get element type
const Type& Expression_ArrayElement::get_type(void) const {
	if (element_type == nullptr) {
		// error :type not yet set
		throw Error("Internal error, array type non resolded");
	}
	return *element_type;
}

// visit all part used in the Expression
void Expression_ArrayElement::visit_epression(IVisitExpression& visitor) 
{
	visitor.onArrayElement(*this); 
}

// init
void Expression_ArrayElement::init(Scope& parent_scope) {
	// get the array type from name
	const VariableDefinition* parent_definition = parent_scope.find_variable_by_name(array_name);
	if (parent_definition == nullptr) {
		// error : 'name' not found
		throw Error("identifier not found :", array_name);
	}
	// must be a an arrau
	assert(parent_definition->type != nullptr);
	assert(parent_definition->type->is_complete());
	const TypeArray* parent_type_array = parent_definition->type->cast_to_TypeArray();
	if (parent_type_array == nullptr) {
		// error : parent is not an array
		throw Error("not an	array : ", array_name);
	}
	assert(parent_type_array->element_type != nullptr);

	// keep the type 
	element_type = parent_type_array->element_type;
}

// build the circuit for the expression
std::vector<Connection*> Expression_ArrayElement::build_circuit(BuildContext& ctx) {
	// get the parent variable type by name
	ScopeVariable* var = ctx.variables().find_by_name(array_name);
	if (var == nullptr)
		throw Error("Unknonwn variable : ", array_name);
	// if variable not set
	if (!var->is_set())
		throw Error("Uninitialized variable : ", array_name);

	//1. literal value as index. ex : a[3]
	Literal* index_literal = index->cast_to_literal();
	if (index_literal != nullptr) {
	
		// set outputs to get the value of the part of variable in the struct
		assert(element_type != nullptr);
		int element_size  = element_type->size_in_bit();
		int offset_in_bit = element_size * index_literal->get_int_value();
		auto begin_member = var->bits.begin() + offset_in_bit;
		std::vector < Connection*> bits_member(begin_member, begin_member + element_size);
		assert(bits_member.size() == element_type->size_in_bit());
		return bits_member;
	}

	//2. expression as index. ex : a[b]
//TODO
	throw Error("a[b]Not yet implemented");

}
