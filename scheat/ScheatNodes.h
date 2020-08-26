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
namespace node{

class NodeData {
    
public:
    std::string value;
    std::string size;
    NodeData(std::string a, std::string b) : value(a), size(b) {};
};

class Node {
    
public:
    virtual node::NodeData* codegen(IRStream &) { return nullptr; };
    
    virtual ~Node() {};
};

class TermNode : public Node {
    
    
public:
    node::NodeData * codegen(IRStream &) override{ return nullptr; };
    virtual ~TermNode();
};

class PrimaryExprNode : public Node {
    
public:
    node::NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~PrimaryExprNode();
};

class ExprNode : public Node {
    
public:
    node::NodeData * codegen(IRStream &) override { return nullptr; };
    virtual ~ExprNode();
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
    scheat::Token *id;
    TypeData *type;
public:
    __deprecated PrototypeExpr(scheat::Token *t, TypeData *ty) : id(t), type(ty), ExprNode() {};
    
};

class Statement : public Node {
    
public:
    virtual void dump(IRStream &) {};
};

class PrimaryExpr : public ExprNode {
    unique(PrimaryExpr) exprs;
    scheat::Token *opTok;
    unique(Term) term;
public:
    __deprecated PrimaryExpr();
    node::NodeData * codegen(IRStream &) override;
    static unique(PrimaryExpr) make(unique(Term));
    static unique(PrimaryExpr) make(unique(PrimaryExpr), scheat::Token *, unique(Term));
};


} // end of node namespace

}
#undef unique
#endif /* ScheatNodes_h */
