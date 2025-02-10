#include "board.h"

// We cash the neighbors of a tile to save time while scoring a board
// These represent the 8 neighbors of a tile or grid position
// up, down, left, right and the four diagonals
int row_inc[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
int col_inc[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

// neighbor cache
int **neighbors;

// for any tile get it's neighbors
// start with the 8 grid neighbors and then filter out any tiles
// that are out of the grid
int get_neighbor(int tile, int neighbor) {
  int row = (tile / GRID_SIZE) + row_inc[neighbor];
  int col = (tile % GRID_SIZE) + col_inc[neighbor];
  if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return -1;
  return (row * GRID_SIZE) + col;
}

// build the neighbor cache
void build_neighbors() {
  neighbors = new int *[BOARD_SIZE];

  for (int i = 0; i < BOARD_SIZE; i++) {
    neighbors[i] = new int[8];

    for (int j = 0; j < 8; j++) {
      int neighbor = get_neighbor(i, j);
      neighbors[i][j] = get_neighbor(i, j);
    }
  }
}