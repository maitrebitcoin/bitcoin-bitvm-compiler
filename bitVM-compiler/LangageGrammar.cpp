
#include "LangageGrammar.h"
#include "Compiler.h"


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
// grammar elements
enum RuleType {
	RULE_FUNCTION = 1000,
	RULE_FUNCTION_DEFINTION, // ex : bool f(byte a, byte b) 
	RULE_TYPE,							// ex : bool
	RULE_N_PARAMETERS_DECL,				// ex : (bool a, byte b)		
	RULE_1_PARAMETER_DECL,				// ex : { return a+b; }
	RULE_CODEBLOC,
	RULE_N_STATEMENTS,
	RULE_1_STATEMENT,
	RULE_OPERATION,
	RULE_INSTRUCITON_RETURN,
};
// get the token definition
std::pair<TokenDefinition*, int> LangageGrammar::get_token_definition(void) {
	int nb_token_rules = sizeof(token_definition) / sizeof(token_definition[0]);
	return { token_definition, nb_token_rules };
}


std::vector<RuleDefinition> LangageGrammar::get_grammar_definition(void) {
	RuleDefinition rules_definition[] =
	{
		// ex : bool fct_name(byte a, byte b) { return a+b; }
		{ RULE_FUNCTION, { RULE_FUNCTION_DEFINTION, RULE_CODEBLOC }, nullptr },
		// ex : bool fct_name(bool a, byte b)
		{ RULE_FUNCTION_DEFINTION, { RULE_TYPE, TOKEN_IDENTIFIER, '(', RULE_N_PARAMETERS_DECL ,')'},
			[this](TokenValue& result, std::vector<TokenValue> param) { result.function_definition_value = new Function::Definition(*param[0].type_value, *param[1].string_value);  }
		},
		// ex: bool a, byte b
		{ RULE_N_PARAMETERS_DECL , { RULE_N_PARAMETERS_DECL, ',', RULE_1_PARAMETER_DECL } ,
			nullptr
		},
		// ex : bool a
		{ RULE_N_PARAMETERS_DECL , { RULE_1_PARAMETER_DECL },  nullptr,
		},
		// ex : bool a 
		{ RULE_1_PARAMETER_DECL , { RULE_TYPE, TOKEN_IDENTIFIER } ,
			[this](TokenValue& result, std::vector<TokenValue> param) { result.function_paramter_value = new Function::Parameter(*param[0].type_value, *param[1].string_value);  }
		},
		// ex { a++; return a; }
		{ RULE_CODEBLOC , { '{', RULE_N_STATEMENTS, '}'} ,
			nullptr,
		},
		// ex { return a; }
		{ RULE_N_STATEMENTS , {RULE_N_STATEMENTS, RULE_1_STATEMENT } ,
			nullptr,
		},
		{ RULE_N_STATEMENTS , {RULE_1_STATEMENT } ,
			nullptr,
		},
		{ RULE_1_STATEMENT , {RULE_INSTRUCITON_RETURN, TOKEN_IDENTIFIER, ';' } ,
			nullptr,
		},
		// ex : bool
		{ RULE_TYPE , { TOKEN_TYPE_BOOL } ,
			[this](TokenValue& result, std::vector<TokenValue> param) { result.type_value = new Type(Type::Native::bit);  }
		},

	};
	int nb_grammar_rules = sizeof(rules_definition) / sizeof(rules_definition[0]);
	return std::vector<RuleDefinition>(rules_definition, rules_definition+nb_grammar_rules);
};
