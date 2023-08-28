// Author: Zoraiz Khan
// UCID: 30103088
// Date: 20-02-2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// shared structure
struct shared {
  char *board;
  int turn;
};

struct shared s;

// initializing the board
void boardInit() {
  s.board = "_________";
}

// displaying the board
void display()
{
  printf(" _ _ _\n");
  for (int i = 0; i <= 6; i = i + 3){
    printf("|%c|%c|%c|\n", s.board[0+i], s.board[1+i], s.board[2+i]);
  }
  printf("\n");
}

// game evaluation
int gameEval() {
    // (case 1) There is a horizontal match return 1
    for (int i = 0; i <= 6; i = i + 3) {    
        if (s.board[0+i] == s.board[1+i] && s.board[0+i] == s.board[2+i] && s.board[0+i] != '_'){
            return (1);
        } 
    }

    // (case 2) There is a vertical match return 1
    for (int i = 0; i <= 2; i++) {    
        if (s.board[0+i] == s.board[3+i] && s.board[0+i] == s.board[6+i] && s.board[0+i] != '_'){
            return (1);
        } 
    }

    // (case 3) There is a diagonal match return 1
    if (((s.board[0] == s.board[4] && s.board[0] == s.board[8]) || 
        (s.board[2] == s.board[4] && s.board[2] == s.board[6])) && s.board[4] != '_' ) {
        return (1);
    }

    for (int i = 0; i < 9; i++){
        if (s.board[i] == '_') {
             // game has not reached a conclusion so return 0
             return (0);
        }
    } 

    // Otherwise if the game has drawn return 0
    return (2);
}

int makeMove() {
  char board[10];
  char mark;

  // Select a random location to place the mark
  srand( time(0));
  int pos = rand() % 10;
  
  // Select mark based on the turn 
  if (s.turn == 1) {
    mark = 'X';
  } else {
    mark = 'O';
  }

  // Set a new mark
  strcpy(board, s.board);
  if (s.board[pos] == '_') {
    board[pos] = mark;
    s.board = &board[0];
    return (1);
  }
  return (0);
}

void *playGame(void *arg) {
  while (1) {
    int *t = (int *)arg;

    // Waiting for turn
    while (s.turn != *t);
    
    // Place a mark on an unmarked area 
    while (makeMove() == 0);

    printf("Player %d made the following move: \n", *t);

    // turn over the control to main thread
    s.turn = 0;
  }
}



int main() {
  // Game state
  int game_on = 1;

  // Last player to make a turn
  int curr_turn = 1;
  
  // Type of evaluation
  int eval;

  // initializing game
  boardInit();
  display();
  s.turn = 1;

  int player1 = 1;
  int player2 = 2;

  // creating threads
  pthread_t tid1, tid2;
  pthread_create(&tid1, NULL, playGame, &player1);
  pthread_create(&tid2, NULL, playGame, &player2);

  while (game_on == 1) {
    
    // Waiting for main thread's turn
    while (s.turn != 0);

    display();

    // if game has reached an evaluation
    eval = gameEval();
    if (eval > 0) {
      
      if (eval == 1) {
        printf("Player %d wins!\n", curr_turn);
      } else {
        printf("Game drawn!\n", curr_turn);
      }
      //end game
      pthread_cancel(tid1);
      pthread_cancel(tid2);
      game_on = 0;
    
    } else {

      // Switch turns b/w players.
      if (curr_turn == 1) {
        curr_turn = 2;
      } else {
        curr_turn = 1;
      }
      s.turn = curr_turn;
    
    }

  }
    
}




  
