
#include <iostream>
#include <string>

#include "board.h"
#include "dict.h"
#include "util.h"
using namespace std;

void score_board(Node &root, Board &b) {
  Node::g_timestamp++;
  int score = 0;
  for (int i = 0; i < BOARD_SIZE; i++) {
    score += root.children[b[i]]->score_tile(b, i, 1);
  }
  b.score = score;
}

Board steepest_ascent_move(Node &root, const Board &b) {
  Board best_board = b;
  best_board.score = 0;  // allow downward moves if we're at a peak
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < ALPHABET_SIZE; j++) {
      if (j == b[i]) continue;
      Board nb = b;
      nb[i] = j;
      score_board(root, nb);
      if (nb.score > best_board.score) best_board = nb;
    }
  }
  return best_board;
}

int main() {
  build_neighbors();
  Node root;
  int score = 0;
  srand(time(0));
  if (read_dictionary(root)) {
    double start_time = get_cpu_time();
    Board best_board;
    for (size_t j = 0; j < 1000; j++) {
      Board b;
      Board local_best = b;
      for (int i = 0; i < 35; i++) {
        b = steepest_ascent_move(root, b);
        if (b.score > local_best.score) local_best = b;
      }
      if (local_best.score > 5000) cout << local_best << endl;
      if (local_best.score > best_board.score) best_board = local_best;
    }
    double run_time = get_cpu_time() - start_time;
    cout << fixed;
    cout << setprecision(3);
    cout << "run time " << run_time << endl;
    cout << best_board << endl;
  }
  //   root.print_words();

  return 0;
}