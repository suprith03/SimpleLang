#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <cstring>

#define MAX_TOKEN_LEN 100
using namespace std;

// token types
enum TokenType {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_IF, TOKEN_EQUAL, TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_SEMICOLON, TOKEN_UNKNOWN, TOKEN_EOF
};

// token structure
struct Token {
    TokenType type;
    char text[MAX_TOKEN_LEN];
};

// lexer function
void getNextToken(ifstream &file, Token &token) {
    char c;
    while (file.get(c)) {
        if (isspace(c)) continue;
        if (isalpha(c)) {
            int len = 0;
            token.text[len++] = c;
            while (file.get(c) && isalnum(c)) {
                if (len < MAX_TOKEN_LEN - 1) token.text[len++] = c;
            }
            file.unget();
            token.text[len] = '\0';
            if (strcmp(token.text, "int") == 0) token.type = TOKEN_INT;
            else if (strcmp(token.text, "if") == 0) token.type = TOKEN_IF;
            else token.type = TOKEN_IDENTIFIER;
            return;
        }
        if (isdigit(c)) {
            int len = 0;
            token.text[len++] = c;
            while (file.get(c) && isdigit(c)) {
                if (len < MAX_TOKEN_LEN - 1) token.text[len++] = c;
            }
            file.unget();
            token.text[len] = '\0';
            token.type = TOKEN_NUMBER;
            return;
        }
        switch (c) {
            case '=': token.type = TOKEN_ASSIGN; token.text[0] = '='; token.text[1] = '\0'; return;
            case '+': token.type = TOKEN_PLUS; token.text[0] = '+'; token.text[1] = '\0'; return;
            case '-': token.type = TOKEN_MINUS; token.text[0] = '-'; token.text[1] = '\0'; return;
            case '{': token.type = TOKEN_LBRACE; token.text[0] = '{'; token.text[1] = '\0'; return;
            case '}': token.type = TOKEN_RBRACE; token.text[0] = '}'; token.text[1] = '\0'; return;
            case ';': token.type = TOKEN_SEMICOLON; token.text[0] = ';'; token.text[1] = '\0'; return;
        }
    }
    token.type = TOKEN_EOF;
    token.text[0] = '\0';
}

// AST node structure
struct ASTNode {
    TokenType type;
    char text[MAX_TOKEN_LEN];
    ASTNode *left;
    ASTNode *right;

    ASTNode(TokenType t, const char *txt) : type(t), left(nullptr), right(nullptr) {
        strncpy(text, txt, MAX_TOKEN_LEN);
    }
};

// Parsing functions
ASTNode* parseExpression(vector<Token> &tokens, int &index) {
    ASTNode *node = new ASTNode(tokens[index].type, tokens[index].text);
    index++;
    if (tokens[index].type == TOKEN_ASSIGN) {
        ASTNode *assignNode = new ASTNode(tokens[index].type, tokens[index].text);
        index++;
        assignNode->left = node;
        assignNode->right = parseExpression(tokens, index);
        return assignNode;
    }
    return node;
}

ASTNode* parse(vector<Token> &tokens) {
    int index = 0;
    return parseExpression(tokens, index);
}

// printing AST
void printAST(ASTNode *node, int level = 0) {
    if (!node) return;
    for (int i = 0; i < level; i++) cout << "  ";
    cout << node->text << endl;
    printAST(node->left, level + 1);
    printAST(node->right, level + 1);
}

// generating code
void generateCode(ASTNode *node) {
    if (!node) return;
    if (node->type == TOKEN_ASSIGN) {
        generateCode(node->right);
        cout << "MOV " << node->left->text << ", ACC" << endl;
    } else if (node->type == TOKEN_NUMBER) {
        cout << "LOADI " << node->text << endl;
    } else if (node->type == TOKEN_IDENTIFIER) {
        cout << "LOAD " << node->text << endl;
    }
}

int main() {
    ifstream file("input.txt");
    if (!file.is_open()) {
        cerr << "Failed to open file" << endl;
        return 1;
    }

    vector<Token> tokens;
    Token token;
    do {
        getNextToken(file, token);
        tokens.push_back(token);
    } while (token.type != TOKEN_EOF);
    file.close();

    ASTNode *root = parse(tokens);
    printAST(root);
    generateCode(root);

    return 0;
}
