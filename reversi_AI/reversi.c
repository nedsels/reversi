#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEPTH 6

void printBoard(char board[][26], int n);
void nextTurn(char *);
bool positionInBounds(int n, int row, int col);
bool checkLegal(char board[][26], int n, int row, int col, char colour);
bool checkLegalInDirection(char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol);
int letterToNumber(char);
char numberToLetter(int);
void updateBoard(char board[][26], int n, int row, int col, char colour);
bool moveAvailable(char[][26], int, char);
int calculateScore(char[][26], int, char);
int colourWon(char board[][26], int n, char colour);
int makeMove(char board[][26], int n, char turn, int *row, int *col);
double playerScore(char board[][26], int n, char computerColour,
                   char playerColour, int iteration, double alpha, double beta,
                   int progress);
double computerScore(char board[][26], int n, int *row, int *col,
                     char computerColour, char playerColour, int iteration,
                     double alpha, double beta, int progress);
double calculateRelativeValue(char board[][26], int n, char colour,
                              int progress);
void copyBoard(char boardTo[][26], char boardFrom[][26], int n);
int discsPlayed(char board[][26], int n);

int main() {
  int n = 0;
  char computerColour = 0;
  char playerColour = 0;

  printf("Enter the board dimension: ");
  scanf("%d", &n);

  printf("Computer plays (B/W): ");
  scanf(" %c", &computerColour);

  if (computerColour == 'B') {
    playerColour = 'W';
  } else {
    playerColour = 'B';
  }

  char board[26][26];

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      board[i][j] = 'U';
    }
  }

  board[n / 2 - 1][n / 2 - 1] = 'W';
  board[n / 2 - 1][n / 2] = 'B';
  board[n / 2][n / 2 - 1] = 'B';
  board[n / 2][n / 2] = 'W';

  printBoard(board, n);

  bool gameOver = false;
  char playerMove[3];
  playerMove[2] = 0;
  int computerRow = 0;
  int computerCol = 0;

  gameOver = !moveAvailable(board, n, computerColour) &&
             !moveAvailable(board, n, playerColour);

  for (char turn = 'B'; !gameOver; nextTurn(&turn)) {
    if (!moveAvailable(board, n, turn)) {
      printf("%c player has no valid move.\n", turn);
      continue;
    }

    if (playerColour == turn) {
      printf("Enter move for colour %c (RowCol): ", playerColour);
      scanf("%s", playerMove);

      if (!checkLegal(board, n, letterToNumber(playerMove[0]),
                      letterToNumber(playerMove[1]), playerColour)) {
        printf("Invalid move.\n%c player wins.\n", computerColour);
        return 0;
      } else {
        updateBoard(board, n, letterToNumber(playerMove[0]),
                    letterToNumber(playerMove[1]), playerColour);
      }
    } else if (computerColour == turn) {
      makeMove(board, n, computerColour, &computerRow, &computerCol);

      printf("Computer places %c at %c%c.\n", computerColour,
             numberToLetter(computerRow), numberToLetter(computerCol));
      updateBoard(board, n, computerRow, computerCol, computerColour);
    }

    printBoard(board, n);

    gameOver = !moveAvailable(board, n, computerColour) &&
               !moveAvailable(board, n, playerColour);
  }

  int Wscore = calculateScore(board, n, 'W');
  int Bscore = calculateScore(board, n, 'B');

  if (Bscore > Wscore) {
    printf("B player wins.\n");
  } else if (Wscore > Bscore) {
    printf("W player wins.\n");
  } else {
    printf("Draw!\n");
  }
}

/* void printBoard(char board[][26], int n) {
  for (int row = 0; row <= n; row++) {
    if (row == 0) {
      printf("  ");
      for (int col = 1; col <= n; col++) {
        printf("%c", 'a' - 1 + col);
      }
    } else if (row > 0) {
      printf("%c ", 'a' - 1 + row);
      for (int col = 1; col <= n; col++) {
        printf("%c", board[row - 1][col - 1]);
      }
    }
    printf("\n");
  }

  return;
}  */

void printBoard(char board[][26], int n) {
  for (int row = 0; row <= n; row++) {
    if (row == 0) {
      printf("\t");
      for (int col = 1; col <= n; col++) {
        printf("%c ", 'a' - 1 + col);
      }
    } else if (row > 0) {
      printf("%c\t", 'a' - 1 + row);
      for (int col = 1; col <= n; col++) {
        char c = board[row - 1][col - 1];
        if (c == 'U') c = '_';
        printf("%c ", c);
      }
    }
    printf("\n");
  }

  return;
}

void nextTurn(char *turn) {
  if (*turn == 'B') {
    *turn = 'W';
  } else {
    *turn = 'B';
  }
}

bool positionInBounds(int n, int row, int col) {
  if (row < n && col < n && row >= 0 && col >= 0) {
    return true;
  } else {
    return false;
  }
}

bool checkLegal(char board[][26], int n, int row, int col, char colour) {
  if (board[row][col] == 'U' && positionInBounds(n, row, col)) {
    for (int rowDir = -1; rowDir <= 1; rowDir++) {
      for (int colDir = -1; colDir <= 1; colDir++) {
        if (!(colDir == 0 && rowDir == 0) &&
            (checkLegalInDirection(board, n, row, col, colour, rowDir,
                                   colDir))) {
          return true;
        }
      }
    }
  }

  return false;
}

bool checkLegalInDirection(char board[][26], int n, int row, int col,
                           char colour, int deltaRow, int deltaCol) {
  char oppositeColour;
  if (colour == 'B') {
    oppositeColour = 'W';
  } else {
    oppositeColour = 'B';
  }

  if ((board[row][col] == 'U') &&
      (positionInBounds(n, row + deltaRow, col + deltaCol)) &&
      (board[row + deltaRow][col + deltaCol] == oppositeColour)) {
    for (int i = 2; positionInBounds(n, row + i * deltaRow, col + i * deltaCol);
         i++) {
      if (board[row + i * deltaRow][col + i * deltaCol] == colour) {
        return true;
      } else if (board[row + i * deltaRow][col + i * deltaCol] == 'U') {
        return false;
      }
    }
  }

  return false;
}

int letterToNumber(char letter) { return letter - 'a'; }

char numberToLetter(int number) { return (char)((int)'a' + number); }

void updateBoard(char board[][26], int n, int row, int col, char colour) {
  for (int rowDir = -1; rowDir <= 1; rowDir++) {
    for (int colDir = -1; colDir <= 1; colDir++) {
      if (!(colDir == 0 && rowDir == 0) &&
          (checkLegalInDirection(board, n, row, col, colour, rowDir, colDir))) {
        for (int i = 1; board[row + i * rowDir][col + i * colDir] != colour;
             i++) {
          board[row + i * rowDir][col + i * colDir] = colour;
        }
      }
    }
  }

  board[row][col] = colour;

  return;
}

bool moveAvailable(char board[][26], int n, char colour) {
  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      if (checkLegal(board, n, row, col, colour)) {
        return true;
      }
    }
  }

  return false;
}

int calculatePointsInDirection(char board[][26], int n, int row, int col,
                               char colour, int deltaRow, int deltaCol) {
  int points = 1;

  for (int i = 2; positionInBounds(n, row + i * deltaRow, col + i * deltaCol);
       i++) {
    if (board[row + i * deltaRow][col + i * deltaCol] == 'U' ||
        board[row + i * deltaRow][col + i * deltaCol] == colour) {
      break;
    } else {
      points++;
    }
  }

  return points;
}

int calculateScore(char board[][26], int n, char colour) {
  int score = 0;

  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      if (board[row][col] == colour) {
        score++;
      }
    }
  }

  return score;
}

int colourWon(char board[][26], int n, char colour) {
  char oppositeColour;

  if (colour == 'B') {
    oppositeColour = 'W';
  } else {
    oppositeColour = 'B';
  }

  int difference = calculateScore(board, n, colour) -
                   calculateScore(board, n, oppositeColour);

  return 200 * difference;
}

int makeMove(char board[][26], int n, char turn, int *row, int *col) {
  char computerColour = turn;
  char playerColour;

  if (computerColour == 'B') {
    playerColour = 'W';
  } else {
    playerColour = 'B';
  }

  computerScore(board, n, row, col, computerColour, playerColour, 1, INT_MIN,
                INT_MAX, discsPlayed(board, n));

  return 0;
}

double playerScore(char board[][26], int n, char computerColour,
                   char playerColour, int iteration, double alpha, double beta,
                   int progress) {
  char newBoard[26][26];

  bool playerMoveAvailable = moveAvailable(board, n, playerColour);

  if (!moveAvailable(board, n, computerColour) && !playerMoveAvailable) {
    return colourWon(board, n, computerColour);
  }

  if (iteration > DEPTH && progress < 45) {
    return calculateRelativeValue(board, n, computerColour, progress);
  } else if (iteration > 8) {
    return calculateRelativeValue(board, n, computerColour, progress);
  }

  if (!playerMoveAvailable) {
    beta = fmin(beta,
                computerScore(newBoard, n, NULL, NULL, computerColour,
                              playerColour, iteration, alpha, beta, progress));
    if (alpha >= beta) {
      return alpha;
    }
  } else {
    int rowOrder[60] = {0, 0, 7, 7, 2, 2, 2, 2, 3, 3, 4, 4, 5, 5, 5,
                        5, 0, 0, 0, 0, 7, 7, 7, 7, 2, 3, 4, 5, 2, 3,
                        4, 5, 1, 1, 1, 1, 6, 6, 6, 6, 2, 3, 4, 5, 2,
                        3, 4, 5, 0, 0, 1, 1, 6, 6, 7, 7, 1, 1, 6, 6};
    int colOrder[60] = {0, 7, 0, 7, 2, 3, 4, 5, 2, 5, 2, 5, 2, 3, 4,
                        5, 2, 3, 4, 5, 2, 3, 4, 5, 0, 0, 0, 0, 7, 7,
                        7, 7, 2, 3, 4, 5, 2, 3, 4, 5, 1, 1, 1, 1, 6,
                        6, 6, 6, 1, 6, 0, 7, 0, 7, 1, 6, 1, 6, 1, 6};
    for (int i = 0; i < 60; i++) {
      if (checkLegal(board, n, rowOrder[i], colOrder[i], playerColour)) {
        copyBoard(newBoard, board, n);
        updateBoard(newBoard, n, rowOrder[i], colOrder[i], playerColour);

        beta = fmin(beta, computerScore(newBoard, n, NULL, NULL, computerColour,
                                        playerColour, iteration + 1, alpha,
                                        beta, progress + 1));
        if (alpha >= beta) {
          return alpha;
        }
      }
    }
  }

  return beta;
}

double computerScore(char board[][26], int n, int *row, int *col,
                     char computerColour, char playerColour, int iteration,
                     double alpha, double beta, int progress) {
  char newBoard[26][26];

  bool computerMoveAvailable = moveAvailable(board, n, computerColour);

  if (!moveAvailable(board, n, computerColour) && !computerMoveAvailable) {
    return colourWon(board, n, computerColour);
  }

  if (iteration > DEPTH && progress < 45) {
    return calculateRelativeValue(board, n, computerColour, progress);
  } else if (iteration > 8) {
    return calculateRelativeValue(board, n, computerColour, progress);
  }

  if (!computerMoveAvailable) {
    alpha = fmax(alpha, playerScore(board, n, computerColour, playerColour,
                                    iteration, alpha, beta, progress));
    if (alpha >= beta) {
      return beta;
    }
  } else {
    int rowOrder[60] = {0, 0, 7, 7, 2, 2, 2, 2, 3, 3, 4, 4, 5, 5, 5,
                        5, 0, 0, 0, 0, 7, 7, 7, 7, 2, 3, 4, 5, 2, 3,
                        4, 5, 1, 1, 1, 1, 6, 6, 6, 6, 2, 3, 4, 5, 2,
                        3, 4, 5, 0, 0, 1, 1, 6, 6, 7, 7, 1, 1, 6, 6};
    int colOrder[60] = {0, 7, 0, 7, 2, 3, 4, 5, 2, 5, 2, 5, 2, 3, 4,
                        5, 2, 3, 4, 5, 2, 3, 4, 5, 0, 0, 0, 0, 7, 7,
                        7, 7, 2, 3, 4, 5, 2, 3, 4, 5, 1, 1, 1, 1, 6,
                        6, 6, 6, 1, 6, 0, 7, 0, 7, 1, 6, 1, 6, 1, 6};
    for (int i = 0; i < 60; i++) {
      if (checkLegal(board, n, rowOrder[i], colOrder[i], computerColour)) {
        copyBoard(newBoard, board, n);
        updateBoard(newBoard, n, rowOrder[i], colOrder[i], computerColour);

        double value = playerScore(newBoard, n, computerColour, playerColour,
                                   iteration + 1, alpha, beta, progress + 1);

        if (iteration == 1 && value > alpha) {
          *row = rowOrder[i];
          *col = colOrder[i];
        }

        alpha = fmax(alpha, value);
        if (alpha >= beta) {
          return beta;
        }
      }
    }
  }

  return alpha;
}

double calculateRelativeValue(char board[][26], int n, char colour,
                              int progress) {
  char oppositeColour;

  if (colour == 'B') {
    oppositeColour = 'W';
  } else {
    oppositeColour = 'B';
  }

  double score_count = 0;
  double oppScore_count = 0;
  double score_corner = 0;
  double oppScore_corner = 0;
  double score_byCorner = 0;
  double oppScore_byCorner = 0;
  double score_center = 0;
  double oppScore_center = 0;
  double score_balancedEdge = 0;
  double oppScore_balancedEdge = 0;
  // double score_static = 0;
  // double oppScore_static = 0;
  double score_frontier = 0;
  double oppScore_frontier = 0;
  double score_mobility = 0;
  double oppScore_mobility = 0;

  /* int staticValues[26][26] = {
      {100, -7, 0, 0, 0, 0, -7, 100}, {-7, -10, 0, 0, 0, 0, -10, -7}, {0},
      {0, 0, 0, 5, 5, 0, 0, 0},       {0, 0, 0, 5, 5, 0, 0, 0},       {0},
      {-7, -10, 0, 0, 0, 0, -10, -7}, {100, -7, 0, 0, 0, 0, -7, 100}}; */

  for (int row = 0; row < n; row++) {
    for (int col = 0; col < n; col++) {
      if (board[row][col] == colour) {
        score_count++;  // Number of discs
        bool doubleBreak = false;
        for (int rowDir = -1; rowDir <= 1; rowDir++) {  // Frontier discs
          for (int colDir = -1; colDir <= 1; colDir++) {
            if (!(rowDir == 0 && colDir == 0) &&
                positionInBounds(n, row + rowDir, col + colDir) &&
                board[row + rowDir][col + colDir] == 'U') {
              score_frontier++;
              doubleBreak = true;
              break;
            }
          }
          if (doubleBreak) break;
        }
      } else if (board[row][col] == oppositeColour) {
        oppScore_count++;  // Number of discs
        bool doubleBreak = false;
        for (int rowDir = -1; rowDir <= 1; rowDir++) {  // Frontier discs
          for (int colDir = -1; colDir <= 1; colDir++) {
            if (!(rowDir == 0 && colDir == 0) &&
                positionInBounds(n, row + rowDir, col + colDir) &&
                board[row + rowDir][col + colDir] == 'U') {
              oppScore_frontier++;
              doubleBreak = true;
              break;
            }
          }
          if (doubleBreak) break;
        }
      } else {  // Number of legal moves
        if (checkLegal(board, n, row, col, colour)) {
          score_mobility++;
        }
        if (checkLegal(board, n, row, col, oppositeColour)) {
          oppScore_mobility++;
        }
      }
    }
  }

  // Corner discs
  if (board[0][0] == colour) score_corner++;
  if (board[0][7] == colour) score_corner++;
  if (board[7][0] == colour) score_corner++;
  if (board[7][7] == colour) score_corner++;

  if (board[0][0] == oppositeColour) oppScore_corner++;
  if (board[0][7] == oppositeColour) oppScore_corner++;
  if (board[7][0] == oppositeColour) oppScore_corner++;
  if (board[7][7] == oppositeColour) oppScore_corner++;

  // byCorner discs
  if (board[0][0] == 'U') {
    if (board[0][1] == colour) score_byCorner++;
    if (board[0][6] == colour) score_byCorner++;
    if (board[1][1] == colour) score_byCorner++;

    if (board[0][1] == oppositeColour) oppScore_byCorner++;
    if (board[0][6] == oppositeColour) oppScore_byCorner++;
    if (board[1][1] == oppositeColour) oppScore_byCorner++;
  }

  if (board[0][7] == 'U') {
    if (board[1][0] == colour) score_byCorner++;
    if (board[1][7] == colour) score_byCorner++;
    if (board[1][6] == colour) score_byCorner++;

    if (board[1][0] == oppositeColour) oppScore_byCorner++;
    if (board[1][7] == oppositeColour) oppScore_byCorner++;
    if (board[1][6] == oppositeColour) oppScore_byCorner++;
  }

  if (board[7][0] == 'U') {
    if (board[6][0] == colour) score_byCorner++;
    if (board[6][7] == colour) score_byCorner++;
    if (board[6][1] == colour) score_byCorner++;

    if (board[6][0] == oppositeColour) oppScore_byCorner++;
    if (board[6][7] == oppositeColour) oppScore_byCorner++;
    if (board[6][1] == oppositeColour) oppScore_byCorner++;
  }

  if (board[7][7] == 'U') {
    if (board[7][1] == colour) score_byCorner++;
    if (board[7][6] == colour) score_byCorner++;
    if (board[6][6] == colour) score_byCorner++;

    if (board[7][1] == oppositeColour) oppScore_byCorner++;
    if (board[7][6] == oppositeColour) oppScore_byCorner++;
    if (board[6][6] == oppositeColour) oppScore_byCorner++;
  }

  // Center discs
  if (board[2][2] == colour) score_center++;
  if (board[2][3] == colour) score_center++;
  if (board[2][4] == colour) score_center++;
  if (board[2][5] == colour) score_center++;
  if (board[3][2] == colour) score_center++;
  if (board[3][3] == colour) score_center += 2;
  if (board[3][4] == colour) score_center += 2;
  if (board[3][5] == colour) score_center++;
  if (board[4][2] == colour) score_center++;
  if (board[4][3] == colour) score_center += 2;
  if (board[4][4] == colour) score_center += 2;
  if (board[4][5] == colour) score_center++;
  if (board[5][2] == colour) score_center++;
  if (board[5][3] == colour) score_center++;
  if (board[5][4] == colour) score_center++;
  if (board[5][5] == colour) score_center++;

  if (board[2][2] == oppositeColour) oppScore_center++;
  if (board[2][3] == oppositeColour) oppScore_center++;
  if (board[2][4] == oppositeColour) oppScore_center++;
  if (board[2][5] == oppositeColour) oppScore_center++;
  if (board[3][2] == oppositeColour) oppScore_center++;
  if (board[3][3] == oppositeColour) oppScore_center += 2;
  if (board[3][4] == oppositeColour) oppScore_center += 2;
  if (board[3][5] == oppositeColour) oppScore_center++;
  if (board[4][2] == oppositeColour) oppScore_center++;
  if (board[4][3] == oppositeColour) oppScore_center += 2;
  if (board[4][4] == oppositeColour) oppScore_center += 2;
  if (board[4][5] == oppositeColour) oppScore_center++;
  if (board[5][2] == oppositeColour) oppScore_center++;
  if (board[5][3] == oppositeColour) oppScore_center++;
  if (board[5][4] == oppositeColour) oppScore_center++;
  if (board[5][5] == oppositeColour) oppScore_center++;

  // Balanced edges
  if (board[0][0] == 'U' && board[0][7] == 'U') {
    if (board[0][2] == colour && board[0][3] == colour &&
        board[0][4] == colour && board[0][5] == colour) {
      score_balancedEdge++;
      if (board[0][1] == colour && board[0][6] == colour) score_balancedEdge++;
    }
    if (board[0][2] == oppositeColour && board[0][3] == oppositeColour &&
        board[0][4] == oppositeColour && board[0][5] == oppositeColour) {
      oppScore_balancedEdge++;
      if (board[0][1] == oppositeColour && board[0][6] == oppositeColour)
        oppScore_balancedEdge++;
    }
  }

  if (board[0][0] == 'U' && board[7][0] == 'U') {
    if (board[2][0] == colour && board[3][0] == colour &&
        board[4][0] == colour && board[5][0] == colour) {
      score_balancedEdge++;
      if (board[1][0] == colour && board[6][0] == colour) score_balancedEdge++;
    }
    if (board[2][0] == oppositeColour && board[3][0] == oppositeColour &&
        board[4][0] == oppositeColour && board[5][0] == oppositeColour) {
      oppScore_balancedEdge++;
      if (board[1][0] == oppositeColour && board[6][0] == oppositeColour)
        oppScore_balancedEdge++;
    }
  }

  if (board[0][7] == 'U' && board[7][7] == 'U') {
    if (board[2][7] == colour && board[3][7] == colour &&
        board[4][7] == colour && board[5][7] == colour) {
      score_balancedEdge++;
      if (board[1][7] == colour && board[6][7] == colour) score_balancedEdge++;
    }
    if (board[2][7] == oppositeColour && board[3][7] == oppositeColour &&
        board[4][7] == oppositeColour && board[5][7] == oppositeColour) {
      oppScore_balancedEdge++;
      if (board[1][7] == oppositeColour && board[6][7] == oppositeColour)
        oppScore_balancedEdge++;
    }
  }

  if (board[7][0] == 'U' && board[7][7] == 'U') {
    if (board[7][2] == colour && board[7][3] == colour &&
        board[7][4] == colour && board[7][5] == colour) {
      score_balancedEdge++;
      if (board[7][1] == colour && board[7][6] == colour) score_balancedEdge++;
    }
    if (board[7][2] == oppositeColour && board[7][3] == oppositeColour &&
        board[7][4] == oppositeColour && board[7][5] == oppositeColour) {
      oppScore_balancedEdge++;
      if (board[7][1] == oppositeColour && board[7][6] == oppositeColour)
        oppScore_balancedEdge++;
    }
  }

  double count_final;
  double corner_final;
  double byCorner_final;
  double center_final;
  double balancedEdge_final;
  double frontier_final;
  double mobility_final;

  if (score_count > oppScore_count) {
    count_final = score_count / (score_count + oppScore_count);
  } else if (score_count < oppScore_count) {
    count_final = -1 * oppScore_count / (score_count + oppScore_count);
  } else {
    count_final = 0;
  }

  corner_final = (score_corner - oppScore_corner) / 4;

  byCorner_final = (score_byCorner - oppScore_byCorner) / 12;

  center_final = (score_center - oppScore_center) / 20;

  balancedEdge_final = (score_balancedEdge - oppScore_balancedEdge) / 8;

  if (score_frontier > oppScore_frontier) {
    frontier_final = score_frontier / (score_frontier + oppScore_frontier);
  } else if (score_frontier < oppScore_frontier) {
    frontier_final =
        -1 * oppScore_frontier / (score_frontier + oppScore_frontier);
  } else {
    frontier_final = 0;
  }

  if (score_mobility > oppScore_mobility) {
    mobility_final = score_mobility / (score_mobility + oppScore_mobility);
  } else if (score_mobility < oppScore_mobility) {
    mobility_final =
        -1 * oppScore_mobility / (score_mobility + oppScore_mobility);
  } else {
    mobility_final = 0;
  }

  double count_weight = 0;
  double corner_weight = 0;
  double byCorner_weight = 0;
  double center_weight = 0;
  double balancedEdge_weight = 0;
  double frontier_weight = 0;
  double mobility_weight = 0;

  if (progress <= 20) {
    count_weight = 0.5;
    corner_weight = 80;
    byCorner_weight = -40;
    center_weight = 30;
    balancedEdge_weight = 20;
    frontier_weight = -13;
    mobility_weight = 17;
  } else if (progress <= 40) {
    count_weight = 1;
    corner_weight = 125;
    byCorner_weight = -50;
    center_weight = 15;
    balancedEdge_weight = 60;
    frontier_weight = -3.5;
    mobility_weight = 6.5;
  } else {
    count_weight = 3;
    corner_weight = 110;
    byCorner_weight = -9;
    center_weight = 0;
    balancedEdge_weight = 40;
    frontier_weight = -3.5;
    mobility_weight = 2;
  }

  return count_final * count_weight + corner_final * corner_weight +
         byCorner_final * byCorner_weight + center_final * center_weight +
         balancedEdge_final * balancedEdge_weight +
         frontier_final * frontier_weight + mobility_final * mobility_weight;
}

void copyBoard(char boardTo[][26], char boardFrom[][26], int n) {
  for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
      boardTo[j][i] = boardFrom[j][i];
    }
  }
}

int discsPlayed(char board[][26], int n) {
  int count = -4;

  for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
      if (board[j][i] != 'U') {
        count++;
      }
    }
  }

  return count;
}
