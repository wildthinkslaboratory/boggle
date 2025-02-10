#ifndef BOARD_H
#define BOARD_H
#include <iostream>
#include <string>

#include "util.h"
using namespace std;

const int GRID_SIZE = 5;
const int BOARD_SIZE = GRID_SIZE * GRID_SIZE;
inline int board_index(int row, int col) { return (row * GRID_SIZE) + col; }

// Board is a 5 x 5 grid of letter tiles represented as an array of 25 letters
struct Board {
  int letters[BOARD_SIZE];
  Board();
  Board(const string &b);
  void print_grid() const;
};

// We cash the neighbors of a tile to save time while scoring a board
extern int **neighbors;
int get_neighbor(int tile, int neighbor);
void build_neighbors();

inline Board::Board() {
  for (int i = 0; i < BOARD_SIZE; i++) {
    letters[i] = random_int(26);
  }
}

inline Board::Board(const string &b) {
  assert(b.length() == 25);
  for (int i = 0; i < b.length(); i++) {
    letters[i] = IDX(b[i]);
  }
}

inline void Board::print_grid() const {
  for (size_t i = 0; i < GRID_SIZE; i++) {
    for (size_t j = 0; j < GRID_SIZE; j++) {
      cout << CHR(letters[board_index(i, j)]) << ' ';
    }
    cout << endl;
  }
}

inline std::ostream &operator<<(std::ostream &os, const Board &b) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    os << CHR(b.letters[i]);
  };
  return os;
}

#endif
