#pragma once

#include "BinaryOperation.h"

// bit shit expression. ex :"a<<3"
class ShiftOperation : public BinaryOperation {
protected:
	// value of the lirreral R agufment
	int nb_sihft = 0;
public:
	// constructor
	ShiftOperation(Operator op, Expression* left, Expression* right) : BinaryOperation(op, left, right) {};
	// init
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};
