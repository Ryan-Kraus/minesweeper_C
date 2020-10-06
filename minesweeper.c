#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "minesweeperStruct.h"

//Constants
#define MAX_DIFFICULTY_INPUT 8 //max size of input when user selects difficulty.
#define MAX_RAW_INPUT 15 //max size of input user can type

//boardSideSize and numberOfMines constants for easy, normal, and hard game difficulties
#define EASY_SIZE 10
#define EASY_MINES 7
#define NORMAL_SIZE 20
#define NORMAL_MINES 20
#define HARD_SIZE 30
#define HARD_MINES 40

//Global Variables
int boardSideSize;
int numberOfMines;
bool gameOver;

//Function Prototypes
void parseCmd(int argc, char*argv[]);
void flush_stdin(char input[MAX_DIFFICULTY_INPUT]);
void getDifficulty(void);
void init(tile board[boardSideSize][boardSideSize]);
void initField(tile board[boardSideSize][boardSideSize]);
void placeMines(tile board[boardSideSize][boardSideSize]);
void placeNumbers(tile board[boardSideSize][boardSideSize]);
int countMines(tile board[boardSideSize][boardSideSize], int x, int y);
void gameLoop(tile board[boardSideSize][boardSideSize]);
void printBoard(tile board[boardSideSize][boardSideSize]);
void turn(tile board[boardSideSize][boardSideSize]);
void getPlayerMove(turnInput* playerInput);
void checkGameOver(tile board[boardSideSize][boardSideSize], turnInput playerInput);
void gameOverCondition(tile board[boardSideSize][boardSideSize]);
void makeMinesVisible(tile board[boardSideSize][boardSideSize]);
void updateGameBoard(tile board[boardSideSize][boardSideSize], turnInput playerInput);
void clearBlanks(tile board[boardSideSize][boardSideSize], int y, int x);
void checkGameWin(tile board[boardSideSize][boardSideSize]);
void winCondition(tile board[boardSideSize][boardSideSize]);
            
/* Function Tree

parseCmd
    getDifficulty
        flush_stdin
init
    initField
    placeMines
    placeNumbers
        countMines
gameLoop
    printBoard
    turn
        getPlayerMove
            flush_stdin
        checkGameOver
            gameOverCondition
                makeMinesVisible
                printBoard
        updateGameBoard
            clearBlanks
                clearBlanks(recursive)
        checkGameWin
            winCondition
                printBoard
*/

//________________________________________________________//
//---------------------MAIN FUNCTION----------------------//
//________________________________________________________//

int main(int argc, char *argv[]) {
    parseCmd(argc, argv);
    tile board[boardSideSize][boardSideSize];// Initialize 2D array containing type tile.
		init(board);
		gameLoop(board);
		return 0;
}

//-----------------------FUNCTIONS-----------------------//

//MAN PAGE IMPLEMENTATION
//Parse command line arguments from user.
void parseCmd(int argc, char*argv[]) {
    //Default values if no command line arguments are supplied.
    if(argc == 1) {
        getDifficulty();
    }
    else if(argc == 2 && (strcmp(argv[1], "man") == 0 || strcmp(argv[1], "help") == 0)) {
        printf("\n-----Manual for Minesweeper-----\n\n");
        printf("Try to clear the mine field without stepping on a mine!\n");
        printf("To clear a tile, enter <row> <column>\n");
        printf("Use flags to keep track of where you think mines are using f <row> <column>\n");
        printf("If you want to remove a flag, just type in f <row> <column> again.\n");
        printf("You cannot select a tile if it is flagged, if it has already been cleared, or if it does not exist.\n\n");
        printf("Difficulty levels:\n");
        printf("easy: %i x %i board, %i mines\n", EASY_SIZE, EASY_SIZE, EASY_MINES);
        printf("normal: %i x %i board, %i mines\n", NORMAL_SIZE, NORMAL_SIZE, NORMAL_MINES);
        printf("hard: %i x %i board, %i mines\n", HARD_SIZE, HARD_SIZE, HARD_MINES);
        printf("custom\n\n");
        printf("--------------------------------\n\n");
        exit(2);
    }
    else {
        printf("Please enter a valid input. \nTry running './mineSweeper.out man' for help.\n");
        exit(1);
    }
}

//flush the input buffer so extra characters will not feed into the next input.
void flush_stdin(char input[MAX_DIFFICULTY_INPUT]) {
    int i = strlen(input)-1;
    if(input[i] != '\n') {
        while(getchar() != '\n');
    }
}

//Get difficulty settings from user.
void getDifficulty(void) {
      bool inputValid = 0;
      char rawInput[MAX_DIFFICULTY_INPUT];
      while(!inputValid) {
          printf("Please select a difficulty (easy, normal, hard):");
          if(fgets(rawInput, MAX_DIFFICULTY_INPUT, stdin)) {
              flush_stdin(rawInput);
              if(strcmp(rawInput, "easy\n") == 0) {
                  boardSideSize = EASY_SIZE;
                  numberOfMines = EASY_MINES;
                  inputValid = 1;
              }
              else if(strcmp(rawInput, "normal\n") == 0) {
                  boardSideSize = NORMAL_SIZE;
                  numberOfMines = NORMAL_MINES;
                  inputValid = 1;
              }
              else if(strcmp(rawInput, "hard\n") == 0) {
                  boardSideSize = HARD_SIZE;
                  numberOfMines = HARD_MINES;
                  inputValid = 1;
              }
              else {
                  printf("Please enter a valid difficulty.\n");
              }
          }
      }    
}

//-----------Initialize Game----------//

void init(tile board[boardSideSize][boardSideSize]) {
		gameOver = false;
		srand(time(0)); //seeds the rand fn so every game is different.
		initField(board);
		placeMines(board);
		placeNumbers(board);
}

//Set all gameBoard tiles to not visible, not a mine, and not flagged in preparation for mine and number placement, and game turns.
void initField(tile board[boardSideSize][boardSideSize]) {
	  for(int i = 0; i < boardSideSize; i++) {
        for(int j = 0; j < boardSideSize; j++) {
				    board[i][j].isVisible = false;
						board[i][j].isMine = false;
            board[i][j].flag = false;
				}
		}
}

//Populate mineField with mines at random locations until numberOfMines has been reached.
void placeMines(tile board[boardSideSize][boardSideSize]) {
    int i = 0;
    while(i < numberOfMines) {
	      int x = rand() % boardSideSize;
				int y = rand() % boardSideSize;
				if(board[y][x].isMine == false) {
					  board[y][x].isMine = true;
					  i++;
				}
		}
}

//place numbers in all tiles that have a mine around them, equal to the number of mines around them.
void placeNumbers(tile board[boardSideSize][boardSideSize]) {
	  for(int i = 0; i < boardSideSize; i++) {
			  for(int j = 0; j < boardSideSize; j++) {
						if(board[i][j].isMine == true) {
							  ;
						}
						else {
						    board[i][j].numMinesAround = countMines(board, j, i);
						}
				}
		}
}

//Count how many mines are around a tile.
int countMines(tile board[boardSideSize][boardSideSize], int x, int y) {
	  int count = 0;
		for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
			      if(board[y-i+1][x-j+1].isMine == true && y-i+1 >= 0 && y-i+1 < boardSideSize && x-j+1 >= 0 && x-j+1 < boardSideSize) { //if on gameboard and is a mine
						    count++;
						}
	      }
		}
		return count;		
}

//--------------Game Loop-------------//
void gameLoop(tile board[boardSideSize][boardSideSize]) {
		while(!gameOver) {
			  printBoard(board);
				turn(board);
		}
}

//print the current state of the game board.
void printBoard(tile board[boardSideSize][boardSideSize]) {
	  //print columns
		printf("    ");
	  for(int i = 1; i <= boardSideSize; i++) {
        if(i <= 9) {
			      printf("%i  ", i);
        }
        else if(i > 9) {
            printf("%i ", i);
        }
		}
		printf("\n");
		//print board
    for(int i = 0; i < boardSideSize; i++) {
			  if(i < 9){
				    printf("%i   ", i + 1);
				}
				else if(i >= 9 && i < 99) {
					  printf("%i  ", i + 1);
				}
		    for(int j = 0; j < boardSideSize; j++) {
            if(board[i][j].flag == true) {
                printf("\033[1;33m");
                printf("!  ");
                printf("\033[0m");
            }
					  else if(board[i][j].isVisible == false) {
							  printf("-  ");
						}
						else if(board[i][j].isMine == true) {
                printf("\033[1;31m");
                printf("*  ");
                printf("\033[0m");
						}
						else if(board[i][j].numMinesAround > 0) {
				        printf("%i  ", board[i][j].numMinesAround);
						}
						else {
							  printf("   ");
						}
				}
				printf("\n");
		}
}

//player turn
void turn(tile board[boardSideSize][boardSideSize]){
	  turnInput playerInput;
    playerInput.flag = false; //initialize flag to false every turn when playerInput is created

		//get user input
		getPlayerMove(&playerInput);
    if(board[playerInput.row][playerInput.col].isVisible) {
        printf("\033[1;33m");
        printf("!This tile is already clear!\n\n");
        printf("\033[0m");
    }
    else if(playerInput.flag) {
        board[playerInput.row][playerInput.col].flag = !board[playerInput.row][playerInput.col].flag;
    }
    else if(board[playerInput.row][playerInput.col].flag) {
        printf("\033[1;33m");
        printf("!This tile is flagged!\n\n");
        printf("\033[0m");
    }
    else if(!playerInput.flag) {
        checkGameOver(board, playerInput);
        updateGameBoard(board, playerInput);
        checkGameWin(board);
    }
}

//get player input for their turn
void getPlayerMove(turnInput* playerMove) {
    bool inputFail = true;
    printf("Please enter flag row col: ");
    
    while(inputFail) {
        int tokcount = 0;
        int invalTok = 0;
        //Get user input and parse into tokens
        char rawInput[MAX_RAW_INPUT];
        char *token, *tokptr;
        if(fgets(rawInput, MAX_RAW_INPUT, stdin)) {
            flush_stdin(rawInput);
            for(token = strtok_r(rawInput, " ,", &tokptr); token != NULL; token = strtok_r(NULL, " ,", &tokptr)) {
                tokcount++;
                if(tokcount == 1 && strlen(token) == 1 && *token == 'f') {
                    playerMove->flag = true;
                }
                else if(tokcount == 1 && (int) strtol(token, (char**)NULL, 10) > 0 && (int) strtol(token, (char**)NULL, 10) <= boardSideSize) {
                    playerMove->row = (int) strtol(token, (char**)NULL, 10) - 1;
                }
                else if(tokcount == 2 && playerMove->flag == false && (int) strtol(token, (char**)NULL, 10) > 0 && (int) strtol(token, (char**)NULL, 10) <= boardSideSize) {
                    playerMove->col = (int) strtol(token, (char**)NULL, 10) - 1;
                    inputFail = false;
                }
                else if(tokcount == 2 && playerMove->flag == true && (int) strtol(token, (char**)NULL, 10) > 0 && (int) strtol(token, (char**)NULL, 10) <= boardSideSize) {
                    playerMove->row = (int) strtol(token, (char**)NULL, 10) - 1;
                }
                else if(tokcount == 3 && playerMove->flag == true && (int) strtol(token, (char**)NULL, 10) > 0 && (int) strtol(token, (char**)NULL, 10) <= boardSideSize) {
                    playerMove->col = (int) strtol(token, (char**)NULL, 10) - 1;
                    inputFail = false;
                }
                else {
                    invalTok++;
                }
            }
            //flush_stdin(rawInput); //flush stdin buffer so if user entered more than MAX_RAW_INPUT, it wont overflow into the next input
            if(invalTok > 0) {
                printf("Please enter a valid input: ");
            }
        }
    }
    printf("\n\n\n\n\n\n\n\n\n\n");
}

//Check if turn input will lead to a game over situation.
void checkGameOver(tile board[boardSideSize][boardSideSize], turnInput playerInput) {
	  if(board[playerInput.row][playerInput.col].isMine == true) {
        gameOverCondition(board);
		}
}

//When user loses, make all mines visible and print the board alerting user of the loss.
void gameOverCondition(tile board[boardSideSize][boardSideSize]) {
    makeMinesVisible(board);
    printBoard(board);
    printf("GAME OVER\n");
    gameOver = true;
}

//Set all and only tiles that have a mine to visible.
void makeMinesVisible(tile board[boardSideSize][boardSideSize]) {
    int i, j;
    for(i = 0; i < boardSideSize; i++) {
        for(j = 0; j < boardSideSize; j++) {
            if(board[i][j].isMine == true) {
                board[i][j].isVisible = true;
            }
        }
    }
}

//If user did not select a mine, clear the tile user selected, and clear any contiguous tiles if the selected tile is not next to a mine.
void updateGameBoard(tile board[boardSideSize][boardSideSize], turnInput playerInput) {
		board[playerInput.row][playerInput.col].isVisible = true;
		clearBlanks(board, playerInput.row, playerInput.col);
}

//If selected tile does not touch a mine, clear all tiles surrounding it. Iterate through all surrounding tiles and clear them if they do not touch a mine recursively.
void clearBlanks(tile board[boardSideSize][boardSideSize], int y, int x) {
    if(board[y][x].numMinesAround == 0) {
		    for(int i = 0; i < 3; i++) {
				    for(int j = 0; j < 3; j++) {
                if(i == 1 && j == 1) {
                    ;
                }
								else if(board[y-i+1][x-j+1].isVisible == false && y-i+1 >= 0 && y-i+1 < boardSideSize && x-j+1 >= 0 && x-j+1 < boardSideSize) { //check if tile is visible and if tile is within board constraints.
                    board[y-i+1][x-j+1].isVisible = true;
                    clearBlanks(board, y-i+1, x-j+1);
                }
				    }
		    }
		}
}

//Check to see if there are any tiles are not visible and not a mine left.
void checkGameWin(tile board[boardSideSize][boardSideSize]) {
    int i, j, count = 0;
    for(i = 0; i < boardSideSize; i++) {
        for(j = 0; j < boardSideSize; j++) {
            if(board[i][j].isVisible == false) {
                count++;
            }
        }
    }
    if(count == numberOfMines) {
        winCondition(board);
    }
}

//When user wins, print the board and alert them of the win.
void winCondition(tile board[boardSideSize][boardSideSize]) {
    printBoard(board);
    printf("YOU WIN\n");
    gameOver = true;
}
