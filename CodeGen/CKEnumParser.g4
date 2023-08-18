parser grammar CKEnumParser;
options { tokenVocab = CKGeneralLexer; }

prog: enumBody* ;

enumBody: CKGENERAL_TYPEDEF? CKGENERAL_ENUM CKGENERAL_ID CKGENERAL_LBRACKET 
entryPair+
CKGENERAL_RBRACKET CKGENERAL_ID? CKGENERAL_SEMICOLON ;

entryPair: CKGENERAL_ID (CKGENERAL_EQUAL entryValue)? CKGENERAL_COMMA? ;

entryValue: CKGENERAL_NUM (CKGENERAL_LSHIFT CKGENERAL_NUM)?     # entryDirectValue
| CKGENERAL_ID (CKGENERAL_OR CKGENERAL_ID)*                         # entryRelativeValue
;