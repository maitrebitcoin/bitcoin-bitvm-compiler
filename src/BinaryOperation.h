#pragma once


// Math expression. ex :"a+2"
class BinaryOperation : public Expression {
public:
	// type of the result
	TypeBasic result_type;
	// opération types : | & ^ + - *  
	enum class Operator {
		invalid_operator = -1,
		op_and,
		op_or,
		op_xor,
		op_add,
		op_sub,
		op_left_shift,
		op_right_shift,
		op_test_equal,
		op_test_not_equal,
		op_test_lower,
		op_test_lower_or_equal,
		op_test_greater,
		op_test_greater_or_equal,
		op_mult,
		op_div,
		op_modulo,
	};
	Operator operation;
	// left operand
	Expression* left_operand;
	// right operand
	Expression* right_operand;
	// expression as string for debug purposes
	//std::string expression_for_debug;
public:
	// constructor
	BinaryOperation(Operator op, Expression* left, Expression* right);
	// reorg epxresion tree to ensure operator precedence
	// retrun the new root of the expression
	BinaryOperation* reorg_for_precedence(LangageAttributes& language);
	// visit all part used in the Expression
	virtual void visit_epression(IVisitExpression& visitor) override;
	// init
	virtual void init(Scope& parent_scope) override;
	// true is the expression has a higher precedence than the othe expression
	bool has_higher_precedence(const BinaryOperation& other_expression, LangageAttributes& language) const;
	// get expression type
	virtual const Type& get_type(void) const override { return result_type; }
	// if the expression is a binairy expression, return it
	virtual BinaryOperation* cast_to_BinaryOperation(void) override { return this; }
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
	// build the circuit for the "a+b" expression
	static std::vector<Connection*>  build_circuit_add(BuildContext& ctx,
		std::vector<Connection*>& in_a,
		std::vector<Connection*>& in_b);
	// build the circuit for the "a-b" expression
	static std::vector<Connection*>  build_circuit_sub(BuildContext& ctx,
		std::vector<Connection*>& in_a,
		std::vector<Connection*>& in_b);

};

