
#include <iostream>
#include <string>

#include "board.h"
#include "dict.h"
#include "util.h"
using namespace std;

const int score_board(const Node &root, const Board &b) {
  Node::g_timestamp++;
  int score = 0;
  for (int i = 0; i < BOARD_SIZE; i++) {
    score += root.children[b.letters[i]]->score_tile(b, i, 1);
  }
  return score;
}

int main() {
  build_neighbors();
  Node root;
  int score = 0;
  if (read_dictionary(root)) {
    Board b("rscsmgetaidnrtneiaesslpsd");
    double start_time = get_cpu_time();
    for (int i = 0; i < 1000; i++) score = score_board(root, b);
    double run_time = get_cpu_time() - start_time;
    cout << fixed;
    cout << setprecision(3);
    cout << "run time " << run_time << endl;
    cout << "score " << score << endl;
  }

  //   root.print_words();

  return 0;
}