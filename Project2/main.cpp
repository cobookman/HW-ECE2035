/**********************************
 * Author: Clifton Thomas
 * AI and 7 Seg Hex Modifications: Colin Bookman
 * Date: 3/28/13
 * Institution: Georgia Tech
 *
 * Title: MAIN
 * Class: ECE2035
 * Assignment: Project 2
 **********************************/

//includes
#include <string.h>
#include <cstdarg>
#include "mbed.h"
#include "rtos.h"
#include "TextLCD.h"
#include "SDFileSystem.h"
#include "CommModule.h"
#include "wave_player.h"
#include "FATFileSystem.h"


//defines
#define BAUDRATE            9600
#define BUFFSIZE            100
#define BOMB_WAVFILE        "/sd/wavfiles/bomb.wav"
#define ACK                 "status-ack"
#define STATUS_KEY          "status"
#define HIT_KEY             "hit"
#define MISS_KEY            "miss"
#define SANK_KEY            "sank"
#define HIT                    1
#define MISS                   0
#define SANK                   2
#define DEAD                  -1
#define ERROR                 -1
#define SUNK                   2

//Globals
int aliveSize[] = {2, 3, 3, 4, 5}; //Lists the ship sizes.  If ship no longer exists, size = -1
int sunkenShips  = 0;                   //# of sunken ships
int board[10][10];
int prob[10][10] = {0};
int numBombs = 0;
int max_row, max_col; //Max prob location, if -1 issue

//function prototypes
void lowerCase(char *src);
void startGame(void);
void waitForAck(void);
int bomb(int rowIndex, int colIndex);
void playBombSound(void);
void print(const char *format, ...);
int hunt(int row, int col);
//AI Prototype
void probality();    //Caclulates board probability
void chooseNext();  //places best move in max_row and max_col, if -1 returned than bad!
void sinkIt(int row, int col); //SInks the ship after 1 hit
int placeBomb(int row, int col); //Bomb wrapper, returns -1 for error


//declare functions (assembly subroutines)
extern "C" void setup_sequence(void);
extern "C" void seg_driver_initialize(void);
extern "C" void seg_driver(int value);


//initialize hardware
SDFileSystem sd(p5, p6, p7, p8, "sd"); // mosi, miso, sck, cs
TextLCD lcd(p26, p25, p24, p23, p22, p21); // rs, e, d4-d7
AnalogOut DACout(p18);
wave_player waver(&DACout);


//communication device
commSerial serDevice(USBTX, USBRX, BAUDRATE); //tx, rx


int main() {
    //initialize and clear 7-Segment Display (assembly subroutine)
    setup_sequence();
    seg_driver_initialize();
    seg_driver(0);
    
    //check for wav file
    lcd.cls();
    lcd.printf("Locating WAV file...");
    FILE *test_file;
    while(1) {
        test_file=fopen(BOMB_WAVFILE,"r");
        if(test_file != NULL) {
            lcd.printf("File not found");
            break;
        }
        wait(0.5);
    }
    fclose(test_file);
    lcd.cls();

    //notification
    lcd.cls();
    lcd.printf("Battleship");
    wait(1);
       
    while(1) {
        //synchronize front end display
        startGame();
        
        print("Lets play!");
       //Re-Init board
        for(int i = 0; i<10; i++) {
            for(int j = 0; j<10; j++) {
                board[i][j] = -2;
            }
        }   
        //Re-Init gloabls
        int shipsizes[] = {2, 3, 3, 4, 5};
        aliveSize[0] = 2;
        aliveSize[1] = 3;
        aliveSize[2] = 3;
        aliveSize[3] = 4;
        aliveSize[4] = 5;
        sunkenShips  = 0;                   //# of sunken ships
        numBombs = 0;
        
        int shotResult = -1;
        while(sunkenShips<5) {
            //Calculate board probability
            probality();
            //bomb best move
            chooseNext();
            //print("Hitting  Row: %d, Col: %d", max_row, max_col);
            shotResult = placeBomb(max_row, max_col);
            //print("Result: %d", shotResult);
            //Got a hit, sink the bastard
            if(shotResult == HIT) {
                sinkIt(max_row, max_col);
            }
        }//Finished game!
        lcd.cls();
        lcd.printf("GAME OVER!");
    }
}
//=======START HELPER FUNCTIONS =====
void probality() {
    //Init prob board to 0
    memset(prob, 0, sizeof(prob));
    //Start @ 0,0
    int row = 0;
    int col = 0;
    int downTile, rightTile;
    int downSpace, rightSpace;
    //Iterate through each and every tile
    while((col!=10) && (row!=10)) {
        downSpace = 0;
        rightSpace = 0;
        /************* Get Down Space ***********/
        downTile = row; //start looking @ current tile
        while(downTile<10 && board[downTile][col]<0) { //Loop until we get to a Miss/Hit, or end of board
            downSpace++;
            downTile++;
        }
        /************* Check if ships can fit in down space *******/
        for(int ship = 0; ship<5; ship++) {
            if(aliveSize[ship] != DEAD) {
                //Check if we can place ship downards, if so Add 1 to it's prob score
                if(downSpace >= aliveSize[ship]) { 
                    for(int i=row; i<(row+aliveSize[ship]); i++) {
                        prob[i][col]++;
                    }
                }
            }
        }
                
        /************* Get Right Space ***********/
        rightTile = col;
        while(rightTile<10 && board[row][rightTile]<0) { //Loop till end of board or Hit/Miss
            rightSpace++;
            rightTile++;
        } 
        /************* CHECK if ship can fit in right space *******/
        for(int ship = 0; ship<5; ship++) {
            if(aliveSize[ship] != DEAD) {
                //add 1 to prob score if ship fits right
                if(rightSpace >= aliveSize[ship]) {
                    for(int i=col; i<(col+aliveSize[ship]); i++) {
                        prob[row][i]++;
                    }
                }
            }
        }

        /***** NEXT POS *****/
        row++;
        if(row==10) { 
            //End of column...
            row = 0;
            col++;
        }
    }//END TILE LOOP
}//END probality()

void chooseNext() {
    max_row = -1;
    max_col = -1;
    int max_amount = ERROR; //Garenteed to return a tile
    //Find max prob location and amount
    for(int row = 0; row<10; row++) {
        for(int col = 0; col<10; col++ ) {
            if(prob[row][col]>max_amount) {
                max_amount = prob[row][col];
                max_row = row;
                max_col = col;
            }
        }
    }
}

void sinkIt(int row, int col) {
    int curCol, curRow;
    //get Left Size
        curCol = col;
        int leftSize = 0;
        while(curCol >= 0 && board[row][curCol] != 0) { //While no miss to left
            leftSize = leftSize+1;
            curCol--; //Move left
        }
    //Get Right Size
        curCol = col;
        int rightSize = 0;
        while(curCol < 10 && board[row][curCol] != 0 ) {
            rightSize = rightSize+1;
            curCol++; //move right
        }
    //Get Up Size
        curRow = row;
        int upSize = 0;
        while(curRow>= 0 && board[curRow][col] != 0 ) {
            upSize = upSize + 1;
            curRow--; //move down
        }
    //Get Down Size
        curRow = row;
        int downSize = 0;
        while(curRow < 10 && board[curRow][col] != 0 ) {
            curRow++;
            downSize++; //Move up
        }
        
    //Find if we can even have a ship to our left/right/up/down
        int left = 0; 
        int right = 0; 
        int up = 0; 
        int down = 0; 
        for(int ship = 0; ship<5; ship++) {
            if(aliveSize[ship] > 0) {   //Disregard any sunken ships or ships of size of <1
                if(aliveSize[ship]<=leftSize) {
                    left = 1;
                }
                if(aliveSize[ship]<=rightSize) {
                    right = 1;
                }
                if(aliveSize[ship]<=upSize) {
                    up = 1;
                }
                if(aliveSize[ship]<=downSize) {
                    down = 1;
                }
            }
        }
    //Try and sink the ship by hitting where it's possible to have the ship
    int bombRes = -1; //Result of hit here
    int boatLength = 1; //Already have 1 hit
    /******Bomb LEFT*****/
    curCol = col-1;
    //Place miss to our left if boat can't fit in left
    if(left == 0 && (col-1)>=0) {
        board[row][col-1] = MISS;
    }
    while(left == 1 && bombRes!=SUNK && curCol >= 0) {
        bombRes = placeBomb(row, curCol);
        if(bombRes == MISS) {
            left = 0;
            //Place miss
            board[row][curCol] = MISS;

        } else if(bombRes == HIT || bombRes == SUNK) {
            //Place hit where we just 'hit'
            board[row][curCol] = HIT;           
            boatLength++; //Increase the boat length

            //Place miss above
            if((row+1) < 10) {
                board[row+1][curCol] = MISS; //Curr
                board[row+1][curCol+1] = MISS; //Past
            }
            //Place misses below
            if((row-1) >= 0) {
                board[row-1][curCol] = MISS; //Curr
                board[row-1][curCol+1] = MISS; //Past
            }

        }
        if(bombRes == SUNK) {
            /*Place Misses in front our first cell, we only moved leftwards */
            if((col+1) < 10) { board[row][col+1]   = MISS; }

            /* Place misses behind our the cell that sank the sucker */
            if((curCol-1)>=0) { board[row][curCol-1]   = MISS; }
        }
        curCol--; //Go left
    }
        

    /******Bomb RIGHT*****/
    curCol = col+1;
    //Place miss to our right, if no boat can fit to right
    if(right == 0 && (col+1)<10) {
        board[row][col+1] = MISS;
    }
    while(right == 1 && bombRes!=SUNK && curCol<10) {
        bombRes = placeBomb(row,curCol);
        if(bombRes == MISS) {
            right = 0;
            //Place misses to our right, nothing there
            board[row][curCol]   = MISS;

        } else if(bombRes == HIT || bombRes == SUNK) {
            //Place hit where we just 'hit'
            board[row][curCol] = HIT;   
            boatLength++;       
            //Place miss below
            if((row-1)>=0) {
                board[row-1][curCol] = MISS; //Curr
                board[row-1][curCol-1] = MISS; //Past
            }
            //Place miss above
            if((row+1)<10) {
                board[row+1][curCol] = MISS; //Curr
                board[row+1][curCol-1] = MISS; //Past
            }
            
        }
        //Place miss to right as boat sunk
        if((bombRes == SUNK) && (curCol+1)<10) {
                board[row][curCol+1] = MISS;
        }
        curCol++;
    }
        
    /****Bomb Down****/
    curRow = row+1;
    //Place miss below if no ship fits downards
    if(down == 0 && (row+1)<10) {
        board[row+1][col] = MISS;
    }
    while(down == 1 && bombRes!=SUNK && curRow<10) {
        bombRes = placeBomb(curRow, col);
        if(bombRes == MISS) {
            down = 0;
            board[curRow][col] = MISS;
        } else if(bombRes == HIT || bombRes == SUNK) {
            //Place hit where we just 'hit'
            board[curRow][col] = HIT;   
            boatLength++;       
            //Place miss to left
            if((col-1)>=0) {
                board[curRow][col-1] = MISS; //current
                board[curRow-1][col-1] = MISS;  //past
            }
            //Place miss to right
            if((col+1)<10) {
                board[curRow][col+1] = MISS;  //Curr
                board[curRow-1][col+1] = MISS;  //Past
            }
            
        }
        //Place miss down as boat sunk
        if((bombRes == SUNK) && (curRow+1)<10) {
                board[row+1][curCol] = MISS;
        }
        curRow++; //Move Down
    }

    //BOMB UP
    curRow = row-1;
    //Place miss above if no ship fits upwards
    if(up == 0 && (row-1)>=0) {
        board[row-1][col] = MISS;
    }
    while(up == 1 && bombRes!=SUNK && curRow>=0) {
        bombRes = placeBomb(curRow, col);
        if(bombRes == MISS) {
            up = 0;
            board[curRow][col] = MISS;
        } else if(bombRes == HIT || bombRes == SUNK) {
            //Place hit where we just hit
            board[curRow][col] = HIT;
            boatLength++;
            //Place miss to left
            if((col-1)>=0) {
                board[curRow][col-1] = MISS; //Curr
                board[curRow+1][col-1] = MISS; //Past
            }
            //Place miss to right
            if((col+1)<10) {
                board[curRow][col+1] = MISS; //Curr
                board[curRow+1][col+1] = MISS; //Past
            }
        }
        //Place miss down as boat sunk
        if((bombRes == SUNK) && (curRow+1)<10) {
            board[row+1][curCol] = MISS;
        }
        curRow--; //MOVE UP
    }
    /**** Boat should be sunken ****/
    if(bombRes!=SUNK) { 
        print("IT MESSED UP TRYING TO SINK SHIP"); 
    } else if(bombRes == SUNK) {
        sunkenShips++;
        //Update hex display w/sunken ship count
        seg_driver(sunkenShips);
        bool removedShip = false;
        int ship = 0;
        //REMOVE SHIP
        while(!removedShip && ship<5) {
            if(aliveSize[ship] == boatLength) {
                aliveSize[ship] = -1;
                removedShip = true;
            }
            ship++;
        }
    }//END SUNK IF
}//END Function
int placeBomb(int row, int col) {
    int ret = -1;
    //Ok row, col #s....nvr bombed area before
    if((row<10 && row >=0) && (col<10 && col >=0) && board[row][col]<0) {
            ret = bomb(row, col);
            board[row][col] = ret;
            lcd.cls();
            numBombs++;
            lcd.printf("B(%d, %d) = %d\n%d bombs used",row, col, ret, numBombs);
            print("Bombing: R %d  C %d - %d bombs used", row, col, numBombs);
    }
    return ret;
}

//=======END MY FUNCTIONS=======

char * outMessages[] = {"Error", "Miss", "Hit", "Sank"};

//fcn to get acknowledgement from serial peripheral
void waitForAck(void) {
    //get acknowlegement
    char buffer[BUFFSIZE];
    while(1) {
        serDevice.receiveData(buffer);
        lowerCase(buffer);
        if(strcmp(ACK, buffer) == 0) {
            break;
        }
        memset(&buffer[0],0,strlen(buffer));     
    }
    return;
}

//fcn to initialize the frontend display
void startGame(void) {
    //temp variables
    char buffer[BUFFSIZE];
    
    //construct message
    sprintf(buffer, "start");
    
    //send message
    serDevice.sendData(buffer);
    
    //wait for acknowledgement
    waitForAck();
}

//fcn to bomb a given coordinate
int bomb(int rowIndex, int colIndex) {
    //temp variables
    char buffer[BUFFSIZE];
    
    //check if coordinate is valid
    if((rowIndex >= 0) && (rowIndex <= 9) && (colIndex >= 0) && (colIndex <= 9)) {
        //construct message
        sprintf(buffer, "bomb-%d-%d", rowIndex, colIndex);
        
        //send message
        serDevice.sendData(buffer);
        
        //wait for status response
        while(1) {
            //temp variables
            memset(&buffer[0],0,strlen(buffer));
            char *ptr = NULL;
            
            //receive status response
            serDevice.receiveData(buffer);
            
            //parse string to extract status key
            ptr = strstr(buffer, STATUS_KEY);
            if(ptr == NULL) {continue;}
            
            //if status key found, parse string to extract status message
            ptr+=(strlen(STATUS_KEY)+1);
            if(strcmp(ptr, HIT_KEY) == 0) {
                playBombSound();
                return(1);
            }
            else if(strcmp(ptr, MISS_KEY) == 0) {
                return(0);
            }
            else if(strcmp(ptr, SANK_KEY) == 0) {
                playBombSound();
                return(2);             
            }
            else {
                return(-1);
            }
        }            
    }
    return(-1);
}

//fcn to play bomb noise
void playBombSound(void) {
    //open wav file
    FILE *wave_file;
    wave_file=fopen(BOMB_WAVFILE,"r");
    
    //play wav file
    waver.play(wave_file);
    
    //close wav file
    fclose(wave_file);
}

//fcn to print to console
void print(const char *format, ...) {
    //temp variables
    char buffer[BUFFSIZE];
    char temp[BUFFSIZE-6];
    
    //construct message part 1
    sprintf(buffer, "print-");
    
    //construct message part 2
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(temp, BUFFSIZE-7, format, arguments);
    va_end(arguments);
    
    //concatenate parts
    strcat(buffer, temp);
    
    //send message
    serDevice.sendData(buffer);
    
    //wait for acknowledgement
    waitForAck();  
}

//fcn to convert string to lowercase
void lowerCase(char *src) {
    int i=0;;
    while(src[i] != '\0') {
        if((src[i] > 64) && (src[i] < 91)) {
            src[i]+=32;
        }
        i++;
    }
    return;
}
