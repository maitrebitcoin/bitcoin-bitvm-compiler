#pragma once

// token elements (lexer)
enum E_TokenId {
	TOKEN_TYPE_BOOL			  = 257,
	TOKEN_TYPE_BYTE,		   // 258
	TOKEN_IDENTIFIER_FNNAME,   // 259 - function name
	TOKEN_IDENTIFIER_FNPARAM,  // 260 - in function parmetres only
	TOKEN_IDENTIFIER,	       // 261 - in function body statement only,
	TOKEN_IDENTIFIER_LOCALVAR, // 262 - in function body statement only, after a type: local var déclaration
	TOKEN_IDENTIFIER_SETVAR,   // 263 - in function body statement only, var name for affectation only.
	TOKEN_NUMBER,			   // 264
	TOKEN_RETURN,			   // 265
	TOKEN_TRUE,				   // 266 true
	TOKEN_FALSE,			   // 267 false
	TOKEN_LEFT_SHIFT,		   // 268 <<
	TOKEN_RIGHT_SHIFT,		   // 269 >>
	TOKEN_TEST_EQUAL,		   // 270 ==
	TOKEN_TEST_NOTEQUAL,	   // 270 !=
	TOKEN_TEST_LOWER,		   // 270 <
	TOKEN_TEST_LOWEROREQ,	   // 270 <=
	TOKEN_TEST_GREATER,		   // 270 >
	TOKEN_TEST_GREATEROREQ,	   // 270 >=
};
// grammar elements
enum E_RuleId {
	RULE_FUNCTION				= 1000,
	RULE_FUNCTION_DEFINTION	,	// 1001 ex : bool f(byte a, byte b) 
	RULE_TYPE,					// 1002 ex : bool
	RULE_N_PARAMETERS_DECL,		// 1003 ex : (bool a, byte b)		
	RULE_1_PARAMETER_DECL,		// 1004 ex : bool b
	RULE_CODEBLOC,				// 1005 ex : { return a+b; }
	RULE_N_STATEMENTS,			// 1006 ex : a++;b++;return a+b;	
	RULE_1_STATEMENT,			// 1007 ex : a++
	RULE_STATEMENT,				// 1008 ex : a++
	RULE_OPERATION,				// 1009 ex : a&b
	RULE_INSTRUCITON_RETURN,	// 1010 ex : return a+b
	RULE_EXPRESSION,			// 1011 ex : 123 our a or a+b
	RULE_LITTERAL,				// 1012 ex : 123
	RULE_VARIABLE,				// 1013 ex : a
	RULE_OPERATOR_AND,			// 1014 ex : a&b
	RULE_OPERATOR_OR,			// 1015 ex : a|b
	RULE_OPERATOR_XOR,			// 1016 ex : a^b
	RULE_OPERATOR_NOT,			// 1017 ex : !a
	RULE_OPERATOR_ADD,			// 1018 ex : a+b
	RULE_OPERATOR_SUB,			// 1019 ex : a-b
	RULE_OPERATOR_COMPLEMENT,   // 1020 ex : ~a 
	RULE_OPERATOR_SHIFT,		// 1021 ex : << >>
	RULE_OPERATOR_CMP,			// 1022 ex : <= > ===
	RULE_PROGRAM = 1999, //  the whole program
};