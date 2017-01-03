#include <bits/stdc++.h>
using namespace std;

// Class definition
class Symbol {
public:
  enum Type {
    Integer, Function
  };
  string name;
  Type type;
  int value;
  Symbol() = default;
  Symbol(string name, Type type, int value):
    name(name),
    type(type),
    value(value){}
  // For Integer the value
  // For Function the line number
};

// Global datas
unordered_map<string, Symbol> symbols;
vector<string> program;

// Temp stores
char buffer[2][1024];

// Helper functions
bool beginWith(const string& source, const string& pattern) {
  return source.substr(0, pattern.size()) == pattern;
}

namespace LL1 {
  int expr (const string& expression, int& cursor);
  int pmexpr (const string& expression, int& cursor);

  void gonext (const string& expression, int& cursor) {
    if(isalnum(expression[cursor])) {
      while(isalnum(expression[cursor])) {
        cursor++;
      }
    }
    else {
      cursor++;
    }
    while(expression[cursor] == ' ') {
      cursor++;
    }
  }

  // LL1 functions

  int atom (const string& expression, int& cursor) {
    int ret;
    if(expression[cursor] == '(') {
      gonext(expression, cursor);
      ret = expr(expression, cursor);
      gonext(expression, cursor);
    } else if(isdigit(expression[cursor])) {
      sscanf(expression.c_str() + cursor, "%d", &ret);
      gonext(expression, cursor);
    } else if(isalpha(expression[cursor])) {
      sscanf(expression.c_str() + cursor, "%[a-zA-Z]", buffer[0]);
      string s(buffer[0]);
      ret = symbols[s].value;
      gonext(expression, cursor);
    }
    return ret;
  }

  pair<bool, int> rpmexpr (const string& expression, int& cursor) {
    if(expression[cursor] == '*') {
      gonext(expression, cursor);
      return make_pair(true, pmexpr(expression, cursor));
    } else if (expression[cursor] == '/') {
      gonext(expression, cursor);
      return make_pair(false, pmexpr(expression, cursor));
    } else {
      return make_pair(true, 1);
    }
  }

  int pmexpr (const string& expression, int& cursor) {
    int temp = atom(expression, cursor);
    int temp2;
    bool op;
    tie(op, temp2) = rpmexpr(expression, cursor);
    return op ? temp * temp2 : temp / temp2;
  }

  int rexpr (const string& expression, int& cursor) {
    if(expression[cursor] == '+') {
      gonext(expression, cursor);
      return expr(expression, cursor);
    } else if (expression[cursor] == '-') {
      gonext(expression, cursor);
      return -expr(expression, cursor);
    } else {
      return 0;
    }
  }

  int expr (const string& expression, int& cursor) {
    int temp = pmexpr(expression, cursor);
    return temp + rexpr(expression, cursor);
  }
}

int parseExpression (string expression) {
  int cursor = 0;
  expression.push_back('$');
  return LL1::expr(expression, cursor);
}

void readProgram () {
  string temp;
  while(getline(cin, temp)) {
    if(temp != "") {
      program.push_back(temp);
    }
  }
}

// Executers
bool executeStatement (const string& statement, unsigned& lineno);
bool executeLine (const string& line, unsigned& lineno);

void executeAssign (const string& statement, unsigned& lineno) {
  sscanf(statement.c_str(), "%*s%s %[^\n]", buffer[0], buffer[1]);
  string symbol(buffer[0]), expression(buffer[1]);
  int value = parseExpression(expression);
  symbols[symbol] = Symbol(symbol, Symbol::Integer, value);
  cout << "Assigning " << value << " to " << symbol << endl;
  lineno++;
}

void executeDefine (const string& statement, unsigned& lineno) {
  sscanf(statement.c_str(), "%*s%s", buffer[0]);
  string symbol(buffer[0]);
  symbols.emplace(make_pair(symbol, Symbol(symbol, Symbol::Function, lineno)));
  cout << "Defining " << symbol << endl;
  while(!beginWith(program[lineno], "END")) {
    lineno++;
  }
  lineno++;
}

void executeCall (const string& statement, unsigned& lineno) {
  sscanf(statement.c_str(), "%*s%s", buffer[0]);
  string symbol(buffer[0]);
  cout << "Calling " << symbol << endl;
  unsigned sl = symbols[symbol].value + 1;
  while(executeLine(program[sl], sl))
  ;
  lineno++;
}

bool executeStatement (const string& statement, unsigned& lineno) {
  if(beginWith(statement, "ASSIGN")) {
    executeAssign(statement, lineno);
  } else if(beginWith(statement, "DEFINE")) {
    executeDefine(statement, lineno);
  } else if(beginWith(statement, "END")) {
    // Block end
    return false;
  } else if(beginWith(statement, "CALL")) {
    executeCall(statement, lineno);
  }
  // Block not finished
  return true;
}

bool executeLine (const string& line, unsigned& lineno) {
  if(!beginWith(line, "FOR")) {
    // If it doesn't begin with "FOR", it's a single statement
    return executeStatement(line, lineno);
  } else {
    int cursor = 0;
    sscanf(line.c_str(), "%*s %[^\n]", buffer[0]);
    string blendExpr(buffer[0]);
    int times = LL1::expr(blendExpr, cursor);
    unsigned dummy = 0;
    for(int i = 0; i < times; i++) {
      executeStatement(blendExpr.substr(cursor), dummy);
    }
    lineno++;
    return true;
  }
}

int main () {
  readProgram();
  unsigned lp = 0;
  while(lp < program.size()) {
    executeLine(program[lp], lp);
  }
}
