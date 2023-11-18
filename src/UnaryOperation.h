#pragma once

#include "Expression.h"

// Math with 1 operand. ex :"!2"
class UnaryOperation : public Expression {
public:
	// type of the result
	TypeBasic result_type;
	// opération : | & ^ + - *  
	//  types
	enum class Operator {
		op_not,		  // !a
		op_negate,	  // -a
		op_complement,// ~a
	};
	Operator operation;
	//  operand
	Expression* operand;
	// expression as string for debug purposes
	//std::string expression_for_debug;
public:
	// constructor
	UnaryOperation(Operator op, Expression* exp);
	// get expression type
	virtual const Type& get_type(void) const override { return result_type; }
	// visit all variables used in the Expression
	virtual void visit_all_used_variable(std::function<void(IVariableToConnexion& expr_var)> visitor)  override { operand->visit_all_used_variable(visitor); }
	// init
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;

	// build the circuit for the not expression
	static std::vector<Connection*> build_circuit_not(BuildContext& ctx, std::vector<Connection*>& inputs);
	// build the circuit for the negate expression
	static std::vector<Connection*> build_circuit_negation(BuildContext& ctx, std::vector<Connection*>& inputs);
};
