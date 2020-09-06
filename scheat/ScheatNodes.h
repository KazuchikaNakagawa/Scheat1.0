//
//  ScheatNodes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef ScheatNodes_h
#define ScheatNodes_h

namespace scheat {
#define unique(id) std::unique_ptr<id>
#include <string>
#include "ScheatContext.h"
namespace node{

// NodeData
// have a parsed data.
// basically, value is register
class NodeData {
    
public:
    std::string value;
    TypeData size;
    NodeData(std::string a, std::string b) : value(a), size(b) {};
};

class Node {
    
public:
    SourceLocation location;
    
    virtual node::NodeData* codegen(IRStream &) { return nullptr; };
    
    virtual ~Node() {};
};

class TermNode : public Node {
    
    
public:
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    virtual ~TermNode(){};
};

class PrimaryExprNode : public Node {
    
public:
    node::NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~PrimaryExprNode(){};
    PrimaryExprNode() {};
};

class ExprNode : public Node {
    
public:
    node::NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~ExprNode(){};
    ExprNode() {};
};

class IdentifierTerm : public TermNode {
    std::string value;
    
public:
    __deprecated_msg("this class is only available for unique_ptr")
    IdentifierTerm(std::string v) : value(v) {};
    static unique(IdentifierTerm) create(std::string, bool);
};

class IdentifierExpr : public ExprNode {
    unique(IdentifierExpr) expr;
    // operator or . token
    scheat::Token * opTok;
    unique(IdentifierTerm) term;
public:
    
};

class IntTerm : public TermNode {
    
public:
    
};

class Term : public Node {
    unique(Term) terms;
    scheat::Token *opTok;
    unique(TermNode) node;
public:
    node::NodeData * codegen(IRStream &) override;
    static unique(Term) create(unique(TermNode));
    static unique(Term) create(unique(Term), scheat::Token *, unique(TermNode));
};

class Expr : public Node {
    
public:
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
};

class PrototypeExpr : public ExprNode {
    scheat::Token *identifier;
    TypeData *type;
public:
    __deprecated PrototypeExpr(scheat::Token *t, TypeData *ty) : identifier(t), type(ty), ExprNode() {};
    
};

class StatementNode : public Node {
    
public:
    virtual void dump(IRStream &) {};
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    virtual ~StatementNode() {};
    StatementNode() {};
};

class Statements : public Node {
public:
    unique(Statements) stmts;
    unique(StatementNode) stmt;
    node::NodeData * codegen(IRStream &) override;
    static unique(Statements) make(unique(StatementNode), unique(Statements));
};

class PrimaryExpr : public ExprNode {
    unique(PrimaryExpr) exprs;
    scheat::Token *opTok;
    unique(Term) term;
public:
    __deprecated PrimaryExpr() {};
    
    node::NodeData * codegen(IRStream &) override;
    static unique(PrimaryExpr) make(unique(Term));
    static unique(PrimaryExpr) make(unique(PrimaryExpr), scheat::Token *, unique(Term));
};

class PrintStatement : public StatementNode {
    unique(Expr) ex;
public:
    ~PrintStatement(){};
    __deprecated PrintStatement() {};
    node::NodeData * codegen(IRStream &) override;
    void dump(IRStream &) override;
    static unique(PrintStatement) make(unique(Expr));
};


} // end of node namespace

}
#undef unique
#endif /* ScheatNodes_h */
