
#include <string>
#include "LangageGrammar.h"
#include "Compiler.h"
#include "TokenId.h"
#include "Program.h"
#include "ParsingContext.h"


// token definition (lexer)
static const char *REGEXP_IDENTIFIER = "[a-z][a-zA-Z0-9_]*"; // ex : a, a1, a_1, _a1
static const char * REGEXP_USERTYPE = "[A-Z_][a-zA-Z0-9_]*"; // ex : Header, Bloc, ...
std::vector<TokenDefinition> LangageGrammar::get_token_definition(void) {
	TokenDefinition token_definition[] =
	{
		// types
		{ TOKEN_TYPE_BOOL,			"bool"},
		{ TOKEN_TYPE_INT8,			"int8"},
		{ TOKEN_TYPE_UINT8,			"uint8"},
		{ TOKEN_TYPE_INT32,			"int32"},
		{ TOKEN_TYPE_UINT32,		"uint32"},
		{ TOKEN_TYPE_INT64,			"int64"},
		{ TOKEN_TYPE_UINT64,		"uint64"},
		{ TOKEN_TYPE_INT256,		"int256"},
		{ TOKEN_TYPE_UINT256,		"uint256"},
		// keyword
		{ TOKEN_RETURN,				"return"},
		{ TOKEN_TRUE,				"true"},
		{ TOKEN_FALSE,				"false"},
		{ TOKEN_KEYWORKD_STRUCT,	"struct"},
		{ TOKEN_KEYWORKD_IF,		"if"},
		{ TOKEN_KEYWORKD_FOR,		"for"},
		{ TOKEN_KEYWORKD_BREAK,		"break"},
		{ TOKEN_LEFT_SHIFT,			"<<"},
		{ TOKEN_RIGHT_SHIFT,		">>"},
		{ TOKEN_TEST_EQUAL,			"=="},
		{ TOKEN_TEST_NOTEQUAL,		"!="},
		{ TOKEN_TEST_LOWEROREQ,		"<="},
		{ TOKEN_TEST_LOWER,			"<"},
		{ TOKEN_TEST_GREATEROREQ,	">="},
		{ TOKEN_TEST_GREATER,		">"},
		{ TOKEN_INCREMENT,			"++"},
		{ TOKEN_DECREMENT,			"--"},
		{ TOKEN_IDENTIFIER_FNNAME,	nullptr, REGEXP_IDENTIFIER, [this](char next_char) {return !in_body && !in_fn_param && !in_declare_struct; }},
		{ TOKEN_IDENTIFIER_FNPARAM,	nullptr, REGEXP_IDENTIFIER, [this](char next_char) {return !in_body && in_fn_param; }},
		{ TOKEN_IDENTIFIER_SETVAR  ,nullptr, REGEXP_IDENTIFIER, [this](char next_char) {return in_body && in_set_var_possible && next_char == '='; }},
		{ TOKEN_IDENTIFIER,			nullptr, REGEXP_IDENTIFIER, [this](char next_char) {return (in_body && !in_decl_localvar) && next_char != '.' && !in_use_struct; }},
		{ TOKEN_IDENTIFIER_DECL,	nullptr, REGEXP_IDENTIFIER, [this](char next_char) {return (in_body && in_decl_localvar);   }},
		{ TOKEN_DECLARE_STRUCT_NAME,nullptr, REGEXP_USERTYPE,	 [this](char next_char) {return in_declare_struct;  }},
		{ TOKEN_USE_STRUCT,			nullptr, REGEXP_IDENTIFIER,  [this](char next_char) {return next_char == '.'; } },
		{ TOKEN_USE_STRUCT_MEMBER,	nullptr, REGEXP_IDENTIFIER,  [this](char next_char) {return in_use_struct; } },
		{ TOKEN_STRUCT_TYPE,		nullptr, REGEXP_USERTYPE,    [this](char next_char) {return next_char != '.' && !in_declare_struct; } },
		{ TOKEN_HEXANUMBER,			nullptr, "0x[0-9A-Fa-f]*"},
		{ TOKEN_NUMBER,				nullptr, "[0-9]*"},

	};
	int nb_lex_rules = sizeof(token_definition) / sizeof(token_definition[0]);
	return std::vector<TokenDefinition>(token_definition, token_definition + nb_lex_rules);
}

// grammar 
std::vector<RuleDefinition> LangageGrammar::get_grammar_definition(void) {
RuleDefinition rules_definition[] =
{
	// struct + 1 function
	{ RULE_PROGRAM, { RULE_N_STATEMENTS, RULE_N_FUNCTION },
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.program_value = new_program();
			result.program_value->add_struct_definition(p[0].code_block_value);
			result.program_value->add_array_function(p[1].function_array_value);
		},
		[this](void) { return next_token_type == 0; } // end of fils

	},
	// just 1 function
	{ RULE_PROGRAM, { RULE_N_FUNCTION }, 
		[this](TokenValue& result, std::vector<TokenValue> p) { 
			result.program_value = new_program();
			result.program_value->add_array_function(p[0].function_array_value);
		},
		[this](void) { return next_token_type == 0; } // end of fils

	},
	// N Functions
	{ RULE_N_FUNCTION, { RULE_N_FUNCTION, RULE_FUNCTION },
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.function_array_value = p[0].function_array_value;
			result.function_array_value->push_back(p[1].function_value);
		}
	},
	// N Functions
	{ RULE_N_FUNCTION, { RULE_FUNCTION },
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.function_array_value = new_functions_array();
			result.function_array_value->push_back(p[0].function_value);

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
			result.function_definition_value = new_function_definition(p[0].type_value, *p[1].string_value, p[3].function_all_paramters_value);  }
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
		[this](TokenValue& result, std::vector<TokenValue> p) { result.function_paramter_value = new_function_parameter(p[0].type_value, *p[1].string_value);  }
	},
	// ex { a++; return a; }
	{ RULE_CODEBLOC , { '{', RULE_N_STATEMENTS, '}'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.code_block_value = p[1].code_block_value; }
	},
	// ex a++; return a;
	{ RULE_N_STATEMENTS , {RULE_N_STATEMENTS,  RULE_STATEMENT_WITH_END  } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.code_block_value = p[0].code_block_value;
			result.code_block_value->add_statement((p[1].statement_value));
		}
	},
	{ RULE_N_STATEMENTS , {RULE_STATEMENT_WITH_END } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.code_block_value = new_code_bloc(p[0].statement_value);
		},
		[this](void) { return !in_for_statement; } // pre-condition to prevent regroupin statements in for loop
	},
	// statements -------------------
	//ex : a++; or  return a;
	{ RULE_STATEMENT_WITH_END , {RULE_1_STATEMENT,  ';'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.statement_value = p[0].statement_value; }
	},
	// return
	// 
	// ex: return a
	{ RULE_1_STATEMENT , {TOKEN_RETURN, RULE_EXPRESSION} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_retun_statement(p[1].expression_value);
		},
		[this](void) { return next_token_type == ';'; } // pre-condition needed until the parse can backtrack. ex : "return a+b" is confused with "return a" 
	},
	// function call. ex func(a)
	{ RULE_1_STATEMENT , {TOKEN_IDENTIFIER_FNNAME, '(', ')'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_function_call_statement(*p[0].string_value);
		},
		[this](void) { return next_token_type == ';'; } // pre-condition needed until the parse can backtrack. ex : "return a+b" is confused with "return a" 
	},
	// variable declaration
	// ex: int c;
	{ RULE_1_STATEMENT , {RULE_TYPE, TOKEN_IDENTIFIER_DECL} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_declare_var_statement(p[0].type_value, *p[1].string_value);
		},
		[this](void) { return next_token_type == ';'; } // pre-condition needed until the parse can backtrack
	},
	// variable affectation
	// ex: a = b+c;
	{ RULE_1_STATEMENT , {TOKEN_IDENTIFIER_SETVAR, '=', RULE_EXPRESSION} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_set_var_statement(*p[0].string_value, p[2].expression_value);
		},
		[this](void) { return next_token_type == ';'; } // pre-condition needed until the parse can backtrack
	},
	// variable declaration ands affectation
	// ex: bool a = b+c;
	{ RULE_1_STATEMENT , {RULE_TYPE, TOKEN_IDENTIFIER_DECL, '=', RULE_EXPRESSION} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_declare_and_set_var_statement(p[0].type_value, *p[1].string_value,  p[3].expression_value);
		},
		[this](void) { return next_token_type == ';'; } // pre-condition needed until the parse can backtrack
	},
	// var++ / var--
	{ RULE_1_STATEMENT , {RULE_STATEMENT_INCREMENT} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = p[0].expression_value; 
		}
	},
	// var++
	{ RULE_STATEMENT_INCREMENT , {RULE_EXPRESSION, TOKEN_INCREMENT} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_increment_statement(*p[0].expression_value, true);
		}
	},
	// var--
	{ RULE_STATEMENT_INCREMENT , {RULE_EXPRESSION, TOKEN_DECREMENT} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_increment_statement(*p[0].expression_value, false);
		}
	},
	// strucutre declaration
	// ex: struct Header { byte zize; bool is_ok; }
	{ RULE_1_STATEMENT , {TOKEN_KEYWORKD_STRUCT, TOKEN_DECLARE_STRUCT_NAME, RULE_CODEBLOC} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_declare_struct_statement(*p[1].string_value, p[2].code_block_value);
		}
	},
	// if (a) {code}
	// ex: struct Header { byte zize; bool is_ok; }
	{ RULE_STATEMENT_WITH_END , {TOKEN_KEYWORKD_IF, RULE_EXPRESSION, RULE_CODEBLOC} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_if_statement(p[1].expression_value, p[2].code_block_value);
		}
	},
	// for (int8 i=0; i<10; i++) {code}
	{ RULE_STATEMENT_WITH_END , {TOKEN_KEYWORKD_FOR, '(', RULE_STATEMENT_WITH_END, RULE_EXPRESSION, ';', RULE_1_STATEMENT, ')', RULE_CODEBLOC  } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_for_statement(p[2].statement_value,	// int8 i=0;
													   p[3].expression_value,	// i<10;
													   p[5].statement_value,	// i++
													   p[7].code_block_value);	// {code}
		}
	},
	// break	
	{ RULE_1_STATEMENT , {TOKEN_KEYWORKD_BREAK} ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.statement_value = new_break_statement();
		}
	},

			

	// all expressions : a,123,a&b,a-1+b
	{ RULE_EXPRESSION , {RULE_VARIABLE} ,			[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_LITTERAL } ,			[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_AND } ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_OR }  ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_XOR } ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_NOT } ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_ADD } ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_SUB } ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_COMPLEMENT} ,[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_SHIFT} ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	{ RULE_EXPRESSION , {RULE_OPERATOR_CMP} ,		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = p[0].expression_value; } },
	//(a)
	{ RULE_EXPRESSION , { '(',RULE_EXPRESSION, ')'},[this](TokenValue& result, std::vector<TokenValue> p) { 
			result.expression_value = p[1].expression_value; 
			result.expression_value->set_parentesis(true);
		}
	},
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
	//a+b
	{ RULE_OPERATOR_ADD , {RULE_EXPRESSION, '+', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_binary_operation(BinaryOperation::Operator::op_add, p[0].expression_value, p[2].expression_value);
		}
	},
	//a-b
	{ RULE_OPERATOR_SUB , {RULE_EXPRESSION, '-', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_binary_operation(BinaryOperation::Operator::op_sub, p[0].expression_value, p[2].expression_value);
		}
	},
	//a<<2
	{ RULE_OPERATOR_SHIFT , {RULE_EXPRESSION, TOKEN_LEFT_SHIFT, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_shift_operation(BinaryOperation::Operator::op_left_shift, p[0].expression_value, p[2].expression_value);
		}
	},
	//a>>2
	{ RULE_OPERATOR_SHIFT , {RULE_EXPRESSION, TOKEN_RIGHT_SHIFT, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_shift_operation(BinaryOperation::Operator::op_right_shift, p[0].expression_value, p[2].expression_value);
		}
	},
	//a==b
	{ RULE_OPERATOR_CMP , {RULE_EXPRESSION, TOKEN_TEST_EQUAL, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_test_operation(BinaryOperation::Operator::op_test_equal, p[0].expression_value, p[2].expression_value);
		}
	},
	//a!=b
	{ RULE_OPERATOR_CMP , {RULE_EXPRESSION, TOKEN_TEST_NOTEQUAL, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_test_operation(BinaryOperation::Operator::op_test_not_equal, p[0].expression_value, p[2].expression_value);
		}
	},		
	//a<b
	{ RULE_OPERATOR_CMP , {RULE_EXPRESSION, TOKEN_TEST_LOWER, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_test_operation(BinaryOperation::Operator::op_test_lower, p[0].expression_value, p[2].expression_value);
		}
	},	
	//a<=b
	{ RULE_OPERATOR_CMP , {RULE_EXPRESSION, TOKEN_TEST_LOWEROREQ, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_test_operation(BinaryOperation::Operator::op_test_lower_or_equal, p[0].expression_value, p[2].expression_value);
		}
	},	
	//a>b
	{ RULE_OPERATOR_CMP , {RULE_EXPRESSION, TOKEN_TEST_GREATER, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_test_operation(BinaryOperation::Operator::op_test_greater, p[0].expression_value, p[2].expression_value);
		}
	},	
	//a>=b
	{ RULE_OPERATOR_CMP , {RULE_EXPRESSION, TOKEN_TEST_GREATEROREQ, RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_test_operation(BinaryOperation::Operator::op_test_greater_or_equal, p[0].expression_value, p[2].expression_value);
		}
	},	
			
	//!a
	{ RULE_OPERATOR_NOT , {'!', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_unary_operation(UnaryOperation::Operator::op_not, p[1].expression_value);
		}
	},
	//-a : minus a
	{ RULE_OPERATOR_NOT , {'-', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_unary_operation(UnaryOperation::Operator::op_negate, p[1].expression_value);
		}
	},
	//-a : a'complement
	{ RULE_OPERATOR_COMPLEMENT , {'~', RULE_EXPRESSION } ,
		[this](TokenValue& result, std::vector<TokenValue> p) {
			result.expression_value = new_unary_operation(UnaryOperation::Operator::op_complement, p[1].expression_value);
		}
	},
			
	// ex: a
	{ RULE_VARIABLE , { TOKEN_IDENTIFIER } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_variable_expression(*p[0].string_value);  },
		[this](void) { return next_token_type != '['; } // pre-condition to prevent a[3] to be interpreted as a variable
	},
	// ex: struct_header.a
	{ RULE_VARIABLE , { TOKEN_USE_STRUCT ,'.', TOKEN_USE_STRUCT_MEMBER} ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_struct_member(*p[0].string_value, *p[2].string_value);  }
	},
	// ex: a[4]
	{ RULE_VARIABLE , { TOKEN_IDENTIFIER, '[', RULE_EXPRESSION, ']'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_array_element(*p[0].string_value, p[2].expression_value);  }
	},

	// - literals
	// ex: 123
	{ RULE_LITTERAL , { TOKEN_NUMBER } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_literal(Type::Native::int8, *p[0].string_value);  }
	},
	// ex: 0Xff
	{ RULE_LITTERAL , { TOKEN_HEXANUMBER } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_literal(Type::Native::int8, *p[0].string_value);  }
	},
	// true
	{ RULE_LITTERAL , { TOKEN_TRUE } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_literal(Type::Native::bit, *p[0].string_value);  }
	},
	// false
	{ RULE_LITTERAL , { TOKEN_FALSE } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.expression_value = new_literal(Type::Native::bit, *p[0].string_value);  }
	},

	// array type. ex : int8[10]
	{ RULE_TYPE , { RULE_TYPE, '[', RULE_EXPRESSION  , ']'} ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.type_value = new_type_array(p[0].type_value, p[2].expression_value);  }
	},
	// types: bool, int8, int16, int32, int64, uint8, uint16, uint32, uint64, ..
	{ RULE_TYPE , { TOKEN_TYPE_BOOL } ,	  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::bit);    }},
	{ RULE_TYPE , { TOKEN_TYPE_INT8 } ,	  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::int8);   }},
	{ RULE_TYPE , { TOKEN_TYPE_UINT8 },	  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::uint8);  }},
	{ RULE_TYPE , { TOKEN_TYPE_INT32 },	  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::int32);  }},
	{ RULE_TYPE , { TOKEN_TYPE_UINT32 },  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::uint32); }},
	{ RULE_TYPE , { TOKEN_TYPE_INT64 },	  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::int64);  }},
	{ RULE_TYPE , { TOKEN_TYPE_UINT64 },  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::uint64); }},
	{ RULE_TYPE , { TOKEN_TYPE_INT256 },  [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::int256); }},
	{ RULE_TYPE , { TOKEN_TYPE_UINT256 }, [this](TokenValue& result, std::vector<TokenValue>) { result.type_value = new_type_basic(Type::Native::uint256);}},
	// use type
	{ RULE_TYPE , { TOKEN_STRUCT_TYPE } ,
		[this](TokenValue& result, std::vector<TokenValue> p) { result.type_value = new_type_user_defined(*p[0].string_value);  }
	},

};
	int nb_grammar_rules = sizeof(rules_definition) / sizeof(rules_definition[0]);
	return std::vector<RuleDefinition>(rules_definition, rules_definition + nb_grammar_rules);
};

