#include<iostream>
#include<regex>
#include "lex.h"
#include <map>
#include<string> 


using namespace std;
static map<Token, string> tokenPrint = { {PRINT,"PRINT"},{IF,"IF"},{BEGIN,"BEGIN"},{END,"END"},{IDENT,"IDENT"},{ICONST,"ICONST"},{SCONST,"SCONST"}, {RCONST,"RCONST"},{PLUS,"PLUS"},{MINUS,"MINUS"},{MULT,"MULT"},{DIV,"DIV"},{EQ,"EQ"},{LPAREN,"LPAREN"},{RPAREN,"RPAREN"},{SCOMA,"SCOMA"},{COMA,"COMA"},{ERR,"ERR"},{DONE,"DONE"}, {THEN,"THEN"} };

inline ostream& operator<<(ostream& out, const LexItem& tok) {

    string* token = &tokenPrint[tok.GetToken()];
    cout << *token;
    if (tok.GetToken() == SCONST || tok.GetToken() == RCONST || tok.GetToken() == ICONST || tok.GetToken() == IDENT || tok.GetToken() == ERR) {
        cout << "(" << tok.GetLexeme() << ")";
    }
    return out;
}
LexItem tCurr;
//LexItem tPrev;

LexItem getNextToken(istream& in, int& linenum) {
    enum TokState { START, INID, INSTRING, ININT, INREAL, INCOMMENT, SIGN } lexState = START;
    string res[5] = { "begin", "print","if", "then","end" };
    string lexem;
    char c1;

    while (in.get(c1)) {

        switch (lexState) {

        case START:
            if (c1 == '\n') {
                linenum++;
            }


            if (in.peek() == -1) {
                return LexItem(DONE, lexem, linenum);
            }

            if (isspace(c1)) {
                continue;
            }

            lexem = c1;

            if (c1 == '/' && char(in.peek()) == '/') {
                lexState = INCOMMENT;
                continue;
            }

            if (c1 == '+' || c1 == '-' || c1 == '*' || c1 == '/' || c1 == '(' || c1 == ')' || c1 == '=' || c1 == ',' || c1 == ';') {
                lexState = SIGN;
                continue;
            }

            if (c1 == '\"') {
                lexState = INSTRING;
                continue;
            }

            if (isdigit(c1)) {
                lexState = ININT;
                continue;
            }

            if (c1 == '.') {
                lexState = INREAL;
                continue;
            }

            if (isalpha(c1)) {
                lexState = INID;
                continue;
            }
            return LexItem(ERR, lexem, linenum);

        case INID:

            if (regex_match(lexem + c1, regex("[a-zA-Z][a-zA-Z0-9]*"))) {
                lexem += c1;
                continue;
            }
            else if (!regex_match(lexem + c1, regex("[a-zA-Z][a-zA-Z0-9]*")) || in.peek() == -1) {
                lexState = START;
                in.putback(c1);
            }

            if (lexem == "begin") {
                tCurr = LexItem(BEGIN, lexem, linenum);
            }
            else if (lexem == "print") {
                tCurr = LexItem(PRINT, lexem, linenum);
            }
            else if (lexem == "end") {
                /*if (tPrev.GetToken() != SCOMA){
                    return LexItem(ERR, tPrev.GetLexeme(), linenum);
                }*/
                tCurr = LexItem(END, lexem, linenum);
            }
            else if (lexem == "if") {
                tCurr = LexItem(IF, lexem, linenum);
            }
            else if (lexem == "then") {
                tCurr = LexItem(THEN, lexem, linenum);
            }
            else if (res[0].find(lexem + char(in.peek())) != -1 || res[1].find(lexem + char(in.peek())) != -1 || res[2].find(lexem + char(in.peek())) != -1 || res[3].find(lexem + char(in.peek())) != -1 || res[4].find(lexem + char(in.peek())) != -1) {
                continue;
            }
            else {
                tCurr = LexItem(IDENT, lexem, linenum);
            }
            return tCurr;

            break;

        case INSTRING:

            if (c1 == '\n') {
                return LexItem(ERR, lexem, linenum);
            }
            if (regex_match(lexem + c1, regex("\"[ -~]*"))) {
                lexem += c1;
            }
            if (regex_match(lexem + c1, regex("\"[ -~]*\""))) {
                lexState = START;
                tCurr = LexItem(SCONST, lexem, linenum);
                return tCurr;
            }
            break;

        case ININT:
            if (isalpha(c1)) {
                return LexItem(ERR, lexem + c1, linenum);
            }
            if (regex_match(lexem + c1, regex("[0-9]+"))) {
                lexem += c1;
            }
            else if (c1 == '.') {
                lexState = INREAL;
                in.putback(c1);
                continue;
            }

            else {
                lexState = START;
                in.putback(c1);
                tCurr = LexItem(ICONST, lexem, linenum);
                return tCurr;
            }
            break;

        case INREAL:
            if (isalpha(c1)) {
                return LexItem(ERR, lexem + c1, linenum);
            }
            if (regex_match(lexem + c1, regex("[0-9]*\\.[0-9]+"))) {
                lexem += c1;
            }
            else if (regex_match(lexem + c1, regex("[0-9]*\\.[0-9]*"))) {
                lexem += c1;
            }
            else {
                if (lexem[lexem.length() - 1] == '.') {
                    return LexItem(ERR, lexem, linenum);
                }
                lexState = START;
                in.putback(c1);
                tCurr = LexItem(RCONST, lexem, linenum);
                return tCurr;

            }
            break;

        case INCOMMENT:
            if (c1 == '\n') {
                linenum++;
                lexState = START;
            }
            continue;

        case SIGN:
            in.putback(c1);
            if (lexem == "+") {
                tCurr = LexItem(PLUS, lexem, linenum);
            }
            else if (lexem == "-") {
                tCurr = LexItem(MINUS, lexem, linenum);
            }
            else if (lexem == "*") {
                tCurr = LexItem(MULT, lexem, linenum);
            }
            else if (lexem == "/") {
                tCurr = LexItem(DIV, lexem, linenum);
            }
            else if (lexem == "=") {
                tCurr = LexItem(EQ, lexem, linenum);
            }
            else if (lexem == "(") {
                tCurr = LexItem(LPAREN, lexem, linenum);
            }
            else if (lexem == ")") {
                tCurr = LexItem(RPAREN, lexem, linenum);
            }
            else if (lexem == ";") {
                tCurr = LexItem(SCOMA, lexem, linenum);
            }
            else if (lexem == ",") {
                tCurr = LexItem(COMA, lexem, linenum);
            }
            else {
                return LexItem(ERR, lexem + c1, linenum);
            }

            return tCurr;
            break;




        }




    }
    return LexItem(DONE, "", linenum);

}
