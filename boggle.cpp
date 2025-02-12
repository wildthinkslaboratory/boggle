
#include <fstream>
#include <iostream>
#include <string>

#include "board.h"
#include "dict.h"
#include "util.h"
using namespace std;

void output_commandline_args() {
  cout << "usage: [-seed <int>] <dictionary_file> <command> <board>" << endl;
  cout << "Seed is optional" << endl;
  cout << "Available command:" << endl;
  cout << "\t\tscore <board>" << endl;
  cout << "\t\twords <board>" << endl;
  cout << "\t\tbenchmark <board>" << endl;
  cout << "\t\tsteepest_ascent" << endl;
}

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

//
//
//
void random_steepest_ascent(Node &root) {
  int score = 0;
  ofstream outfile;
  outfile.open("boards.dat", std::ios_base::app);

  double start_time = get_cpu_time();
  Board best_board;
  for (size_t j = 0; j < 2; j++) {
    Board b;
    Board local_best = b;
    for (int i = 0; i < 3; i++) {
      b = steepest_ascent_move(root, b);
      if (b.score > local_best.score) local_best = b;
    }

    if (local_best.score > 0) {
      outfile << local_best << endl;
      cout << local_best << endl;
    }
    if (local_best.score > best_board.score) best_board = local_best;
  }

  double run_time = get_cpu_time() - start_time;
  outfile.close();
  cout << fixed;
  cout << setprecision(3);
  cout << "time: " << run_time << endl;
  cout << best_board << endl;
}

//
//
//
//
int main(int argc, char **argv) {
  if (argc < 3) output_commandline_args();

  int dict_arg = 1;
  // check if they've entered a seed
  if (argv[dict_arg][0] == '-') {
    srand(atoi(argv[2]));
    dict_arg = dict_arg + 2;
  } else
    srand(time(0));

  int command_arg = dict_arg + 1;
  string command = argv[command_arg];
  string s;
  Board b;
  if (argc == command_arg + 2) {  // read in a board if given
    s = argv[dict_arg + 2];
    b = Board(s);
  }

  Node dictionary_root;
  if (read_dictionary(dictionary_root, argv[dict_arg])) {  // read dictionary
    build_neighbors();

    // manage various commands
    if (argc <= command_arg - 1) {  // no board arg
      if (command == "steepest_ascent") {
        random_steepest_ascent(dictionary_root);
      } else {
        output_commandline_args();
      }
    } else {
      if (command == "score") {
        cout << "scoring board " << b << endl;
        score_board(dictionary_root, b);
        cout << b << endl;

      } else if (command == "words") {
        score_board(dictionary_root, b);
        dictionary_root.print_words();

      } else if (command == "benchmark") {
        double start_time = get_cpu_time();
        for (int i = 0; i < 1000; i++) score_board(dictionary_root, b);
        cout << fixed;
        cout << setprecision(3);
        cout << "time to score 1000 boards: " << get_cpu_time() - start_time
             << endl;
        cout << b << endl;
      } else {
        output_commandline_args();
      }
    }
  }

  return 0;
}
