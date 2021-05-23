//
//  Lexer.cpp
//  scheat
//
//  Created by かずちか on 2020/07/31.
//

#include "ScheatToken.h"
#include "Lexer.hpp"
#include "ScheatObjects.h"
#include "ScheatStatics.h"

using namespace scheat;
using namespace scheat::lexer;

std::string Token::encodeOperator(){
    if (kind != TokenKind::val_operator) {
        scheato->Warning(SourceLocation(), __FILE_NAME__, __LINE__, "token was tried to encoded though it was not an operator");
        return "__NON_OPERATOR_ENCODED__";
    }
    std::string result = "";
    for (auto iter = value.strValue.begin(); iter != value.strValue.end(); iter = std::next(iter)) {
        char c = *iter;
        if (c == '%') {
            result.append("per_");
        }
        if (c == '+') {
            result.append("plus_");
        }
        if (c == '-') {
            result.append("minus_");
        }
        if (c == '/') {
            result.append("slash_");
        }
        if (c == '*') {
            result.append("astrsk_");
        }
        if (c == '&') {
            result.append("and_");
        }
        if (c == '!') {
            result.append("not_");
        }
        if (c == '^') {
            result.append("hat_");
        }
        if (c == '$') {
            result.append("doll_");
        }
        if (c == '=') {
            result.append("eq_");
        }
        if (c == '@') {
            result.append("at_");
        }
        if (c == '<') {
            result.append("less_");
        }
        if (c == '>') {
            result.append("more_");
        }
        if (c == '|') {
            result.append("bar_");
        }
        if (c == ':') {
            result.append("coron_");
        }
    }
    return result;
}

Token *Token::last(){
    Token *cpy = this;
    if (cpy == nullptr) {
        return nullptr;
    }
    while (cpy->next != nullptr) {
        cpy = cpy->next;
    }
    return cpy;
}

Token *Token::first(){
    Token *cpy = this;
    if (cpy == nullptr) {
        return nullptr;
    }
    
    while (cpy->prev != nullptr) {
        cpy = cpy->prev;
    }
    
    return cpy;
}

Lexer::Lexer(scheat::_Scheat *host) : location(host->location){
    buf = "";
    skipFlag = false;
    this->host = host;
    tokens = nullptr;
    commentDepth = 0;
    state = initState;
    if (host->tokens != nullptr) tokens = host->tokens->last()->prev;
    else tokens = nullptr;
    location = host->location;
}

void Lexer::lex(std::ifstream &stream){
    if (!stream.is_open()) {
        host->FatalError(SourceLocation(), __FILE_NAME__, __LINE__, "%s is not open",
                         host->targettingFile.c_str());
    }
    int c;
    while (c = stream.get(), c != EOF) {
        input(c, stream.get());
        stream.unget();
        if (host->hasProbrem()) {
            break;
        }
    }
    genTok();
    addEOFToken();
    host->location = location;
}

void Lexer::addEOFToken(){
    auto tok = new Token();
    tok->kind = TokenKind::tok_EOF;
    tok->location = location;
    tokens = Token::add(tokens, tok);
}

void Token::valInt(std::string k){
    value.intValue = atoi(k.c_str());
    kind = TokenKind::val_num;
}

void Token::valStr(std::string k){
    if (k[0] != '"') {
        printf("Scheat System Error %u\n", __LINE__);
        printf("    reason: string started \" else character.\n");
        exit(0);
    }
    // not sure if it doesn't make crash
    if (k[k.length() - 1] != '"') {
        printf("Scheat System Error %u\n", __LINE__);
        printf("    reason: string ended \" else character.\n");
        exit(0);
    }
    value.strValue = k;
    kind = TokenKind::val_str;
}

void Token::valBool(std::string k){
    if (k == "true") {
        value.boolValue = true;
    }else if (k == "false"){
        value.boolValue = false;
    }else if (k == "Yes"){
        value.boolValue = true;
    }else if (k == "No"){
        value.boolValue = false;
    }else{
        printf("Scheat System Error %u\n", __LINE__);
        printf("    reason: illegal bool token error\n");
        exit(0);
    }
    kind = TokenKind::val_bool;
}

void Token::valDouble(std::string k){
    value.doubleValue = atof(k.c_str());
    kind = TokenKind::val_double;
}

Token *Token::add(Token *tokens, Token *token){
    if (tokens == nullptr) {
        return token;
    }
    tokens->next = token;
    token->prev = tokens;
    tokens = tokens->next;
    return tokens;
}

#define tadd tokens = Token::add(tokens, tok)

void Lexer::genTok(){
    scheato->DevLog(SourceLocation(), __FILE_NAME__, __LINE__, "token generated with %s\n", buf.c_str());
    if (buf.empty()) {
        state = initState;
        clear();
        return;
    }
    Token *tok = new Token();
    tok->location = location;
    if (buf == "...") {
        tok->kind = TokenKind::val_operator;
        tok->value.strValue = buf;
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == ".") {
        if (AccessTokFlag) {
            tok->kind = TokenKind::tok_access;
            AccessTokFlag = false;
        }else{
            tok->kind = TokenKind::tok_period;
        }
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == ",") {
        tok->kind = TokenKind::tok_comma;
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == "(") {
        tok->kind = TokenKind::tok_paren_l;
        tadd;
        clear();
        return;
    }
    if (buf == ")") {
        tok->kind = TokenKind::tok_paren_r;
        tadd;
        clear();
        return;
    }
    if (buf == "{") {
        tok->kind = TokenKind::tok_brace_l;
        tadd;
        clear();
        return;
    }
    if (buf == "}") {
        tok->kind = TokenKind::tok_brace_r;
        tadd;
        clear();
        return;
    }
    if (state == numberState) {
        tok->valInt(buf);
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (state == stringState) {
        buf.pop_back();
        buf.append("\\0\"");
        tok->valStr(buf);
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (state == doubleState) {
        tok->valDouble(buf);
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (state == operatorState) {
        tok->value.strValue = buf;
        tok->kind = TokenKind::val_operator;
        tokens = Token::add(tokens, tok);
        clear();
        return;
    }
    if (buf == "this" || buf == "new" || buf == "these") {
        tok->kind = TokenKind::tok_this;
        tadd;
        clear();
        return;
    }
    
    if (buf == "break") {
        tok->kind = TokenKind::tok_break;
        tadd;
        clear();
        return;
    }
    
    if (buf == "loaded" || buf == "at") {
        tok->kind = TokenKind::tok_loaded;
        tadd;
        clear();
        return;
    }
    
    if (buf == "is" || buf == "are" || buf == "shows" || buf == "show" || buf == "become" || buf == "becomes") {
        tok->kind = TokenKind::tok_is;
        tadd;
        clear();
        return;
    }
    
    if (buf == "that" || buf == "the" || buf == "those") {
        tok->kind = TokenKind::tok_the;
        tadd;
        clear();
        return;
    }
    
    if (buf == "or") {
        tok->kind = TokenKind::tok_or;
        tadd;
        clear();
        return;
    }
    
    if (buf == "do" || buf == "does") {
        tok->kind = TokenKind::tok_do;
        tadd;
        clear();
        return;
    }
    
    if (buf == "from") {
        tok->kind = TokenKind::tok_from;
        tadd;
        clear();
        return;
    }
    
    if (buf == "remember") {
        tok->kind = TokenKind::tok_external;
        tadd;
        clear();
        return;
    }
    
    if (buf == "return") {
        tok->kind = TokenKind::tok_return;
        tadd;
        clear();
        return;
    }
    
    if (buf == "global") {
        tok->kind = TokenKind::tok_global;
        tadd;
        clear();
        return;
    }
    
    if (buf == "local") {
        tok->kind = TokenKind::tok_local;
        tadd;
        clear();
        return;
    }
    
    if (buf == "class") {
        tok->kind = TokenKind::tok_class;
        tadd;
        clear();
        return;
    }
    
    if (buf == "its") {
        tok->kind = TokenKind::tok_its;
        tadd;
        clear();
        return;
    }
    
    if (buf == "print") {
        tok->kind = TokenKind::embbed_func_print;
        tadd;
        clear();
        return;
    }
    
    if (buf == "assemble") {
        tok->kind = TokenKind::embbed_func_assemble;
        tadd;
        clear();
        return;
    }
    
    if (buf == "if") {
        tok->kind = TokenKind::tok_if;
        tadd;
        clear();
        return;
    }
    
    if (buf == "import") {
        tok->kind = TokenKind::tok_import;
        tadd;
        clear();
        return;
    }
    
    if (buf == "export") {
        tok->kind = TokenKind::tok_export;
        tadd;
        clear();
        return;
    }
    
    if (buf == "to") {
        tok->kind = TokenKind::tok_to;
        tadd;
        clear();
        return;
    }
    
    if (buf == "of") {
        tok->kind = TokenKind::tok_of;
        tadd;
        clear();
        return;
    }
    
    if (buf == "done") {
        tok->kind = TokenKind::tok_done;
        tadd;
        clear();
        return;
    }
    
    if (buf == "null") {
        tok->kind = TokenKind::val_null;
        tadd;
        clear();
        return;
    }
    
    if (buf == "for") {
        tok->kind = TokenKind::tok_for;
        tadd;
        clear();
        return;
    }
    
    if (buf == "times") {
        tok->kind = TokenKind::tok_times;
        tadd;
        clear();
        return;
    }
    
    if (buf == "with") {
        tok->kind = TokenKind::tok_with;
        tadd;
        clear();
        return;
    }
    
    if (buf == "while") {
        tok->kind = TokenKind::tok_while;
        tadd;
        clear();
        return;
    }
    
    if (buf == "true") {
        tok->kind = TokenKind::val_bool;
        tok->value.boolValue = true;
        tadd;
        clear();
        return;
    }
    
    if (buf == "false") {
        tok->kind = TokenKind::val_bool;
        tok->value.boolValue = false;
        tadd;
        clear();
        return;
    }
    
    if (state == identifierState) {
        tok->value.strValue = buf;
        tok->kind = TokenKind::val_identifier;
        tadd;
    }
    clear();
}

#undef tadd

void Token::out(){
    printf("line: %d, column: %d    ", location.line, location.column);
    
    switch (kind) {
        case TokenKind::tok_EOF:
            printf("EOF token\n");
            break;
        case TokenKind::val_identifier:
            printf("identifier token %s\n", value.strValue.c_str());
            break;
        case TokenKind::val_num:
            printf("integer token %d\n", value.intValue);
            break;
        case TokenKind::val_str:
            printf("string token ->%s\n", value.strValue.c_str());
            break;
        case TokenKind::val_double:
            printf("float-point token ->%lf\n", value.doubleValue);
            break;
        case TokenKind::val_bool:
            printf(value.boolValue ? ("boolean token -> true\n"):("boolean token ->false\n"));
            break;
        case TokenKind::val_operator:
            printf("operator token ->%s ", value.strValue.c_str());
            printf("encoded ->%s\n", this->encodeOperator().c_str());
            break;
        case TokenKind::val_null:
            printf("null token\n");
            break;
        case TokenKind::tok_this:
            printf("this token\n");
            break;
        case TokenKind::tok_the:
            printf("the token\n");
            break;
        case TokenKind::tok_is:
            printf("is token\n");
            break;
        case TokenKind::tok_of:
            printf("of token\n");
            break;
        case TokenKind::tok_period:
            printf(". token\n");
            break;
        case TokenKind::tok_comma:
            printf(", token\n");
            break;
        case TokenKind::tok_range:
            printf("... token\n");
            break;
        case TokenKind::tok_if:
            printf("if token\n");
            break;
        case TokenKind::tok_for:
            printf("for token\n");
            break;
            
        case TokenKind::tok_from:
            printf("from token\n");
            break;
        case TokenKind::tok_times:
            printf("times token\n");
            break;
        case TokenKind::tok_do:
            printf("do token\n");
            break;
        case TokenKind::tok_done:
            printf("done token\n");
            break;
        case TokenKind::tok_or:
            printf("or token\n");
            break;
        case TokenKind::tok_paren_l:
            printf("( token\n");
            break;
        case TokenKind::tok_paren_r:
            printf(") token\n");
            break;
        case TokenKind::tok_brace_l:
            printf("{ token\n");
            break;
        case TokenKind::tok_brace_r:
            printf("} token\n");
            break;
        case TokenKind::tok_access:
            printf("access token(obsoluted.)\n");
            break;
        case TokenKind::tok_external:
            printf("external token\n");
            break;
        case TokenKind::tok_import:
            printf("import token\n");
            break;
        case TokenKind::tok_export:
            printf("export token\n");
            break;
        case TokenKind::tok_to:
            printf("to token\n");
            break;
        case TokenKind::tok_with:
            printf("with token\n");
            break;
        case TokenKind::tok_while:
            printf("while token\n");
            break;
        case TokenKind::tok_loaded:
            printf("loaded token\n");
            break;
        case TokenKind::tok_return:
            printf("return token\n");
            break;
        case TokenKind::tok_break:
            printf("break token\n");
            break;
        case TokenKind::tok_class:
            printf("class token\n");
            break;
        case TokenKind::tok_its:
            printf("its token\n");
            break;
        case TokenKind::tok_local:
            printf("local token\n");
            break;
        case TokenKind::tok_global:
            printf("global token\n");
            break;
        case TokenKind::embbed_func_print:
            printf("print token\n");
            break;
        case TokenKind::embbed_func_import:
            printf("import token\n");
            break;
        case TokenKind::embbed_func_free:
            printf("free token\n");
            break;
        case TokenKind::embbed_func_assemble:
            printf("assemble token\n");
            break;
    }
    
    //printf("unknown token\n");
    
}

void Lexer::clear(){
    state = initState;
    buf = "";
    isPossibleForPPPTok = false;
    commentDepth = 0;
}

void Lexer::input(int c, int next){
    if (host->hasProbrem()) {
        return;
    }
    host->DevLog(location, __FILE_NAME__,__LINE__, "%c was input, %s : now buffer", c, buf.c_str());
    if (c == '\0' || c == EOF) {
        genTok();
        return;
    }
    
    location.column++;
    
    if (c == '\n') {
//        location.line++;
//        location.column = 0;
    }
    
    if (skipFlag) {
        skipFlag = !skipFlag;
        return;
    }
    
    if (commentDepth > 0 && c == '*' && next == '#' && state != stringState) {
        commentDepth--;
        skipFlag = true;
        if (commentDepth == 0) {
            clear();
        }
        return;
    }
    
    if (commentDepth > 0) {
        return;
    }
    
    if (true
        && state == commentState
        && c != '\n') {
        location.line++;
        location.column = 0;
        return;
        
    }else if (state == commentState && c == '\n'){
        clear();
        location.line++;
        location.column = 0;
        return;
    }else if (c == '\n'){
        genTok();
        location.line++;
        location.column = 0;
        return;
    }
    
    if (c == '"' && state == stringState) {
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (c == '"') {
        genTok();
        buf.push_back(c);
        state = stringState;
        return;
    }
    
    if (state == stringState && c == '\\') {
        if (next == 'n') {
            buf.push_back('\\');
            buf.push_back('n');
            skipFlag = true;
            return;
        }
        if (next == 'b') {
            buf.push_back('\b');
            skipFlag = true;
            return;
        }
        if (next == '0') {
            buf.push_back('\0');
            skipFlag = true;
            return;
        }
        if (next == '\"') {
            buf.push_back('\"');
            skipFlag = true;
            return;
        }
        if (next == '\\') {
            buf.push_back('\\');
            skipFlag = true;
            return;
        }
        buf.push_back(next);
        skipFlag = true;
        return;
    }
    
    if (state == stringState) {
        buf.push_back(c);
        return;
    }
    
    if (c == ' ') {
        genTok();
        return;
    }
    
    if (c == '\t') {
        genTok();
        return;
    }
    
    if (c == '#' && next == '*') {
        genTok();
        state = longCommentState;
        commentDepth++;
    }
    
    if (c == '#') {
        state = commentState;
        return;
    }
    
    if (isalpha(c) || c == '_') {
        if (state == operatorState) {
            genTok();
            buf.push_back(c);
            state = identifierState;
            return;
        }
        if (state == initState) {
            state = identifierState;
            buf.push_back(c);
            return;
        }
        if (state == identifierState) {
            buf.push_back(c);
            return;
        }
        host->FatalError(location, __FILE_NAME__,__LINE__, "illegal character %c was input.", c);
        return;
    }
    
    if (isdigit(c)) {
        if (state == operatorState) {
            genTok();
            buf.push_back(c);
            state = numberState;
            return;
        }
        if (state == numberState) {
            buf.push_back(c);
            return;
        }
        if (state == identifierState) {
            buf.push_back(c);
            return;
        }
        if (state == initState) {
            buf.push_back(c);
            state = numberState;
            return;
        }
        if (state == doubleState) {
            buf.push_back(c);
            state = doubleState;
            return;
        }
        host->FatalError(location, __FILE_NAME__,__LINE__, "illegal character %c was input.", location.line, location.column, c);
        return;
    }
    
    if (c == '.') {
        if (state == numberState && isdigit(next)) {
            state = doubleState;
            buf.push_back(c);
            return;
        }
        
        if (next == '.' && isPossibleForPPPTok) {
            // .. <- .
            buf.push_back(c);
            buf.push_back(next);
            genTok();
            skipFlag = true;
            return;
        }
        else if (next == '.' && !isPossibleForPPPTok) {
            genTok();
            buf.push_back(c);
            isPossibleForPPPTok = true;
            return;
        }
        else if (isPossibleForPPPTok && next != '.'){
            // .. token
            host->FatalError(location, __FILE_NAME__, __LINE__, "invalid input '..' . Did you mean '...'?");
        }
        if (isalpha(next)) {
            AccessTokFlag = true;
        }
        genTok();
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (c == ',') {
        if (state == numberState && isdigit(next)) {
            return;
        }
        genTok();
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (c == '(') {
        genTok();
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (c == ')') {
        genTok();
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (c == '{') {
        genTok();
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (c == '}') {
        genTok();
        buf.push_back(c);
        genTok();
        return;
    }
    
    if (true
        && state == initState
        && c == '-'
        && isnumber(next)) {
        state = numberState;
        buf.push_back(c);
        return;
    }
    
    if (false
        || c == '+'
        || c == '-'
        || c == '='
        || c == '&'
        || c == '*'
        || c == '/'
        || c == '^'
        || c == '!'
        || c == '%'
        || c == '$'
        || c == '@'
        || c == ':'
        || c == '<'
        || c == '>'
        || c == '|') {
        if (state == initState) {
            state = operatorState;
            buf.push_back(c);
            return;
        }
        if (state == operatorState) {
            buf.push_back(c);
            return;
        }
        genTok();
        buf.push_back(c);
        state = operatorState;
        return;
    }
    
    host->FatalError(location, __FILE_NAME__,__LINE__, "in %d.%d illegal character %c was input.", location.line, location.column, c);
}

void Token::enumerate(){
    Token *a = this->first();
    while (a != nullptr) {
        a->out();
        a = a->next;
    }
}

void Lexer::lex(std::string str){
    int length = str.length();
    host->targettingFile = "User_input";
    for (int i = 0; i < length; i++) {
        if (host->hasProbrem()) {
            return;
        }
        input(str[i], str[i + 1]);
        if (str[i+1] == '\0') {
            genTok();
            break;
        }
    }
    genTok();
    addEOFToken();
    host->location = location;
    //printf("%d.%d\n", host->location.line, host->location.column);
    host->tokens = (tokens->first());
}

std::string Function::codegen(IRStream &f){
    f << "define " << return_type.ir_used << " @" << name << "(";
    int i = 0;
    for (auto arg = argTypes.begin(); arg != argTypes.end(); arg = std::next(arg)) {
        f << (*arg).ir_used << " %" << std::to_string(i);
        if (argTypes.size() != i+1) {
            f << ", ";
            
        }else{
            f << "){\n";
            break;
        }
        i++;
    }
    
    return getMangledName();
}

string Function::asValue(){
    return lltype() + "* @" + mangledName;
}

void Lexer::clearTokens(){
    if (tokens == nullptr) {
        return;
    }
    tokens->release();
    tokens = nullptr;
    state = initState;
    buf = "";
    commentDepth = 0;
}

Token *Lexer::getTokens(){ return tokens->first(); }

Token *Lexer::getNextTok(){
    tokens = tokens->next;
    return tokens;
}

Token *Lexer::eatThisTok(){
    tokens = tokens->next;
    return tokens;
}

Token *Lexer::lexString(_Scheat *sch, std::string sstream){
    Lexer lexer(sch);
    lexer.lex(sstream);
    sch->tokens = lexer.tokens;
    return lexer.tokens;
}

Token *Lexer::lexThis(_Scheat *sch){
    std::ifstream ifs(sch->sourceFile);
    if (!ifs.is_open()) {
        sch->Warning(SourceLocation(), __FILE_NAME__, __LINE__, "file %s does not exists. ", sch->sourceFile.c_str());
        return nullptr;
    }
    Lexer lexer(sch);
    lexer.lex(ifs);
    sch->tokens = lexer.getTokens();
    return lexer.getTokens();
}
