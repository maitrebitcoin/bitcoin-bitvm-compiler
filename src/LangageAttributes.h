#pragma once

#include <map>

// somme attributes of the langage
using BinaryOperation_Operator = int;
class LangageAttributes {
protected:
	// return precdence number for a operation token. 0 = highest precedence, 100 = lowest precedence
	// key = , value = precedence number
	std::map < BinaryOperation_Operator, int > operator_precedence;

public:
	// constructor
	LangageAttributes();
	// comparae operator precedence. ex "+" vs "<<"
	int compare_operator_precedence(BinaryOperation_Operator operator_1, BinaryOperation_Operator operator_2) const;
};