// implémentation of the compiler

#include "Compiler.h"
#include <sstream>
#include <vector>
#include <assert.h>
#include <regex>
#include <functional>

// constructor
Compiler::Compiler(void) {
	_init_grammar();
}


// type of a variable or function
class Type {
public:
	enum class Native {
		undefined,	// not set/ invalide
		bit,		// bool
		int8,		// char
		uint8,		// byte / unsigned char
		int64,		// long long
		uint64,		// unsigned long long
		uint256,	// unsigned __int256
	};
protected:
	Native native_type = Native::undefined;
public:
	// constructor
	Type(Native t): native_type(t) {}

	// return the size in bits of the type
	int size_in_bit(void) const;

};
// return the size in bits of the type
int Type::size_in_bit(void) const
{
	switch (native_type)	
	{
	case Type::Native::undefined:
		assert(false);
		return 0;
	case Type::Native::bit:    return 1;
	case Type::Native::int8:   return 8;
	case Type::Native::uint8:  return 8;
	case Type::Native::int64:  return 64;
	case Type::Native::uint64: return 64;
	case Type::Native::uint256:return 256;
	default:
		assert(false);
		return 0;
	}
}

class Operand;
// Math expression. ex :"a+2"
class Expression {
protected:
	// type of the result
	Type result_type;
	// opération
	enum class Operation {
		op_and,
		op_or,
		op_not,
	};
	Operation operation;
	// left operand
	Operand* left_operand;
	// right operand
	Operand* right_operand;
	// expression as  string for debug purposes
	std::string expression_for_debug;
};

// base class for a statement. 
// ex "v=a+2" or "return a+2;"
class StatementBase {
protected:

};
// "return" statement
class Statement_Return : public StatementBase {
protected:
	// expression to return
	Expression expression;

};



// represents a function, ex : bool main(bool a, bool b) { return a & b; }
class Function {
public:
	// parameters of the function
	struct Parameter {
		Type type;
		std::string name;
		// constructor
		Parameter(Type t, std::string n) : type(t), name(n) {}
	};
protected:
	// name of the function
	std::string name;
	// return type of the function
	Type return_type;

	std::vector<Parameter> parameters;

public:
	// return the number of bits needed for the input parameters
	int size_in_bit_input(void) const;
	// return the number of bits needed to store the return value
	int size_in_bit_output(void) const;

};
// return the number of bits needed for the input parameters
int Function::size_in_bit_input(void) const
{
	// addd the size of each parameter
	int nb_bit = 0;
	for (Parameter param_i: parameters)
		nb_bit += param_i.type.size_in_bit();
	return nb_bit;
}
// return the number of bits needed to store the return value
int Function::size_in_bit_output(void) const {
	// size of the return type
	return return_type.size_in_bit();
}



// compile a memory stream
bool Compiler::compile(std::istream& source_code, Error& error_out)
{
	// read the source code
	static const int max_line_size = 1024;
	char line_raw[max_line_size];
	int cureent_line_number = 0;
	while (source_code.getline(line_raw, max_line_size)) {
		// compile 1 line
		std::string source_code_at_line(line_raw);
		try {
			_compile_line(source_code_at_line);
		}
		catch (Error& compiler_error)	{
			// compilation failed
			error_out = compiler_error;
			error_out.message += " : " + source_code_at_line; // std::to_string(cureent_line_number);
			error_out.line_number = cureent_line_number;
			return false;
		}
		// next line
		cureent_line_number++;
	}
	// OK
	return true;
}



struct TokenDefinition {
	TokenType   token_type;
	const char* token_value;
	const char* regex;
};

// init token definition (lexer)
#define TOKEN_TYPE_BOOL  257
#define TOKEN_TYPE_BYTE  258
#define TOKEN_IDENTIFIER 258
#define TOKEN_NUMBER	 259
TokenDefinition token_definition[] =
{
	{ TOKEN_TYPE_BOOL,  "bool"},
	{ TOKEN_TYPE_BYTE,  "byte"},
	{ TOKEN_IDENTIFIER, nullptr, "[a-zA-a_][a-zA-a0-9_]*"},
	{ TOKEN_NUMBER,		nullptr, "[0-9]*"},
};

// init grammar
#define RULE_FUNCTION			 1000
#define RULE_TYPE				 1001  // ex : bool
#define RULE_N_PARAMETERS_DECL	 1002  // ex : (bool a, byte b)
#define RULE_1_PARAMETER_DECL	 1003  // ex : bool a 
#define RULE_CODEBLOC			 1004  // ex : { return a+b; }
#define RULE_N_STATEMENTS		 1005
#define RULE_1_STATEMENT		 1006
#define RULE_OPERATION			 1007
#define RULE_INSTRUCITON_RETURN	 1100

void Compiler::_init_grammar(void) 
{
	RuleDefinition rules_definition[] =
	{
		// ex : bool fct_name(byte a, byte b) { return a+b; }
		{ RULE_FUNCTION, { RULE_TYPE, TOKEN_IDENTIFIER, '(', RULE_N_PARAMETERS_DECL ,')', RULE_CODEBLOC }, nullptr },
		// ex : (bool a, byte b)
		{ RULE_N_PARAMETERS_DECL , { RULE_N_PARAMETERS_DECL, ',', RULE_1_PARAMETER_DECL } , nullptr },
		{ RULE_N_PARAMETERS_DECL , { RULE_1_PARAMETER_DECL }  , nullptr  },
		// ex : bool a 
		{ RULE_1_PARAMETER_DECL , { RULE_TYPE, TOKEN_IDENTIFIER } ,   
			[this](TokenValue& result, std::vector<TokenValue> param) { result.function_paramter_value = new Function::Parameter(*param[0].type_value, *param[1].string_value);  }
		},
		// ex : bool
		{ RULE_TYPE , { TOKEN_TYPE_BOOL } , 
			[this](TokenValue& result, std::vector<TokenValue> param) { result.type_value = new Type(Type::Native::bit);  }
		},
		//{ RULE_TYPE , { TOKEN_TYPE_BYTE } , nullptr  },
	};
	int nb_rules = sizeof(rules_definition) / sizeof(rules_definition[0]);
	// init the grammar array
	grammar_rules = std::vector<RuleDefinition>(rules_definition, rules_definition + nb_rules);

}

// test if a rule is matching the current stack
bool Compiler::is_rule_matching_stack(RuleDefinition& rule) const {
	// check if the stack is big enough
	size_t rule_size = rule.tokens.size();
	if (token_stack.size()< rule_size)
		return false;
	// check if the rule match the top of the stack
	size_t stack_top = token_stack.size() - rule_size;
	for (int i=0; i< rule_size; i++) {
		if (rule.tokens[i] != token_stack[stack_top+i].type)
			return false;
	}
	// matching ok
	return true;
}	

// find a matching rule from the token stack
RuleDefinition* Compiler::_find_matching_rule_from_stack(void)  {
	// for each rule in the grammar
	for ( RuleDefinition& rule : grammar_rules) {
		// check if the rule match the token stack
		if (is_rule_matching_stack(rule))
			return &rule;
	}
	// no rule found
	return nullptr;
}
// call a rule and remove the token from the stack
void Compiler::_execute_rule(RuleDefinition& rule) {
	size_t rule_size = rule.tokens.size();
	size_t stack_top = token_stack.size() - rule_size;
	assert(stack_top >= 0);
	// get values form the stack
	std::vector<TokenValue> values;
	for (int i = 0; i < rule_size; i++) {
		values.push_back(token_stack[stack_top + i].value);
	}
	// call the rule to set token_result.value 
	CToken token_result(rule.rule_id);
	if (rule.action != nullptr)
		rule.action(token_result.value, values);

	// remove the token from the top of the stack
	for (int i = 0; i < rule_size; i++) {
		token_stack.pop_back();
	}
	// push the result on the stack
	token_stack.push_back(token_result);

}


// get 1 token from the line
CToken Compiler::_get_next_token(std::string& code_in_out) {

	// ignore white space and tab
	while (code_in_out[0] == ' ' || code_in_out[0] == '\t')
		code_in_out.erase(0, 1);

	// if the string is empty, return an empty token
	if (code_in_out.size() == 0)
		return CToken(0, "");

	// check if the string star wiht of of the known token
	for (TokenDefinition definition : token_definition) {
		// ex "bool"
		if (definition.token_value != nullptr) {
			// check if the string start with the token
			if (code_in_out.compare(0, strlen(definition.token_value), definition.token_value) == 0) {
				// remove the token from the string
				code_in_out.erase(0, strlen(definition.token_value));
				// return the token
				return CToken(definition.token_type, definition.token_value);
			}
		}
		// ex "[0-9*]"
		else {
			assert(definition.regex != nullptr);
			//  check if the string start with the token with definition.regex
			std::string regex_definition = definition.regex;
			std::regex regex_i(regex_definition);
			std::smatch regex_match;
			if (std::regex_search(code_in_out, regex_match, regex_i)) {
				std::string match_string = regex_match[0];
				// check if the string start with the matched part in the regexp 
				if (match_string.size() > 0 && code_in_out.compare(0, match_string.size(), match_string) == 0) {
					// remove the token from the string
					code_in_out.erase(0, match_string.size());
					// return the token
					return CToken(definition.token_type, match_string);
				}
			}
		}
	} // for each token definition

	// send the first char as a token
	char c = code_in_out[0];
	char tok_c[] = { c,0 };
	code_in_out.erase(0, 1);
	return CToken(c, tok_c);
}


// compile 1 line of code
void Compiler::_compile_line(std::string line_of_code) {
	// ignore empty lines
	if (line_of_code.size() == 0) return;



	// read all the tokens
	while (1) {
		// get a token for the line
		CToken token = _get_next_token(line_of_code);
		if (token.type == 0)
			return;
		// add to stack	
		token_stack.push_back(token);
		// check if we have a matching rule
		RuleDefinition* rule = _find_matching_rule_from_stack();
		while (rule != nullptr) {
			// call the rule, remove the token from the stack and push the result
			_execute_rule(*rule);
			// do it utile the no more match
			rule = _find_matching_rule_from_stack();
		}

	}

		
	// TODO : parse the line
	throw Error("syntax error");
}
