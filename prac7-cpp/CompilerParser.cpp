#include "CompilerParser.h"
#include <iostream>

/**
 * Constructor for the CompilerParser
 * @param tokens A linked list of tokens to be parsed
 */
CompilerParser::CompilerParser(std::list<Token*> tokens) {
    this->tokens = tokens; // store our program tokens
    tokenIterator = this->tokens.begin();
    this->tokenCount = 0;
}



void CompilerParser::printCurrent(){
    std::cout << current()->getType() << " " << current()->getValue() << "\n";
}


/**
 * Generates a parse tree for a single program
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileProgram() {

    ParseTree* result = new ParseTree("class", "");

    result->addChild(mustBe("keyword", "class"));
    result->addChild(mustBe("identifier", "Main"));
    result->addChild(mustBe("symbol", "{"));
    result->addChild(mustBe("symbol", "}"));

    return result;
}  

/**
 * Generates a parse tree for a single class
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileClass() {

    ParseTree* result = new ParseTree("class", "");

    /* class Name {
        ...
    
    */
    result->addChild(mustBe("keyword", "class"));

    if(current()->getType() == "identifier"){
        result->addChild(current()); //Main, Bob, Apple
    }
    else{
        throw ParseException();
    }

    next();
    result->addChild(mustBe("symbol", "{"));
    
    // class contents

    // iterates over each variable declaration 
    while(true){

        // the class contains subroutines we must evaluate 
        if(current()->getValue() == "static" 
        || current()->getValue() == "field"){
            
            result->addChild(compileClassVarDec());
        }
        else{
            break;
        }
    }

    // iterates over each subroutine 
    while(true){

        // the class contains subroutines we must evaluate 
        if(current()->getValue() == "function" 
        || current()->getValue() == "method"
        || current()->getValue() == "constructor"){

            result->addChild(compileSubroutine());
        }
        else{
            break;
        }
    }

    // class end }
    result->addChild(mustBe("symbol", "}"));  

    return result;
}

/**
 * Generates a parse tree for a static variable declaration or field declaration
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileClassVarDec() {
    
    ParseTree* result = new ParseTree("classVarDec", "");

    //we have already established this is a either field or static
    result->addChild(new Token("keyword", current()->getValue()));
    next();
    // type of variable
    result->addChild(new Token("keyword", current()->getValue()));
    next();

    // iterating over each variable declaration
    while(true){
        if(current()->getType() == "identifier"){
            result->addChild(current());
            next();
        }
        else if(have("symbol", ",")){
            result->addChild(current());
            next();
        }
        else if(have("symbol", ";")){
            result->addChild(current());
            next();
            break;
        }
        else{
            throw ParseException();
        }
    }


    return result;
}



/**
 * Generates a parse tree for a method, function, or constructor
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileSubroutine() {
    
    ParseTree* result = new ParseTree("subroutine", "");


    //we have already established this is either constructor, function or method
    result->addChild(new Token("keyword", current()->getValue()));
    next();
    
    auto val = current()->getValue(); // either built in type (keyword), or className (identifier)
    if(current()->getType() == "keyword" || current()->getType() == "identifier"){
        result->addChild(current());
    } 
    else{
        throw ParseException();
    }

    next();


    if(current()->getType() == "identifier"){
        result->addChild(new Token("identifier", current()->getValue()));
    } 
    else{
        throw ParseException();
    }

    

    next();

    // compiling parameters...
    result->addChild(mustBe("symbol", "("));
    result->addChild(compileParameterList());
    result->addChild(mustBe("symbol", ")"));

    // compiling inside of subroutine
    result->addChild(compileSubroutineBody());

    return result;

}

/**
 * Generates a parse tree for a subroutine's parameters
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileParameterList() {
    ParseTree* result = new ParseTree("parameterList", "");
 
    if(have("symbol", ")")){
        return result;
    }


    // iterate each parameter...
    while(true){

        // type of parameter, either built in type (keyword) or className (identifier)
        if(current()->getType() == "keyword" || current()->getType() == "identifier"){
            result->addChild(current());
        } 
        else{
            throw ParseException();
        }
        next();

        // name of parameter
        if(current()->getType() == "identifier"){
            result->addChild(current());
        }
        else{
            throw ParseException();
        }

        // (int a, int b)    

        next();
        if(!have("symbol", ",")){
            break;
        }
        else{
            // adding ','
            result->addChild(current());
            next();
        }
    }
    return result;
}

/**
 * Generates a parse tree for a subroutine's body
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileSubroutineBody() {

    ParseTree* result = new ParseTree("subroutineBody", "");

    result->addChild(mustBe("symbol", "{"));
    
    // iterating over each var declaration
    while(true){
        if(have("keyword", "var")){
            result->addChild(compileVarDec());
        }
        else{
            break;
        }
    }
    
    // iterates over each statement
    if(!have("symbol", "}")){
        result->addChild(compileStatements());
    }
    result->addChild(mustBe("symbol", "}"));

    return result;
}

/**
 * Generates a parse tree for a subroutine variable declaration
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileVarDec() {
    
    ParseTree* result = new ParseTree("varDec", "");

    result->addChild(mustBe("keyword", "var"));

    // type of var, either built in type (keyword) or className (identifier)
    if(current()->getType() == "keyword" || current()->getType() == "identifier"){
        result->addChild(current());
        next();
    } 
    else{
        throw ParseException();
    }

    // iterating over each variable declaration
    while(true){
        
        if(current()->getType() == "identifier"){
            result->addChild(current());
            next();
        }
        else if(have("symbol", ",")){
            result->addChild(current());
            next();
        }
        else if(have("symbol", ";")){
            result->addChild(current());
            next();
            break;
        }
        else{
            throw ParseException();
        }
    }

    return result;
}

/**
 * Generates a parse tree for a series of statements
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileStatements() {

    ParseTree* result = new ParseTree("statements", "");

    // iterate over each statement
    while(true){
        if(have("keyword", "return")){
            result->addChild(compileReturn());
        }
        else if(have("keyword", "let")){
            result->addChild(compileLet());
        }
        else if(have("keyword", "if")){
            result->addChild(compileIf());
        }
        else if(have("keyword", "while")){
            result->addChild(compileWhile());
        }
        else if(have("keyword", "do")){
            result->addChild(compileDo());
        }
        else{
            break;
        }
        next();
    }

    return result;
}

/**
 * Generates a parse tree for a let statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileLet() {
    return NULL;
}

/**
 * Generates a parse tree for an if statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileIf() {
    
    ParseTree* result = new ParseTree("ifStatement", "");

    result->addChild(mustBe("keyword", "if"));
    
    result->addChild(compileExpression());

    result->addChild(mustBe("symbol", "{"));
    result->addChild(compileStatements());
    result->addChild(mustBe("symbol", "}"));

    if(have("keyword", "else")){
        next();
        result->addChild(mustBe("symbol", "{"));
        result->addChild(compileStatements());
        result->addChild(mustBe("symbol", "}"));
    }

    return result;
}

/**
 * Generates a parse tree for a while statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileWhile() {
    
    ParseTree* result = new ParseTree("whileStatement", "");

    result->addChild(mustBe("keyword", "while"));
    
    result->addChild(compileExpression());

    result->addChild(mustBe("symbol", "{"));
    result->addChild(compileStatements());
    result->addChild(mustBe("symbol", "}"));

    return result;
}
/**
 * Generates a parse tree for a do statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileDo() {
    
    ParseTree* result = new ParseTree("doStatement", "");

    result->addChild(mustBe("keyword", "do"));

    result->addChild(compileExpression());

    result->addChild(mustBe("symbol", ";"));
    return result;
}

/**
 * Generates a parse tree for a return statement
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileReturn() {
    
    ParseTree* result = new ParseTree("returnStatement", "");

    result->addChild(mustBe("keyword", "result"));

    if(have("symbol", ";")){
        result->addChild(current());
        next();
        return result;
    }
    else {
        result->addChild(compileExpression());
    }
    result->addChild(mustBe("symbol", ";"));

    return result;

}

/**
 * Generates a parse tree for an expression
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileExpression() {
    
    ParseTree* result = new ParseTree("expression", "");

    //check if the expression is just a term
    if(have("keyword", "skip")) {
        result->addChild(new Token("keyword", current()->getValue()));
        next();
        return result;
    }

    //check if there are more terms
    while(have("symbol", "+") 
        || have("symbol", "-") 
        || have("symbol", "*") 
        || have("symbol", "/") 
        || have("symbol", "&") 
        || have("symbol", "|") 
        || have("symbol", "<") 
        || have("symbol", ">") 
        || have("symbol", "=")){

        result->addChild(current());
        next();
        result->addChild(compileTerm());
    }

    next();
    return result;
}

/**
 * Generates a parse tree for an expression term
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileTerm() {

    ParseTree* result = new ParseTree("term", "");

    auto type = current()->getType();

    if(type == "stringConstant" || type == "keywordConstant" || type == "integerConstant"){
        result->addChild(current());
        next();
    }
    else if(type == "identifier"){
        result->addChild(current());

        //check for array
                
        next();

        if(have("symbol", "[")){

            next();
            result->addChild(compileExpression());
            result->addChild(mustBe("symbol", "]"));
        }
    }
    // sub expression
    else if(have("symbol", "(")){
        result->addChild(current());
        next();
        result->addChild(compileExpression());
        result->addChild(mustBe("symbol", ")"));
    }
    else if(type == "unaryOp"){
        result->addChild(current());
        next();
        if(current()->getType() == "term"){
            result->addChild(compileTerm());
        }
        else{
            throw ParseException();
        }
    }



    return result;

}

/**
 * Generates a parse tree for an expression list
 * @return a ParseTree
 */
ParseTree* CompilerParser::compileExpressionList() {
    return NULL;
}

/**
 * Advance to the next token
 */
void CompilerParser::next(){
    std::advance(tokenIterator, 1);
    tokenCount++;
    return;
}

/**
 * Return the current token
 * @return the Token
 */
Token* CompilerParser::current(){
    if(tokenCount >= tokens.size()){
        throw ParseException();
    }
    return *tokenIterator;
}

/**
 * Check if the current token matches the expected type and value.
 * @return true if a match, false otherwise
 */
bool CompilerParser::have(std::string expectedType, std::string expectedValue){
    auto token = current();

    if(tokenCount >= tokens.size()){
        // token out of bounds
        throw ParseException();
        return false;
    }
    
    if(token->getType() == expectedType && token->getValue() == expectedValue){
        return true;
    }
    return false;
}

/**
 * Check if the current token matches the expected type and value.
 * If so, advance to the next token, returning the current token, otherwise throw a ParseException.
 * @return the current token before advancing
 */
Token* CompilerParser::mustBe(std::string expectedType, std::string expectedValue){
    auto token = current();
    
    if(token->getType() == expectedType && token->getValue() == expectedValue){
        next();
        return token;
    }
    throw ParseException(); 
    return nullptr;
}

/**
 * Definition of a ParseException
 * You can use this ParseException with `throw ParseException();`
 */
const char* ParseException::what() {
    return "An Exception occurred while parsing!";
}
