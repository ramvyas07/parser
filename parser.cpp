#include<iostream>
#include<fstream>
#include "parse.h"
#include "lex.cpp"
#include "lex.h"
#include<map>
#include<string>

using namespace std;

LexItem token;
bool trial;
int st_cnt = 0;


bool Prog(istream& in, int& line) {
	
	token = Parser::GetNextToken(in, line);
	if (token != BEGIN) {
		ParseError(line, "BEGIN Missing");
		return false;
	}
	bool st = StmtList(in, line);
	if (st == false) {
		
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != END) {
		ParseError(line, "END Missing");
		return false;
	}
	return true;
	

}
bool StmtList(istream& in, int& line) {
	bool t1 = Stmt(in, line);
	if (t1 == false) {
		return false;
	}
	token = Parser::GetNextToken(in, line);
	
	if (token == SCOMA) {
		token = Parser::GetNextToken(in, line);
		//cout << token << endl;
		if (token == END) {
			Parser::PushBackToken(token);
			return true;
		}
		else {
			Parser::PushBackToken(token);
			return StmtList(in, line);
		}
		
	}
	ParseError(line, "Missing Semicolon");
	return false;

}
bool Stmt(istream& in, int& line) {
	bool status;
	token = Parser::GetNextToken(in, line);
	if (token == PRINT) {
		status = PrintStmt(in, line);
		if (status == true) {
			st_cnt++;
		}
		return status;
		
	}
	else if (token == IF) {
		Parser::PushBackToken(token);
		status = IfStmt(in, line);
		if (status == true) {
			st_cnt++;
		}
		return status;
	}
	else if(token == IDENT){
		Parser::PushBackToken(token);
		status = AssignStmt(in, line);
		if (status == true) {
			st_cnt++;
		}
		return status;
	}
	ParseError(line, "Invalid Statement");
	return false;

}
bool PrintStmt(istream& in, int& line) {
	bool ex = ExprList(in, line);
	if (ex == false) {
		ParseError(line, "Missing Expression after print");
		return false;
	}
	return true;
}
bool IfStmt(istream& in, int& line) {
	token = Parser::GetNextToken(in, line);
	if (token != IF) {
		ParseError(line, "IF missing");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != LPAREN) {
		ParseError(line, "LPAREN missing");
		return false;
	}
	bool expr = Expr(in, line);
	if (expr == false) {
		ParseError(line, "Expression missing inside IF");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != RPAREN) {
		ParseError(line, "RPAREN missing");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token != THEN) {
		ParseError(line, "THEN missing");
		return false;
	}
	bool stmt = Stmt(in, line);
	if (stmt == false) {
		ParseError(line, "Statement missing");
		return false;
	}
	return true;
}
bool Var(istream& in, int& line) {
	string istr;
	token = Parser::GetNextToken(in, line);
	if (token == IDENT) {
		istr = token.GetLexeme();
		if (defVar.find(istr) == defVar.end()) {
			defVar[istr] = true;
		}
		return true;
	}
	ParseError(line, "Invalid Statement");
	return false;
}
bool AssignStmt(istream& in, int& line) {
	bool ident = Var(in, line);
	if (ident == false) {
		ParseError(line, "Ident missing");
		return false;
	}
	 token = Parser::GetNextToken(in, line);
	 if (token != EQ) {
		 ParseError(line, "= required");
		 return false;
	 }
	 bool expr = Expr(in, line);
	 if (expr == false) {
		 ParseError(line, "Expression Missing after EQ");
		 return false;
	 }
	 return true;
}
bool ExprList(istream& in, int& line) {
	bool t1 = Expr(in, line);
	if (t1 == false) {
		ParseError(line, "Expression Missing");
		return false;
	}
	token = Parser::GetNextToken(in, line);
	if (token == COMA) {
		return Expr(in, line);
	}
	else{
		Parser::PushBackToken(token);
		return true;
	}
	
}
bool Expr(istream& in, int& line) {
	bool t1 = Term(in, line);
	if (t1 == false) {
		//ParseError(line, "Missing Expression");
		return false;
	}
	for (;;) {
		token = Parser::GetNextToken(in, line);
		if (token == ERR) {
			ParseError(line, "Unrecognized Input Pattern (" + token.GetLexeme() + ")");
			return false;
		}
		if (token != PLUS && token != MINUS) {
			Parser::PushBackToken(token);
			return true;
		}
		bool t2 = Expr(in, line);

		if (t2 == false) {
			ParseError(token.GetLinenum(), "Factor or Expression required after + or -");
			return false;
		}
		if (token == PLUS || token == MINUS) {
			return true;
		}
	}

	return false;

}
bool Term(istream& in, int& line) {
	bool t1 = Factor(in, line);
	if (t1 == false) {
		//ParseError(line, "Term Missing");
		return false;
	}
	for (;;) {
		token = Parser::GetNextToken(in, line);
		
		if (token != MULT && token != DIV) {
			Parser::PushBackToken(token);
			return true;
		}
		bool t2 = Term(in, line);

		if (t2 == false) {
			ParseError(token.GetLinenum(), "Factor or Expression required after * or /");
			return false;
		}
		if (token == MULT || token == DIV) {
			return true;
		}


	}
	return false;
}

bool Factor(istream& in, int& line) {
	token = Parser::GetNextToken(in, line);
	//cout << token << endl;
	if (token == ICONST) {
		//cout << token.GetLexeme() << " " << token << endl;
		return true;
	}
	if (token == SCONST) {
		//cout << token.GetLexeme() << " " << token << endl;
		return true;
	}
	if (token == RCONST) {
		//cout << token.GetLexeme() << " " << token << endl;
		return true;
	}
	if (token == IDENT) {
		//cout << token.GetLexeme() << " " << token << endl;
		string identstr;
		identstr = token.GetLexeme();
		if (defVar.find(identstr) == defVar.end()) {
			ParseError(line, "Variable not defined");
			return false;
		}
		return true;
	}
	if (token == LPAREN) {
		bool exp = Expr(in, line);
		if (exp == false) {
			ParseError(line, "expression expected");
			return false;
		}
		token = Parser::GetNextToken(in, line);
		if (token != RPAREN) {
			ParseError(line, "Right Paren Expected after Expr");
			return false;
		}
		return true;

	}
	
		ParseError(line, "Unrecognized Input ("+token.GetLexeme()+")");
		return false;

	
	
}