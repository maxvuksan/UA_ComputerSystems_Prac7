#include <iostream>
#include <list>

#include "CompilerParser.h"
#include "Token.h"

using namespace std;

int main(int argc, char *argv[]) {

    /* Tokens for:
        class Main { function void test ( ) { } }  
     */
    list<Token*> tokens;
        tokens.push_back(new Token("symbol", "{"));
        tokens.push_back(new Token("keyword", "var"));
        tokens.push_back(new Token("keyword", "int"));
        tokens.push_back(new Token("identifier", "a"));  
        tokens.push_back(new Token("symbol", ";"));
        tokens.push_back(new Token("symbol", "}"));
    try {
        CompilerParser parser(tokens);
        ParseTree* result = parser.compileSubroutineBody();
        if (result != NULL){
            cout << result->tostring() << endl;
        }
    } catch (ParseException e) {
        cout << "Error Parsing!" << endl;
    }
}