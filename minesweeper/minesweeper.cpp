#include "board.hpp"

int main(){
    srand(time(0));
    board Board(9, 9, 10); 
    Board.initScreen();
    Board.initTable();

    while(true){
        Board.getScreenDims();
        Board.playerMove();
        if(! Board.inCustMenu()){Board.drawBoard();}
        Board.drawUi();
        Board.printBuffer();
    }
    return 0;
}
