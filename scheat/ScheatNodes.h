//
//  ScheatNodes.h
//  ProjectScheat
//
//  Created by かずちか on 2020/08/26.
//

#ifndef ScheatNodes_h
#define ScheatNodes_h


#define p_unique(id) std::unique_ptr<id>
#include <string>
#include "ScheatContext.h"
#include "ScheatObjects.h"
namespace scheat {
namespace node{

// NodeData
// have a parsed data.
// basically, value is register
class NodeData {
    
public:
    std::string value;
    TypeData size;
    NodeData(std::string a, std::string b) : value(a), size(b) {};
    NodeData(std::string a, TypeData t) : value(a), size(t) {};
};

// base of node.
// location may be undefined by accident
class Node {
    
public:
    SourceLocation location;
    
    TypeData node_size;
    
    virtual node::NodeData* codegen(IRStream &) { return nullptr; };
    
    virtual std::string userdump() { return "UNDEFINED"; };
    
    virtual ~Node() {};
};

// base of term
class TermNode : public Node {
    
    
public:
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    virtual ~TermNode(){};
    std::string userdump() override{ return "UNDEFINED"; };
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
    std::string userdump() override { return "undefined"; };
};

class IdentifierTerm : public TermNode {
    std::string value;
    
public:
    __deprecated_msg("this class is only available for unique_ptr")
    IdentifierTerm(std::string v) : value(v) {};
    static p_unique(IdentifierTerm) create(std::string, bool);
    node::NodeData * codegen(IRStream &) override;
    std::string userdump() override{
        return value;
    }
};

class IdentifierExpr : public ExprNode {
    p_unique(IdentifierExpr) expr;
    // operator or . token
    Token * opTok;
    p_unique(IdentifierTerm) term;
public:
    node::NodeData * codegen(IRStream &) override;
    std::string userdump() override{
        std::string buf = "";
        if (expr != nullptr) {
            buf = expr->userdump();
        }
        if (opTok != nullptr) {
            buf = buf + opTok->value.strValue;
        }
        buf = buf + term->userdump();
        return buf;
    };
};

class IntTerm : public TermNode {
    Token *valToken;
    p_unique(IdentifierExpr) ident;
public:
    IntTerm() : TermNode() {};
    static p_unique(IntTerm) make(Token *);
    static p_unique(IntTerm) make(p_unique(IdentifierExpr));
    node::NodeData * codegen(IRStream &) override;
};

class Expr;

class Term : public Node {
    p_unique(Term) terms;
    Token *opTok;
    p_unique(TermNode) node;
    p_unique(Expr) exprNode = nullptr;
public:
    node::NodeData * codegen(IRStream &) override;
    static p_unique(Term) create(p_unique(TermNode));
    static p_unique(Term) create(p_unique(Term), Token *, p_unique(TermNode));
    static p_unique(Term) create(p_unique(Expr));
};

class PrimaryExpr : public ExprNode {
    p_unique(PrimaryExpr) exprs;
    Token *opTok;
    p_unique(Term) term;
public:
    __deprecated PrimaryExpr() {};
    
    node::NodeData * codegen(IRStream &) override;
    static p_unique(PrimaryExpr) make(p_unique(Term));
    static p_unique(PrimaryExpr) make(p_unique(PrimaryExpr), Token *, p_unique(Term));
};

class Expr : public Node {
    p_unique(PrimaryExpr) body;
    p_unique(Expr) exprs;
    Token *op;
public:
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    static p_unique(Expr) make(p_unique(PrimaryExpr) lhs, Token *opTok = nullptr,
                               p_unique(Expr) rhs = nullptr);
};


class PrototypeExpr : public ExprNode {
    Token *identifier;
    TypeData *type;
public:
    __deprecated PrototypeExpr(Token *t, TypeData *ty) : identifier(t), type(ty), ExprNode() {};
    
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
    p_unique(Statements) stmts;
    p_unique(StatementNode) stmt;
    node::NodeData * codegen(IRStream &) override;
    static p_unique(Statements) make(p_unique(StatementNode), p_unique(Statements) = nullptr);
};



class PrintStatement : public StatementNode {
    p_unique(Expr) ex;
public:
    ~PrintStatement(){};
    __deprecated PrintStatement() {};
    node::NodeData * codegen(IRStream &) override;
    void dump(IRStream &) override;
    static p_unique(PrintStatement) make(p_unique(Expr));
};


} // end of node namespace

}
#undef unique
#endif /* ScheatNodes_h */
