#pragma once
#include <string>
#include <vector>
#include <assert.h>
#include "Program.h"
#include "ParsingContext.h"

struct TokenDefinition;
struct RuleDefinition;

// to keep all string an objet crated during parsing alive
class ObjetKeeper : public ParsingContext{

protected:
	// strings
	std::vector<std::string *> strings;
	// types
	std::vector<Type*> Types;
	// 1 function parameters
	std::vector<Function::Parameter*> function_parameters;
	// all function parameters
	std::vector<Function::AllParameter*> function_all_parameters;
	// function definitions
	std::vector<Function::Definition*> function_definitions;
	// binairy operations, variables & litterals
	std::vector<Expression*> expressions;
	// code blocks
	std::vector<CodeBloc*> code_blocs;
	// statements
	std::vector<Statement*> statements;
	// program
	Program* program = nullptr;
	// fuctions
	std::vector<Function*> functions;

public:
	// destuctor
	~ObjetKeeper(void) {
		free_all();
	}
	// get a new string
	std::string*	new_string(const char *c_string) {
		std::string* new_string = new std::string(c_string);
		strings.push_back(new_string);
		return new_string;
	}	
	// get a new Type 
	Type* new_Type(Type::Native t) {
		Type* new_type = new Type(t);
		Types.push_back(new_type);
		return new_type;
	}
	// get a new function parameter
	Function::Parameter* new_function_parameter(Type type, std::string name) {
		Function::Parameter* new_parameter = new Function::Parameter(type, name);
		function_parameters.push_back(new_parameter);
		return new_parameter;
	}
	// get a new function definition
	Function::Definition* new_function_definition(Type type, std::string name, Function::AllParameter* all_params ) {
		Function::Definition* new_definition = new Function::Definition(type, name, all_params);
		function_definitions.push_back(new_definition);
		return new_definition;
	}
	// get a new function all parameters
	Function::AllParameter* new_function_all_parameters(Function::Parameter* p0) {
		Function::AllParameter* new_all_parameters = new Function::AllParameter(*p0);
		function_all_parameters.push_back(new_all_parameters);
		return new_all_parameters;
	}
	// get a new binairy operation
	BinaryOperation* new_binary_operation(BinaryOperation::Operator op, Expression* left, Expression* right)
	{
		BinaryOperation* new_operation = new BinaryOperation(op,left,right);
		expressions.push_back(new_operation);
		return new_operation;
	}
	// get a new unairy operation
	UnaryOperation* new_unary_operation(UnaryOperation::Operator op, Expression* expr) {
		UnaryOperation* new_operation = new UnaryOperation(op, expr);
		expressions.push_back(new_operation);
		return new_operation;
	}

	// get a new variable
	Variable* new_variable( std::string name) {
		Variable* new_variable = new Variable( name);
		expressions.push_back(new_variable);
		return new_variable;
	}
	// get a new litteral
	Literal* new_literal(Type type, std::string name) {
		Literal* new_literal = new Literal(type, name);
		expressions.push_back(new_literal);
		return new_literal;
	}
	// get a new function
	Function* new_function(Function::Definition* def, CodeBloc* body) {
		Function* new_function = new Function(def, body);
		functions.push_back(new_function);
		return new_function;
	}
	// get a new code block
	CodeBloc* new_code_bloc(Statement* fisrt_s) {
		assert(fisrt_s!=nullptr);
		CodeBloc* new_code_block = new CodeBloc(fisrt_s);
		code_blocs.push_back(new_code_block);
		return new_code_block;
	}
	// get a new  return statement
	Statement* new_retun_statement(Expression* expr) {
		Statement_Return* new_return_statement = new Statement_Return(get_current_line_number(), expr);
		statements.push_back(new_return_statement);
		assert(new_return_statement->is_return());
		return new_return_statement;
	}
	// get a new decalre var statement
	Statement* new_declare_var_statement(Type *type, std::string name) {
		Statement_DeclareVar* new_declare_var_statement = new Statement_DeclareVar(get_current_line_number(), type, name);
		statements.push_back(new_declare_var_statement);
		return new_declare_var_statement;
	}

	// get a new program
	Program* new_program(void) {
		Program* new_program = new Program();
		assert(program== nullptr);
		program = new_program;
		return new_program;
	}

	// free all 
	void free_all(void) {
		for (std::string* s: strings) {
			delete s;
		}
		for (Type* t : Types) {
			delete t;
		}
		for (Function::Parameter* p : function_parameters) {
			delete p;
		}
		for (Function::AllParameter* p : function_all_parameters) {
			delete p;
		}
		for (Function::Definition* d : function_definitions) {
			delete d;
		}
		for (auto* ptr : expressions) {
			delete ptr;
		}
		for (Function* f : functions) {
			delete f;
		}
		for (CodeBloc* c : code_blocs) {
			delete c;
		}
		for (Statement* s : statements) {
			delete s;
		}
	}
};
class CToken;



// definition of the language grammar 
// and parsing context
class LangageGrammar : public ObjetKeeper {

public:
	// get the token definition
	std::vector<TokenDefinition> get_token_definition(void);
	// get the grammar definition
	std::vector<RuleDefinition> get_grammar_definition(void);



};

