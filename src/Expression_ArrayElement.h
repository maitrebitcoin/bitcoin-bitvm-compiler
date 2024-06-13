#pragma once

#include <string>
#include "Expression.h"
#include "BuildContext.h"

// expression of the form "my_array[3]"

class Expression_ArrayElement : public Expression {
public:
	std::string array_name; // ex : "my_array"
	Expression* index = nullptr; // index in the array

	const Type* element_type = nullptr; // type the element
	int member_offest_in_bit = 0; // offset in bit of the member 

public:
	// constructor
	Expression_ArrayElement(std::string array_name, Expression* index);

	// get expression type
	virtual const Type& get_type(void) const override;
	// visit all part used in the Expression
	virtual void visit_epression(IVisitExpression& visitor) override;
	// init
	virtual void init(Scope& parent_scope) override;

	// build the circuit for the expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;

protected:
	// build the circuit for the expression for a int index value
	std::vector<Connection*> build_circuit_for_int_index(const ScopeVariable& var_array, int index);
};