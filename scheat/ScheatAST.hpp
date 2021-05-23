//
//  ScheatAST.hpp
//  ProjectScheat
//
//  Created by かずちか on 2021/05/21.
//

#ifndef ScheatAST_h
#define ScheatAST_h

#include <stdio.h>
#include <string>
#include "ScheatTypes.h"
#include "ScheatToken.h"
#include "ScheatContextStructures.h"
#include "ScheatContext.h"
#include "ScheatStd.hpp"
using namespace std;
/*
 program : statements
         | program statements
 
 statements : statements , statement
            | statements .
 statement : do { statement }
           | expr is expr
           | expr
           | if expr, statements
           | while expr, statements
           | for expr times, statements
           | string
           | done
 expr : primary
      | expr op expr
      | op expr
      | expr op
      | the expr
      | at expr
 
 primary : term
         | primary op primary
         | op primary
         | primary op
 term : int
      | str
      | bool
      | float
      | term op term
      | term op
      | op term
      | ( expr )
      | idexpr
 idexpr : identifier
        | identifier arg
        | idexpr identifier
        | idexpr identifier arg
        | this options identifier
 arg : ( expr, expr, ... )
 */

namespace scheat {

namespace nodes2 {

class Node;

class Program;

class Statements;

class Statement;

class DoStatement;

class AssignStatement;

class ExprStatement;

class IfStatement;

class WhileStatement;

class ForStatement;

class LogStringStatement;

class DoneStatement;

class Expr;

class PrimaryExpr;

class InfixOperatorExpr;

class PrefixOperatorExpr;

class PostfixOperatorExpr;

class TheExpr;

class LoadExpr;

class InfixOperatorPrimaryExpr;

class PrefixOperatorPrimaryExpr;

class PostfixOperatorPrimaryExpr;

class Term;

class IntTerm;

class StringTerm;

class BoolTerm;

class FloatTerm;

class InfixOperatorTerm;

class PostfixOperatorTerm;

class PrefixOperatorTerm;

class ParenthesesExpr;

class IdentifierExpr;

class FunctionCallTerm;

class FunctionAttributeExpr;

class VariableAttributeExpr;

class ArgumentExpr;

class NewIdentifierExpr;

}

namespace parser2 {
extern void parse(_Scheat *, Token *);
extern unique_ptr<Program> parseProgram(Token *&);
extern unique_ptr<Statements> parseStatements(Token *&);
extern unique_ptr<Statement> parseStatement(Token *&);
extern unique_ptr<DoStatement> parseDoStatement(Token *&);
extern unique_ptr<AssignStatement> parseAssignStatement(Token *&);
extern unique_ptr<ExprStatement> parseExprStatement(Token *&);
extern unique_ptr<IfStatement> parseIfStatement(Token *&);
extern unique_ptr<WhileStatement> parseWhileStatement(Token *&);
extern unique_ptr<ForStatement> parseForStatement(Token *&);
extern unique_ptr<LogStringStatement> parseLogStringStatement(Token *&);
extern unique_ptr<DoneStatement> parseDoneStatement(Token *&);
extern unique_ptr<Expr> parseExpr(Token *&);
extern unique_ptr<PrimaryExpr> parsePrimaryExpr(Token *&);
extern unique_ptr<Expr> parseOperatedExpr(Token *&);
extern unique_ptr<TheExpr> parseTheExpr(Token *&);
extern unique_ptr<LoadExpr> parseLoadExpr(Token *&);
extern unique_ptr<PrimaryExpr> parseOperatedPrimaryExpr(Token *&);
extern unique_ptr<Term> parseTerm(Token *&);
extern unique_ptr<IdentifierExpr> parseIdentifierExpr(Token *&);
extern unique_ptr<FunctionCallTerm> parseFunctionCallTerm(Token *&);
extern unique_ptr<FunctionCallTerm> parseVariableTerm(Token *&);
extern unique_ptr<VariableAttributeExpr> parseVariableAttributeExpr(Token *&);
extern unique_ptr<FunctionAttributeExpr> parseFunctionAttributeExpr(Token *&);
extern unique_ptr<ArgumentExpr> parseArgumentExpr(Token *&);
extern unique_ptr<NewIdentifierExpr> parseNewIdentifierExpr(Token *&);
}

}

#endif /* ScheatAST_h */
