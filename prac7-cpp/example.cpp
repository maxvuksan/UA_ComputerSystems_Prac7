void CompilerParser::next(){
    if(tokensIn.size()!= 0) {
        tokensIn.pop_front();
    }
}

/
 
Return the current token
@return the Token
*/
Token* CompilerParser::current(){
    Token* result = tokensIn.front();
    return result;
}

/
 
Check if the current token matches the expected type and value.
@return true if a match, false otherwise
*/
bool CompilerParser::have(std::string expectedType, std::string expectedValue){
    if(tokensIn.size() == 0) {
        return false;
    }
    if(tokensIn.front()->getType() == expectedType && tokensIn.front()->getValue() == expectedValue) {
        return true;
    }
    return false;
}

/**
 
Check if the current token matches the expected type and value.
If so, advance to the next token, returning the current token, otherwise throw a ParseException.
@return the current token before advancing
*/
Token* CompilerParser::mustBe(std::string expectedType, std::string expectedValue){
    if(tokensIn.front()->getType() == expectedType && tokensIn.front()->getValue() == expectedValue) {
        Token* result = tokensIn.front();
        next();
        return result;
    }
    throw ParseException();
    return NULL;
}

ParseTree* CompilerParser::compileTerm() {
    Token* currentToken = current();
    ParseTree* result = new ParseTree("term", currentToken->getValue());

    if (currentToken->getType() == "integerConstant") {
        result->addChild(mustBe("integerConstant", currentToken->getValue()));

    } else if (currentToken->getType() == "stringConstant") {
        result->addChild(mustBe("stringConstant", currentToken->getValue()));

    } else if (currentToken->getType() == "keyword") {
        result->addChild(mustBe("keyword", currentToken->getValue()));

    } else if (currentToken->getType() == "varName") {
        result->addChild(mustBe("identifier", currentToken->getValue()));
        if(currentToken->getValue() == "[") {
            result->addChild(mustBe("symbol", "["));
        }
        result->addChild(compileExpression());
        if(currentToken->getValue() == "]") {
            result->addChild(mustBe("symbol", "]"));
        }

    } else if (currentToken->getValue() == "(") {
        result->addChild(mustBe("symbol", "("));
        result->addChild(compileExpression());

    } else if (currentToken->getValue() == ")") {
        result->addChild(mustBe("symbol", ")"));

    } else if (currentToken->getValue() == "_" || currentToken->getValue() == "~") {
        result->addChild(mustBe("unaryOp",currentToken->getValue()));

    } else if (currentToken->getValue() == "subroutineCall") {
        result->addChild((mustBe("identifier", currentToken->getValue())));
        result->addChild(mustBe("symbol", "("));
        result->addChild(compileExpressionList());
        result->addChild(mustBe("symbol", ")"));

    }

    return result;
}

ParseTree* CompilerParser::compileClass() {
    ParseTree* result;

    if(mustBe("keyword", "class")) {
        ParseTree* temp = new ParseTree("keyword", "class");
        result->addChild(temp);
    }

    Token* currentToken = current();
    if(currentToken->getType() == "identifier") {
        ParseTree* temp = new ParseTree("identifier", currentToken->getValue());
        result->addChild(temp);
    }

    if(mustBe("symbol", "{")) {
        ParseTree* temp = new ParseTree("symbol", "{");
        result->addChild(temp);
    }


    mustBe("symbol", "}");

}
CompilerParser::CompilerParser(std::list<Token*> tokens) {
    while(tokens.front() != nullptr) {
        tokensIn.push_back(tokens.front());
        tokens.pop_front();
    }
}

main()
 |
 |--> parseProgram()
       |
       |--> compileClass()
             |
             |--> compileClassVarDec()?
             |     |
             |     |--> compileType()
             |     |--> compileVarName() 
             |
             |--> compileSubroutine()?
                   |
                   |--> compileParameterList()
                   |--> compileSubroutineBody()
                         |
                         |--> compileVarDec()?
                         |--> compileStatements()
                               |
                               |--> compileDo()?
                               |--> compileReturn()?
                               |--> compileExpression()
                                     |
                                     |--> compileTerm()
                                     |--> compileExpressionList()?