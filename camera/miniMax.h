#ifndef MINIMAX_H
#define MINIMAX_H

#include<bits/stdc++.h> 
#include <stdlib.h>
using namespace std; 

struct Move 
{ 
	int row, col; 
}; 

class miniMax
{
private:
    int dif;
    bool isMovesLeft(char board[3][3]);
    int evaluate(char b[3][3]);
    int minimax(char board[3][3], int depth, bool isMax);
public:
    char player, opponent; 
    miniMax(bool x);
    ~miniMax();
    int getResult(char board[3][3]);
    Move findBestMove(char board[3][3]);
};

#endif
