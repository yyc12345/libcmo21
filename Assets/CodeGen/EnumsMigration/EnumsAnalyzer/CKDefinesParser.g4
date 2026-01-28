parser grammar CKDefinesParser;
options { tokenVocab = CKGenericLexer; }

prog: definePair+ ;

definePair: CKGENERIC_DEFINE CKGENERIC_ID (CKGENERIC_NUM | CKGENERIC_ID) ;
