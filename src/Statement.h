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
	virtual bool is_return(void) const { return false; }
	// cast in Statement_DeclareVar
	virtual class Statement_DeclareVar* cast_to_Statement_DeclareVarr(void) { return nullptr; }
	// cast to Statement_DeclareStruct
	virtual class Statement_DeclareStruct* cast_to_Statement_DeclareStruct(void)  { return nullptr; }

	// build the circuit for the return statelebt
	virtual void build_circuit(BuildContext& ctx) const = 0;

};