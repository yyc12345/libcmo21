parser grammar CKEnumsParser;
options { tokenVocab = CKGenericLexer; }

prog: enumBody* ;

enumBody: CKGENERIC_TYPEDEF? CKGENERIC_ENUM CKGENERIC_ID CKGENERIC_LBRACKET 
entryPair+
CKGENERIC_RBRACKET CKGENERIC_ID? CKGENERIC_SEMICOLON ;

entryPair: CKGENERIC_ID (CKGENERIC_EQUAL entryValue)? CKGENERIC_COMMA? ;

entryValue: CKGENERIC_NUM (CKGENERIC_LSHIFT CKGENERIC_NUM)?     # entryDirectValue
| CKGENERIC_ID (CKGENERIC_OR CKGENERIC_ID)*                         # entryRelativeValue
;