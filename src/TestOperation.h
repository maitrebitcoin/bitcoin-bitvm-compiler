#pragma once

#include "BinaryOperation.h"

// test operator ex : "a==b"
// bit shit expression. ex :"a<<3"
class TestOperation : public BinaryOperation {
public:
	// constructor
	TestOperation(Operator op, Expression* left, Expression* right) : BinaryOperation(op, left, right) {};
	// init
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the binairy expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
	// build the circuit for the "==" expression
	static std::vector<Connection*>  build_circuit_equal(BuildContext& ctx,
		std::vector<Connection*>& in_a,
		std::vector<Connection*>& in_b);

};