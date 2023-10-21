
#include "LangageGrammar.h"
#include "Compiler.h"


// init token definition (lexer)
enum E_TokenId {
	TOKEN_TYPE_BOOL   =  257,
	TOKEN_TYPE_BYTE,  // 258
	TOKEN_IDENTIFIER, // 259
	TOKEN_NUMBER,	  // 260
	TOKEN_RETURN,	  // 261
};
TokenDefinition token_definition[] =
{
	{ TOKEN_TYPE_BOOL,  "bool"},
	{ TOKEN_TYPE_BYTE,  "byte"},
	{ TOKEN_RETURN,		"return"},
	{ TOKEN_IDENTIFIER, nullptr, "[a-zA-a_][a-zA-a0-9_]*"},
	{ TOKEN_NUMBER,		nullptr, "[0-9]*"},
	{ TOKEN_NUMBER,		nullptr, "[0-9]*"},
};
// grammar elements
enum E_RuleId {
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
			[this](TokenValue& result, std::vector<TokenValue> p) {
				result.function_definition_value = new_function_definition(*p[0].type_value, *p[1].string_value, p[3].function_all_paramters_value);  }
		},
		// ex: bool a, byte b
		{ RULE_N_PARAMETERS_DECL , { RULE_N_PARAMETERS_DECL, ',', RULE_1_PARAMETER_DECL } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {  result.function_all_paramters_value = p[0].function_all_paramters_value;  
																 p[0].function_all_paramters_value->add(*p[2].function_paramter_value);  }
		},
		// ex : bool a
		{ RULE_N_PARAMETERS_DECL , { RULE_1_PARAMETER_DECL },  
			[this](TokenValue& result, std::vector<TokenValue> p) { result.function_all_paramters_value = new_function_all_parameters(p[0].function_paramter_value);  }
		},
		// ex : bool a 
		{ RULE_1_PARAMETER_DECL , { RULE_TYPE, TOKEN_IDENTIFIER } ,
			[this](TokenValue& result, std::vector<TokenValue> p) { result.function_paramter_value = new_function_parameter(*p[0].type_value, *p[1].string_value);  }
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
		// ex: return a;
		{ RULE_1_STATEMENT , {TOKEN_RETURN, TOKEN_IDENTIFIER, ';' } ,
			nullptr,
		},
		// bool
		{ RULE_TYPE , { TOKEN_TYPE_BOOL } ,
			[this](TokenValue& result, std::vector<TokenValue> param) { result.type_value = new_Type(Type::Native::bit);  }
		},
		// byte
		{ RULE_TYPE , { TOKEN_TYPE_BYTE } ,
			[this](TokenValue& result, std::vector<TokenValue> param) { result.type_value = new_Type(Type::Native::int8);  }
		},
	};
	int nb_grammar_rules = sizeof(rules_definition) / sizeof(rules_definition[0]);
	return std::vector<RuleDefinition>(rules_definition, rules_definition+nb_grammar_rules);
};
