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

// trying out this stl array class
// it's slightly slower than a c array but it's
// so nice and readable and I get to have
// member functions.
class Board : public array<int, BOARD_SIZE> {
 public:
  int score;
  Board();
  Board(const string &b);
  void print_grid() const;
};

// We cash the neighbors of a tile to save time while scoring a board
// you can see how the cache is built in board.cpp
extern int **neighbors;
int get_neighbor(int tile, int neighbor);
void build_neighbors();
void print_neighbors();

// Board member functions
inline Board::Board() : score(0) {
  for (int i = 0; i < size(); i++) {
    operator[](i) = random_int(26);
  }
}

inline Board::Board(const string &b) : score(0) {
  assert(b.length() == size());
  for (int i = 0; i < b.length(); i++) {
    operator[](i) = IDX(b[i]);
  }
}

inline void Board::print_grid() const {
  for (size_t i = 0; i < GRID_SIZE; i++) {
    for (size_t j = 0; j < GRID_SIZE; j++) {
      cout << CHR(operator[](board_index(i, j))) << ' ';
    }
    cout << endl;
  }
}

inline std::ostream &operator<<(std::ostream &os, const Board &b) {
  for (int i = 0; i < b.size(); i++) os << CHR(b[i]);
  os << "\tSCORE: " << b.score;
  return os;
}

#endif
