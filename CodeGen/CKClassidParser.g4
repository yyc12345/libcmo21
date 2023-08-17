parser grammar CKClassidParser;
options { tokenVocab = CKGeneralLexer; }

prog: definePair+ ;

definePair: CKGENERAL_DEFINE CKGENERAL_ID CKGENERAL_NUM ;
