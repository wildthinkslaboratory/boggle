#ifndef BOARD_H
#define BOARD_H
#include <array>
#include <iostream>
#include <string>

#include "util.h"
using namespace std;

const int GRID_SIZE = 5;
const int BOARD_SIZE = GRID_SIZE * GRID_SIZE;
inline int board_index(int row, int col) { return (row * GRID_SIZE) + col; }

class Board2 : public array<int, BOARD_SIZE> {
 public:
  Board2();
  Board2(const string &b);
  void print_grid() const;
};

// Board is a 5 x 5 grid of letter tiles represented as an array
// of 25 letters
struct Board {
  int letters[BOARD_SIZE];
  Board();
  Board(const string &b);
  void print_grid() const;
};

// We cash the neighbors of a tile to save time while scoring a board
// you can see how the cache is built in board.cpp
extern int **neighbors;
int get_neighbor(int tile, int neighbor);
void build_neighbors();

inline Board::Board() {
  for (int i = 0; i < BOARD_SIZE; i++) {
    letters[i] = random_int(26);
  }
}

inline Board::Board(const string &b) {
  assert(b.length() == BOARD_SIZE);
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

inline Board2::Board2() {
  for (int i = 0; i < size(); i++) {
    operator[](i) = random_int(26);
  }
}

inline Board2::Board2(const string &b) {
  assert(b.length() == size());
  for (int i = 0; i < b.length(); i++) {
    operator[](i) = IDX(b[i]);
  }
}

inline void Board2::print_grid() const {
  for (size_t i = 0; i < GRID_SIZE; i++) {
    for (size_t j = 0; j < GRID_SIZE; j++) {
      cout << CHR(operator[](board_index(i, j))) << ' ';
    }
    cout << endl;
  }
}

inline std::ostream &operator<<(std::ostream &os, const Board2 &b) {
  for (int i = 0; i < b.size(); i++) os << CHR(b[i]) << ' ';
  return os;
}

#endif
