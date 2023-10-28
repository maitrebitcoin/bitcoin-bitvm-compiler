
#include "LangageGrammar.h"
#include "Compiler.h"
#include "TokenId.h"

// token definition (lexer)
static const char *REGEXP_IDENTIFIER = "[a-zA-Z_][a-zA-Z0-9_]*"; // ex : a, a1, a_1, _a1
std::vector<TokenDefinition> LangageGrammar::get_token_definition(void) {
	TokenDefinition token_definition[] =
	{
		{ TOKEN_TYPE_BOOL,  "bool"},
		{ TOKEN_TYPE_BYTE,  "byte"},
		{ TOKEN_RETURN,		"return"},
		{ TOKEN_IDENTIFIER_FNNAME,	nullptr, REGEXP_IDENTIFIER, [this]() {return !in_body && !in_fn_param; }},
		{ TOKEN_IDENTIFIER_FNPARAM,	nullptr, REGEXP_IDENTIFIER, [this]() {return !in_body && in_fn_param; }},
		{ TOKEN_IDENTIFIER_SETVAR  ,nullptr, REGEXP_IDENTIFIER, [this]() {return in_body && in_set_var_possible;}},
		{ TOKEN_IDENTIFIER,			nullptr, REGEXP_IDENTIFIER, [this]() {return in_body && !in_decl_localvar ; }},
		{ TOKEN_IDENTIFIER_LOCALVAR,nullptr, REGEXP_IDENTIFIER, [this]() {return in_body && in_decl_localvar ;  }},
		{ TOKEN_NUMBER,			nullptr, "[0-9]*"},
	};
	int nb_lex_rules = sizeof(token_definition) / sizeof(token_definition[0]);
	return std::vector<TokenDefinition>(token_definition, token_definition + nb_lex_rules);
}

// grammar 
std::vector<RuleDefinition> LangageGrammar::get_grammar_definition(void) {
RuleDefinition rules_definition[] =
{
	{ RULE_PROGRAM, { RULE_FUNCTION }, 
		[this](TokenValue& result, std::vector<TokenValue> p) { 
			result.program_value = new_program(); 
			result.program_value->add_function(p[0].function_value);
		}
	},

	// ex : bool fct_name(byte a, byte b) { return a+b; }
	{ RULE_FUNCTION, { RULE_FUNCTION_DEFINTION, RULE_CODEBLOC },
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.function_value = new_function(p[0].function_definition_value, p[1].code_block_value ); }
	},
	// ex : bool fct_name(bool a, byte b)
	{ RULE_FUNCTION_DEFINTION, { RULE_TYPE, TOKEN_IDENTIFIER_FNNAME, '(', RULE_N_PARAMETERS_DECL ,')'},
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
	{ RULE_1_PARAMETER_DECL , { RULE_TYPE, TOKEN_IDENTIFIER_FNPARAM } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.function_paramter_value = new_function_parameter(*p[0].type_value, *p[1].string_value);  }
	},
		// ex { a++; return a; }
	{ RULE_CODEBLOC , { '{', RULE_N_STATEMENTS, '}'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.code_block_value = p[1].code_block_value; }
	},
	// ex a++; return a;
	{ RULE_N_STATEMENTS , {RULE_N_STATEMENTS,  RULE_1_STATEMENT,  } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.code_block_value = p[0].code_block_value;
			result.code_block_value->add_statement((p[1].statement_value));
		}
	},
	{ RULE_N_STATEMENTS , {RULE_1_STATEMENT } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.code_block_value = new_code_bloc(p[0].statement_value);
		}
	},
	// statements -------------------
	// return
	// ex: return a;
	{ RULE_1_STATEMENT , {TOKEN_RETURN, RULE_EXPRESSION, ';'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_retun_statement(p[1].expression_value);
		}
	},
	// variable declaration
	// ex: int c;
	{ RULE_1_STATEMENT , {RULE_TYPE, TOKEN_IDENTIFIER_LOCALVAR, ';'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_declare_var_statement(p[0].type_value, *p[1].string_value);
		}
	},
	// variable affectation
	// ex: a = b+c;
	{ RULE_1_STATEMENT , {TOKEN_IDENTIFIER_SETVAR, '=', RULE_EXPRESSION, ';'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_set_var_statement(*p[0].string_value, p[2].expression_value);
		}
	},
	// variable declaration ands affectation
	// ex: bool a = b+c;
	{ RULE_1_STATEMENT , {RULE_TYPE, TOKEN_IDENTIFIER_LOCALVAR, '=', RULE_EXPRESSION, ';'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_declare_and_set_var_statement(p[0].type_value, *p[1].string_value,  p[3].expression_value);
		}
	},

	// all expressions : a,123,a&b,a-1+b
	{ RULE_EXPRESSION , {RULE_VARIABLE} ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_LITTERAL } ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_AND } ,	[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_OR }  ,	[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_XOR } ,	[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_NOT } ,	[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
		//a&b
	{ RULE_OPERATOR_AND , {RULE_EXPRESSION, '&', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_binary_operation(BinaryOperation::Operator::op_and, p[0].expression_value, p[2].expression_value);
		}
	},
	//a|b
	{ RULE_OPERATOR_OR , {RULE_EXPRESSION, '|', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_binary_operation(BinaryOperation::Operator::op_or, p[0].expression_value, p[2].expression_value);
		}
	},
	//a|b
	{ RULE_OPERATOR_XOR , {RULE_EXPRESSION, '^', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_binary_operation(BinaryOperation::Operator::op_xor, p[0].expression_value, p[2].expression_value);
		}
	},
	//a|b
	{ RULE_OPERATOR_NOT , {'!', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_unary_operation(UnaryOperation::Operator::op_not, p[1].expression_value);
		}
	},
	// ex: a
	{ RULE_VARIABLE , { TOKEN_IDENTIFIER } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_variable(*p[0].string_value);  }
	},
		// ex: 123
	{ RULE_LITTERAL , { TOKEN_NUMBER } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_literal(Type::Native::int8, *p[0].string_value);  }
	},
		// bool
	{ RULE_TYPE , { TOKEN_TYPE_BOOL } ,
		[this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_Type(Type::Native::bit);  }
	},
		// byte
	{ RULE_TYPE , { TOKEN_TYPE_BYTE } ,
		[this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_Type(Type::Native::int8);  }
	},
};
	int nb_grammar_rules = sizeof(rules_definition) / sizeof(rules_definition[0]);
	return std::vector<RuleDefinition>(rules_definition, rules_definition + nb_grammar_rules);
};

