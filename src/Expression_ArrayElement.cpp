
#include "Expression_ArrayElement.h"
#include "Error.h"
#include "Scope.h"
#include "TypeArray.h"
#include "Literal.h"
#include "Expression_Variable.h"
#include "CompositedGate.h"
#include "TestOperation.h"

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

	// init index operand
	index->init(parent_scope);

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

// get the array size = number of elements
int Expression_ArrayElement::get_array_size(const ScopeVariable& var_array) const {
	// divide the number of bits in the arrya by the size of the element
	int element_size = element_type->size_in_bit();
	IndexType array_size = (IndexType)(var_array.bits.size() / element_size);
	return array_size;
	
}

// build the circuit for the expression for a int index value
std::vector<Connection*> Expression_ArrayElement::build_circuit_for_int_index(BuildContext& ctx, const ScopeVariable& var_array, IndexType index_value) {
	assert(element_type != nullptr);
	int element_size = element_type->size_in_bit();

	// check index value
	IndexType array_size = get_array_size(var_array); // (int)var_array.bits.size() / element_size;
	if (index_value < 0 || index_value >= array_size)
		throw Error("Array index out of range : ", std::to_string(index_value));

	// get offest in bits in the array
	int offset_in_bit = element_size * index_value;
	auto begin_member = var_array.bits.begin() + offset_in_bit;
	// get the bits of the element
	std::vector < Connection*> bits_member(begin_member, begin_member + element_size);
	assert(bits_member.size() == element_type->size_in_bit());
	return bits_member;
}


// build the circuit for the expression
std::vector<Connection*> Expression_ArrayElement::build_circuit(BuildContext& ctx) {
	// get the parent variable type by name
	ScopeVariable* var_array = ctx.variables().find_by_name(array_name);
	if (var_array == nullptr)
		throw Error("Unknonwn variable : ", array_name);
	// if variable not set
	if (!var_array->is_set())
		throw Error("Uninitialized variable : ", array_name);

	//1. literal value as index. ex : a[3]
	Literal* index_literal = index->cast_to_literal();
	if (index_literal != nullptr) {
		// set outputs to get the value of the part of variable in the struct
		return build_circuit_for_int_index(ctx, *var_array, index_literal->get_int_value());
	}

	//2. expression as index. ex : a[b+1]
	

	//2.1 : simmple var, ex : a[i]
	Expression_Variable* index_var = index->cast_to_Expression_Variable();
	if (index_var != nullptr)
	{
		// get the variable type by name
		ScopeVariable* var = ctx.variables().find_by_name(index_var->name);
		ScopeVariable::STValInt val_int = var->get_int_value();
		if (val_int.is_set)	{
			return build_circuit_for_int_index(ctx, *var_array, val_int.value );
		}
	}

	//2.1 : complex expression, ex : a[i*j+1]

//TODO

	// generate an expression like this :
	//   (a[0] & (I==0))
	// | (a[1] & (I==1))
	// | (a[2] & (I==2))
	//  ...
	// = a[i]
	// generte index circuit from its expression
	std::vector<Connection*>  index_expression = index->build_circuit(ctx);

	std::vector<Connection*> result;
	// (a[i] xor (a[i] & (Index == 0)))
	IndexType array_size = get_array_size(*var_array);
	assert(array_size > 0);
	result = generate_expression_for_index( ctx, *var_array, index_expression, 0);
	for (IndexType index_to_test = 1; index_to_test < array_size; index_to_test++) {
		// (a[i] xor (a[i] & (Index==i))) ...
		std::vector<Connection*> next = generate_expression_for_index(ctx, *var_array, index_expression, index_to_test);
		// mix input : a = a | b
		result = generate_or(ctx, result, next);
	}
	// final result :
	return result;
}

// return (a | b) addeded to the circuit
std::vector<Connection*> Expression_ArrayElement::generate_or(BuildContext& ctx, const std::vector<Connection*>& a, const std::vector<Connection*>& b) {
	assert(a.size() == b.size());
	std::vector<Connection*> result;

	Gate_OR gate_or;

	// for each bit connect gate input and output
	int size = (int)a.size();
	for (int i = 0; i < size; i++) {
		// IN
		std::array<Connection*, 2> input_2_bit = { a[i], b[i] };
		// OUT = A OP B
		std::array<Connection*, 1> bits_result = gate_or.add_to_circuit(ctx.circuit(), input_2_bit);

		//result.insert(result.end(), bits_result.begin(), bits_result.end());
		result.push_back(bits_result[0]);

	}
	return result;

}
// generate the expression a[index] & (expression==index)
// the circuite will return a[index] if expression == index , 0 otherwise
std::vector<Connection*> Expression_ArrayElement::generate_expression_for_index(BuildContext& ctx, const ScopeVariable& var_array, std::vector<Connection*> expression, IndexType index_value)
{
	// gate for buildinf
	Gate_AND gate_and;

	// get a[index]
	std::vector<Connection*> a_at_index = build_circuit_for_int_index(ctx, var_array, index_value);
	
	// (expression==index] ?
	// get a type with the number of bits of the index
	TypeBasic type_exp = TypeBasic::get_TypeBasic_for_bitsize((int)expression.size());
	Literal index_as_literal(type_exp, std::to_string(index_value) );
	Scope* no_scope = nullptr;
	index_as_literal.init(*no_scope);
	std::vector<Connection*> index_as_connexions = index_as_literal.build_circuit(ctx);
	std::vector<Connection*> exp_eq_i = TestOperation::build_circuit_equal(ctx, expression, index_as_connexions);

	// a[index] & (expression==index)
	std::vector<Connection*> result;
	int size = (int)a_at_index.size();
	for (int i = 0; i < size; i++) {
		// IN : each bit of a[index] and same bit b = (expression==index)
		std::array<Connection*, 2> input_2_bit = { a_at_index[i], exp_eq_i[0] };

		std::array<Connection*, 1> bits_result = gate_and.add_to_circuit(ctx.circuit(), input_2_bit);
		result.push_back(bits_result[0]);
	}

	return result;



}

