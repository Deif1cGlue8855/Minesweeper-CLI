#ifndef board_HPP
#define board_HPP

//Include libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <sstream>
#include <ncurses.h> 
#include <sys/ioctl.h>
#include <unistd.h>

struct tile{
    int value;
    bool isMine = false;
    bool isReveal = false;
    bool isFlag = false;
};
struct RGB{
    int r,g,b;
};

struct player{
    int x = 0;
    int y = 0;
};

class board{
    private:
        std::vector<std::vector<tile>> table;
        int w, h, numOfMines;

        int custW = 10;
        int custH = 10;
        int custM = 40;
        int inMenuSelector = 0;
        bool custMenu = false;

        std::string saveFile = "save.txt";

        RGB red = {255,0,0};
        RGB gruvYellow = {215, 153, 33};
        RGB green = {0,255,0};
        RGB blue = {0,0,255};
        RGB white = {255,255,255};
        RGB gruvGray = {146, 131, 116};
        RGB gruvDarkGray = {40,40,40};
        RGB gruvCream = {235, 219, 178};
        RGB black = {0,0,0};

        RGB gruvBlue = {31, 165, 152};
        RGB gruvAqua = {142, 192, 124};
        RGB gruvPurple = {211, 134, 155};
        RGB gruvGreen = {184, 187, 38};
        RGB gruvOrange = {254, 128, 25};
        RGB gruvRed = {251, 73, 52};
        

        std::ostringstream buffer;
        std::string characterSet[3] = {"█", "●","▒"};
        
        int screenW;
        int screenH;

        bool firstMove = true;

        int numOfFlags = 0;
        
        std::string difficulty = "easy";
        std::string saveData[4][4];

        player pPos;
    public:
        board(int w, int h, int numOfMines);
        
        void initTable();
        tile getTile(int x, int y);
        void drawBoard();
        void printAt(std::string text, int x, int y, const RGB& colour, std::string backGround);
        void initScreen();
        void printBuffer();
        void playerMove();
        void flagTile();
        void selectTile();
        void dominoBreak(int x, int y);
        void gameover();
        void getScreenDims();
        void drawUi();
        void changeDifficulty(std::string diff);
        bool inCustMenu();
};

inline board::board(int w, int h, int numOfMines){
    this->w = w;
    this->h = h;
    this->numOfMines = numOfMines;

    std::fstream file(this->saveFile);
    std::string line;

    int diffNum = 0;
    int saveNum = 0;

    while(std::getline(file,line)){
        if(line[0] == '#'){
            diffNum++;
            saveNum = 0;
        }
        else{
            this->saveData[diffNum][saveNum] = line;
            saveNum ++;
        }
    }
}

/*
if need optimise, loop through to find mines and add 1 to surrounding tiles
*/
inline void board::initTable(){
    this->table.clear();
    int num = this->numOfMines;
    //creates the table
    for(int y = 0; y < this->h; y++){
        std::vector<tile> row;
        for(int x = 0; x < this->w; x++){
            tile t = {0};
            row.push_back(t);
        }
        this->table.push_back(row);
    }
    //Sets the mines
    while(num != 0){
        for(int y = 0; y < this->h; y++){
            for(int x = 0; x < this->w; x++){
                int ranNum = rand() % 10;
                //Add in miss mines that already exist
                if(ranNum == 1 && num != 0 && !this->table[y][x].isMine){
                    this->table[y][x].isMine = true;
                    num--;
                }
                if(num == 0){break;}
            }
            if(num == 0){break;}
        }
    }
    //Adds all the numbers
    for(int y = 0; y < this->h; y++){
        for(int x = 0; x < this->w; x++){
            if(this->table[y][x].isMine){continue;}
            else{
                int numOfMines = 0;

                int minX = -1;
                int minY = -1;
                int maxX = 2;
                int maxY = 2;
                
                if(y == 0){minY = 0;}
                if(y == this->h-1){maxY = 1;}
                if(x == 0){minX = 0;}
                if(x == this->w-1){maxX = 1;}

                for(int yMod = minY; yMod < maxY; yMod++){ for(int xMod = minX; xMod < maxX; xMod++){
                        if(this->table[y + yMod][x + xMod].isMine){
                            numOfMines++;
                        }
                    }
                }
                this->table[y][x].value = numOfMines;
            }
        }
    }
}

inline tile board::getTile(int x, int y){
    return(this->table[y][x]);
}

inline void board::drawBoard(){
    int xMod = (this->screenW / 2) - (this->w / 2);
    int yMod = (this->screenH / 2) - (this->h / 2);
    for(int y = 0; y < this->h; y++){
        for(int x = 0; x < this->w; x++){

            std::string background = "49m";
            RGB colour = gruvCream;

            if(x == pPos.x && y == pPos.y){
                if(this->table[y][x].isReveal || this->table[y][x].isFlag){
                    background = "43m";
                }
                else{
                    colour = gruvYellow;
                }
            }

            if(this->table[y][x].isReveal){
                //Different colours for numbers
                if(this->table[y][x].isMine){
                    colour = red;
                    printAt(characterSet[1], x + 1 + xMod, y + 1 + yMod , colour, background);
                }
                else{
                    switch(this->table[y][x].value){
                        case 0:
                            colour = gruvDarkGray;
                            break;
                        
                        case 1:
                            colour = gruvBlue;
                            break;

                        case 2:
                            colour = gruvGreen;
                            break;

                        case 3:
                            colour = gruvRed;
                            break;

                        case 4:
                            colour = gruvPurple;
                            break;

                        case 5:
                            colour = gruvOrange;
                            break;

                        case 6:
                            colour = gruvAqua;
                            break;
                    }
                    printAt(std::to_string(this->table[y][x].value), x + 1 + xMod, y + 1 + yMod , colour, background);
                }
            }
            else if(this->table[y][x].isFlag){
                colour = red;
                printAt(characterSet[2], x + 1 + xMod, y + 1 + yMod , colour, background);
            }
            else{
                printAt(characterSet[0], x + 1 + xMod, y + 1 + yMod , colour, background);
            }
        }
    }
}

inline void board::printAt(std::string text, int x, int y, const RGB& colour, std::string backGround){
    //Add the ability to print in colour
    std::string line =
        //location to print at
        "\x1b[" + std::to_string(y) + ";" + std::to_string(x) + "H" +
        //Colour definition
        "\x1b[38;2;" + std::to_string(colour.r) + ";"+ std::to_string(colour.g) + ";"+ std::to_string(colour.b) + "m" +
        //Background colour
        "\u001b["+ backGround + 
        //Actual text
        text + 
        //Text format
        "\x1b[0m";

    this->buffer << line;
}

inline void board::initScreen(){
    std::cout << "\033[?1049h";
    std::cout << "\033[H\033[2J";
    buffer << "\x1b[?25l";
    std::cout << buffer.str() << std::endl;
    
    //init the screen using ncurses
    initscr(); // Start curses mode
    cbreak();  // Line buffering disabled
    noecho();  // Don't echo input

    nodelay(stdscr, TRUE);
}

inline void board::printBuffer(){
    std::cout << buffer.str() << std::flush;
    buffer.str("");
    buffer.clear();
}

inline void board::flagTile(){
    if(!this->table[this->pPos.y][this->pPos.x].isReveal){
        if(this->table[this->pPos.y][this->pPos.x].isFlag){numOfFlags--;}
        else{numOfFlags++;}
        this->table[this->pPos.y][this->pPos.x].isFlag = !this->table[this->pPos.y][this->pPos.x].isFlag;
    }
}

inline void board::gameover(){
    this->pPos.x = 0;
    this->pPos.y = 0;
    this->initTable();
    this->firstMove = true;
    this->numOfFlags = 0;
    for(int y = 0; y < this->screenH; y++){
        for(int x = 0; x < this->screenW; x++){
            printAt(" ", x, y, white, "49m");
        }
    }
}

inline void board::dominoBreak(int x, int y){
    //Implement something for gameover
    if (this->table[y][x].isMine){this->gameover(); return;}
    if (this->table[y][x].isReveal) return;  
    if (this->table[y][x].value != 0){this->table[y][x].isReveal = true; return;}  

    this->table[y][x].isReveal = true;

    if (x > 0) this->dominoBreak(x - 1, y);
    if (y > 0) this->dominoBreak(x, y - 1);
    if (x < this->w - 1) this->dominoBreak(x + 1, y);
    if (y < this->h - 1) this->dominoBreak(x, y + 1);
    if(x > 0 && y > 0) this->dominoBreak(x-1,y-1);
    if(x > 0 && y < this->h - 1) this->dominoBreak(x-1,y+1);

    if(x < this->w - 1 && y > 0) this->dominoBreak(x+1,y-1);
    if(x < this->w - 1 && y < this->h - 1) this->dominoBreak(x+1,y+1);
}

inline void board::selectTile(){
    if(this->firstMove){
        this->firstMove = false;

        while(true){
            if(this->table[this->pPos.y][this->pPos.x].value == 0 && ! this->table[this->pPos.y][this->pPos.x].isMine){break;}
            this->initTable();
        }
    }
    if(! this->table[this->pPos.y][this->pPos.x].isFlag){
        this->dominoBreak(this->pPos.x, this->pPos.y);
    }
}

inline void board::playerMove(){
    int k = getch();

    char c = static_cast<char>(k);
    
    if(!this->custMenu){
        if(c == 'h' && this->pPos.x != 0){
            this->pPos.x --;
        }
        else if(c == 'l' && this->pPos.x != this->w -1){
            this->pPos.x ++;
        }
        else if(c == 'k' && this->pPos.y != 0){
            this->pPos.y --;
        }
        else if(c == 'j' && this->pPos.y != this->h - 1){
            this->pPos.y ++;
        }
        else if(c == 'a'){
            this->flagTile();
        }
        else if(c == 's'){
            this->selectTile();
        }
        else if(c == 'r'){
            this->gameover();
        }
        else if(c == 'q'){
            this->changeDifficulty(this->difficulty);
        }
    }
    else{
        if(c == 'l'){
            if(inMenuSelector == 0){
                this->custW ++;
            }
            else if(inMenuSelector == 1){
                this->custH ++;
            }
            else if(inMenuSelector == 2){
                this->custM ++;
            }
        }
        else if(c == 'h'){
            if(inMenuSelector == 0){
                this->custW --;
            }
            else if(inMenuSelector == 1){
                this->custH --;
            }
            else if(inMenuSelector == 2){
                this->custM --;
            }
        }
        else if(c == 'k'){
            this->inMenuSelector --;
            if(this->inMenuSelector == -1){this->inMenuSelector = 2;}
        }
        else if(c == 'j'){
            this->inMenuSelector ++;
            if(this->inMenuSelector == 3){this->inMenuSelector = 0;}
        }
    }

    if(this->difficulty == "cust"){
        if(c == 'e'){
            this->custMenu = !this->custMenu;
            if(this->custMenu == false){
                changeDifficulty("hard");
            }
        }
    }
}

inline void board::getScreenDims(){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    //std::cout << w.ws_row << " " << w.ws_col << std::endl;
    this->screenH = w.ws_row;
    this->screenW = w.ws_col;
}

inline void board::drawUi(){
    if(!this->custMenu){
        int xMod = (this->screenW / 2) + (this->w / 2) + 2;
        int yMod = (this->screenH / 2) - (this->h / 2) + 1;

        printAt("[", xMod, yMod, gruvCream, "49m");
        printAt(std::to_string(this->w), xMod + 1, yMod, gruvBlue, "49m");
        printAt(",", xMod + 1 + std::to_string(this->w).size(), yMod, gruvCream, "49m");
        printAt(std::to_string(this->h), xMod + 2 + std::to_string(this->w).size(), yMod, gruvBlue, "49m");
        printAt("]", xMod + 2 + std::to_string(this->w).size() + std::to_string(this->h).size(), yMod, gruvCream, "49m");
        yMod += 2;

        printAt(characterSet[1], xMod + 1, yMod, gruvRed, "49m");
        printAt(": " + std::to_string(this->numOfMines), xMod + 2, yMod, gruvCream, "49m");

        yMod += 1;

        printAt(characterSet[2], xMod + 1, yMod, gruvRed, "49m");
        printAt(": " + std::to_string(this->numOfFlags), xMod + 2, yMod, gruvCream, "49m");
        
        yMod += 1;

        int diffCol = 0;
        if(difficulty == "easy"){diffCol = 0;}
        else if(difficulty == "medi"){diffCol = 1;}
        else if(difficulty == "hard"){diffCol = 2;}
        else if(difficulty == "cust"){diffCol = 2;}

        printAt("P: ", xMod + 1, yMod + 1, gruvCream, "49m");
        printAt(this->saveData[diffCol][0], xMod + 2, yMod + 1, gruvBlue, "49m");

        printAt("W: ", xMod + 1, yMod + 2, gruvCream, "49m");
        printAt(this->saveData[diffCol][1], xMod + 2, yMod + 2, gruvGreen, "49m");

        printAt("L: ", xMod + 1, yMod + 3, gruvCream, "49m");
        printAt(this->saveData[diffCol][2], xMod + 2, yMod + 3, gruvGreen, "49m");
        //Difficulty side
        xMod = (this->screenW / 2) - (this->w / 2) - 6;
        yMod = (this->screenH / 2) - (this->h / 2) + 1;
        
        std::string backGround = "49m";
        printAt("Easy", xMod, yMod, gruvCream, (this->difficulty == "easy") ? "43m" : backGround);
        printAt("Medi", xMod, yMod+1, gruvCream, (this->difficulty == "medi") ? "43m" : backGround);
        printAt("Hard", xMod, yMod+2, gruvCream, (this->difficulty == "hard") ? "43m" : backGround);
        printAt("Cust", xMod, yMod+3, gruvCream, (this->difficulty == "cust") ? "43m" : backGround);
    }

    if(this->custMenu){

        int xMod = (this->screenW / 2) - 14;
        int yMod = (this->screenH / 2) - 8;

        for(int y = yMod; y < yMod + 16; y++){
            for(int x = xMod; x < xMod + 28; x++){
                if(y == yMod || y == yMod + 15){
                    printAt("=", x, y, gruvCream, "49m");
                }
                else{
                    printAt(" ", x, y, gruvCream, "49m");
                }
            }
        }

        if(this->custW > this->screenW - 20){this->custW = 4;}
        else if(this->custW < 4){this->custW = this->screenW - 20;}

        if(this->custH > this->screenH - 2){this->custH = 4;}
        else if(this->custH < 4){this->custH = this->screenH - 2;}

        if(this->custM > this->custH * this->custW){this->custM = 1;}
        else if(this->custM < 1){this->custM = this->custH * this->custW;}


        printAt("Width:", xMod + 2, yMod + 3, gruvCream, "49m");
        printAt(std::to_string(this->custW), xMod + 15, yMod + 3, gruvCream, (this->inMenuSelector == 0)? "43m" : "49m");

        printAt("Height:", xMod + 2, yMod + 5, gruvCream, "49m");
        printAt(std::to_string(this->custH), xMod + 15, yMod + 5, gruvCream, (this->inMenuSelector == 1)? "43m" : "49m");

        printAt("No. Mines:", xMod + 2, yMod + 7, gruvCream, "49m");
        printAt(std::to_string(this->custM), xMod + 15, yMod + 7, gruvCream, (this->inMenuSelector == 2)? "43m" : "49m");
    }

}

inline void board::changeDifficulty(std::string diff){
    if(diff == "easy"){
        this->w = 16;
        this->h = 16;
        this->numOfMines = 40;
        this->difficulty = "medi";
    }
    else if(diff == "medi"){
        this->w = 30;
        this->h = 16;
        this->numOfMines = 99;
        this->difficulty = "hard";
    }
    else if(diff == "hard"){
        this->w = custW;
        this->h = custH;
        this->numOfMines = custM;
        this->difficulty = "cust";
    }
    else if(diff == "cust"){
        this->w = 9;
        this->h = 9;
        this->numOfMines = 10;
        this->difficulty = "easy";
    }
    this->gameover();
}

inline bool board::inCustMenu(){
    return(this->custMenu);
}
#endif
