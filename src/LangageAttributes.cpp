// definetion of the class LangageAttributes

#include "LangageAttributes.h"
#include "Program.h"

/*
enum  Operator {
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

};*/
static const int invalid_operator		= (int)BinaryOperation::Operator::invalid_operator;
static const int _mult					= (int)BinaryOperation::Operator::op_mult;
static const int _div					= (int)BinaryOperation::Operator::op_div;
static const int _modulo				= (int)BinaryOperation::Operator::op_modulo;
static const int _add					= (int)BinaryOperation::Operator::op_add;
static const int _sub					= (int)BinaryOperation::Operator::op_sub;
static const int _left_shift			= (int)BinaryOperation::Operator::op_left_shift;
static const int _right_shift			= (int)BinaryOperation::Operator::op_right_shift;
static const int _test_lower		    = (int)BinaryOperation::Operator::op_test_lower;
static const int _test_lower_or_equal   = (int)BinaryOperation::Operator::op_test_lower_or_equal;
static const int _test_greater		    = (int)BinaryOperation::Operator::op_test_greater;
static const int _test_greater_or_equal = (int)BinaryOperation::Operator::op_test_greater_or_equal;
static const int _test_equal			= (int)BinaryOperation::Operator::op_test_equal;
static const int _test_not_equal		= (int)BinaryOperation::Operator::op_test_not_equal;
static const int _op_and				= (int)BinaryOperation::Operator::op_and;
static const int _op_xor				= (int)BinaryOperation::Operator::op_xor;
static const int _op_or					= (int)BinaryOperation::Operator::op_or;

struct PrecedenceLevel {
	BinaryOperation_Operator ops[5];
};
// operators precedence : the same a C / C++
PrecedenceLevel PrecedenceLevels[] = {
	// ==> will be evaluated first.
	{_mult, _div, _modulo,													   invalid_operator}, // * / %
	{_add, _sub,															   invalid_operator}, // + -
	{_left_shift, _right_shift,												   invalid_operator}, // << >>
	{_test_lower, _test_lower_or_equal,	_test_greater, _test_greater_or_equal, invalid_operator}, // < <= > >=
	{_test_equal, _test_not_equal,											   invalid_operator}, // == !=
	{_op_and,																   invalid_operator}, // & and
	{_op_xor,																   invalid_operator}, // ^ xor
	{_op_or,																   invalid_operator}, // | or
	// will be evaluated last
};


// constructor
LangageAttributes::LangageAttributes(void) {
	// init operators precedence 
	int nb_level = sizeof(PrecedenceLevels) / sizeof(PrecedenceLevels[0]);
	int current_level = 1;
	for (int i = 0; i < nb_level; i++) {
		PrecedenceLevel level = PrecedenceLevels[i];
		for (int j = 0; level.ops[j] != invalid_operator; j++) {
			// add the operator to the map
			operator_precedence[level.ops[j]] = i + 1;
		}
	}
}
// comparae operator precedence
int LangageAttributes::compare_operator_precedence(BinaryOperation_Operator a, BinaryOperation_Operator b) const {
	// lookup for the precedence of the operator
	auto pos_a = operator_precedence.find(a);
	if (pos_a == operator_precedence.end()) {
		// not found
		assert(false);
		return 0;
	}
	int precedence_a = pos_a->second;
	auto pos_b = operator_precedence.find(b);
	if (pos_b == operator_precedence.end()) {
		// not found
		assert(false);
		return 0;
	}
	int precedence_b = pos_b->second;
	assert(precedence_a >= 0);
	assert(precedence_b >= 0);
	return (precedence_a - precedence_b);
}

