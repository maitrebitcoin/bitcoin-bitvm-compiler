#pragma once

// token elements (lexer)
enum E_TokenId {
	TOKEN_TYPE_BOOL			  = 257,
	TOKEN_TYPE_INT8,		   //  signed 8 bits integer
	TOKEN_TYPE_UINT8,		   // 
	TOKEN_TYPE_INT32,		   //  signed 32 bits integer
	TOKEN_TYPE_UINT32,		   // 
	TOKEN_TYPE_INT64,		   //  signed 64 bits integer
	TOKEN_TYPE_UINT64,		   // 
	TOKEN_TYPE_INT256,		   //  signed 256 bits integer / hash
	TOKEN_TYPE_UINT256,		   // 
	TOKEN_IDENTIFIER_FNNAME,   //  - function name
	TOKEN_IDENTIFIER_FNPARAM,  //  - in function parmetres only
	TOKEN_IDENTIFIER,	       //  - in function body statement only,
	TOKEN_IDENTIFIER_DECL	 , //  - in function body statement only, after a type: local var déclaration
	TOKEN_IDENTIFIER_SETVAR,   //  - in function body statement only, var name for affectation only.
	TOKEN_NUMBER,			   // 
	TOKEN_RETURN,			   // 
	TOKEN_TRUE,				   //  true
	TOKEN_FALSE,			   //  false
	TOKEN_LEFT_SHIFT,		   //  <<
	TOKEN_RIGHT_SHIFT,		   //  >>
	TOKEN_TEST_EQUAL,		   //  ==
	TOKEN_TEST_NOTEQUAL,	   //  !=
	TOKEN_TEST_LOWER,		   //  <
	TOKEN_TEST_LOWEROREQ,	   //  <=
	TOKEN_TEST_GREATER,		   //  >
	TOKEN_TEST_GREATEROREQ,	   //  >=
	TOKEN_HEXANUMBER,		   //  0xF2
	TOKEN_KEYWORKD_STRUCT,	   //  keyword "struct"
	TOKEN_DECLARE_STRUCT_NAME, //  after "struct" keyword
	TOKEN_STRUCT_TYPE,		   //  qs as type befor a var. ex: "struct b;"
	TOKEN_USE_STRUCT,		   //  before . as in "struct.member"
	TOKEN_USE_STRUCT_MEMBER,   //  after . as in "struct.member"
	TOKEN_KEYWORKD_IF,			//  keyword "if"


};
#define is_token_type(t) (t>=TOKEN_TYPE_BOOL && t<=TOKEN_TYPE_UINT256)

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