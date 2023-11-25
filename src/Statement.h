#pragma once

class Scope;

// base class for a statement. 
// ex "v=a+2" or "return a+2;"
class Statement {
public:
	// lgne number in the source code
	int num_line = 0;
public:
	// constructor
	Statement(int line) : num_line(line) {}
	// init a statmenet
	virtual void init(Scope& parent_scope) {}
	// cast in Statement_Return
	virtual class Statement_Return* cast_to_Statement_Return(void) { return nullptr; }
	// cast in Statement_If
	virtual class Statement_If* cast_to_Statement_If(void) { return nullptr; }
	// cast in Statement_DeclareVar
	virtual class Statement_DeclareVar* cast_to_Statement_DeclareVarr(void) { return nullptr; }
	// cast to Statement_DeclareStruct
	virtual class Statement_DeclareStruct* cast_to_Statement_DeclareStruct(void)  { return nullptr; }
	// cast to Statement_DeclareAndSetVar
	virtual class Statement_DeclareAndSetVar* cast_to_Statement_DeclareAndSetVar(void) { return nullptr; }
	// cast to Statement_Increment
	virtual	class Statement_Increment* cast_to_Statement_Increment(void) { return nullptr; }
	// cast to Statement_Break
	virtual	class Statement_Break* cast_to_Statement_Break(void) { return nullptr; }

	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const = 0;

	// build the circuit for the return statelebt
	virtual void build_circuit(BuildContext& ctx) const = 0;

};
