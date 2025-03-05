
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

// a neighbor is flipping a tile to a new letter. Here we
// score every single neighbor and take the highest scoring one.
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

// a neighbor is flipping two tiles each to a new letter. Here
// we score all neighbors and take the best one.
Board steepest_ascent_double(Node &root, const Board &b) {
  Board best_board = b;
  best_board.score = 0;  // allow downward moves if we're at a peak
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int k = i + 1; k < BOARD_SIZE; k++) {
      for (int j = 0; j < ALPHABET_SIZE; j++) {
        for (int l = j + 1; l < ALPHABET_SIZE; l++) {
          Board nb = b;
          nb[i] = j;
          nb[k] = l;
          score_board(root, nb);
          if (nb.score > best_board.score) best_board = nb;
        }
      }
    }
  }
  return best_board;
}

// a neighbor is flipping a tile to a new letter. Here we
// proceed systematically until we improve the score.
// This works really well, but it is biased towards
// flipping tiles with low index to letters with low index.
Board first_up_move(Node &root, const Board &b) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < ALPHABET_SIZE; j++) {
      if (j == b[i]) continue;
      Board nb = b;
      nb[i] = j;
      score_board(root, nb);
      if (nb.score > b.score) return nb;
    }
  }
  return b;
}

// a neighbor is flipping a tile to a new letter.  Here
// we randomly flip a tile and letter and we keep
// flipping until we improve our score. We quit after
// 200 failed flips.

Board hill_climb_move(Node &root, const Board &b) {
  Board nb = b;
  for (size_t j = 0; j < 300; j++) {
    int index = rand() % 25;  // random tile and letter
    int letter = rand() % 26;
    nb[index] = letter;
    score_board(root, nb);
    if (nb.score > b.score) {
      return nb;
    }
    nb[index] = b[index];
  }
  return b;
}

//
//
//
void steepest_ascent_probes(Node &root) {
  ofstream outfile;
  outfile.open("sa_probes.dat", std::ios_base::app);

  for (size_t j = 0; j < 10; j++) {
    Board b;
    Board best = b;
    int it_to_max = 0;
    double start_time = get_cpu_time();
    for (int i = 0; i < 40; i++) {
      b = steepest_ascent_move(root, b);
      if (b.score > best.score) {
        best = b;
        it_to_max = i;
      }
    }
    double run_time = get_cpu_time() - start_time;

    outfile << best << "\t" << run_time << "\t" << it_to_max << endl;
    cout << best << "\t" << run_time << "\t" << it_to_max << endl;
  }
  outfile.close();
}

//
//
//
void first_up_probes(Node &root) {
  ofstream outfile;
  outfile.open("fu_probes.dat", std::ios_base::app);

  for (size_t j = 0; j < 200; j++) {
    Board b;
    Board best = b;
    int it_to_max = 0;
    double start_time = get_cpu_time();
    for (int i = 0; i < 130; i++) {
      b = first_up_move(root, b);
      if (b.score > best.score) {
        best = b;
        it_to_max = i;
      }
    }
    double run_time = get_cpu_time() - start_time;

    outfile << best << "\t" << run_time << "\t" << it_to_max << endl;
    cout << best << "\t" << run_time << "\t" << it_to_max << endl;
  }
  outfile.close();
}

//
//
//
void hill_climb_probes(Node &root) {
  ofstream outfile;
  outfile.open("hc_probes.dat", std::ios_base::app);

  for (size_t j = 0; j < 200; j++) {
    Board b;
    Board best = b;
    int it_to_max = 0;
    double start_time = get_cpu_time();
    for (int i = 0; i < 130; i++) {
      b = hill_climb_move(root, b);
      if (b.score > best.score) {
        best = b;
        it_to_max = i;
      }
    }
    double run_time = get_cpu_time() - start_time;

    outfile << best << "\t" << run_time << "\t" << it_to_max << endl;
    cout << best << "\t" << run_time << "\t" << it_to_max << endl;
  }
  outfile.close();
}

void perturb(Board &b) {
  for (int k = 0; k < 8; k++) {
    // randomly flip a tile
    int index = rand() % 25;  // random tile and letter
    int letter = rand() % 26;
    b[index] = letter;
  }
}

Board polish(Node &root, const Board &iboard) {
  int original_score = iboard.score;
  Board best = iboard;
  for (int k = 0; k < 20; k++) {
    Board b = iboard;
    perturb(b);

    for (int j = 0; j < 15; j++) {
      b = steepest_ascent_move(root, b);
      if (b.score > best.score) {
        best = b;
      }
    }
  }
  return best;
}

//
//
//
int good_letters[19] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'k',
                        'l', 'm', 'n', 'o', 'p', 'r', 's', 't', 'u'};

void search(Node &root) {
  Board very_best;

  int p_size = 600;  // population size
  int b_size = 10;   // size of high scoring group
  vector<Board> best;

  // main loop
  // we can have a max iterations and a max time
  double start_time = get_cpu_time();
  for (int i = 0; i < 20; i++) {
    // create a population of boards
    Board population[p_size];

    // first we hill climb on our boards
    // we do a full probe that requires 100 iterations to reach maximums
    for (int j = 0; j < p_size; j++) {
      for (int it = 0; it < 95; it++) {
        population[j] = hill_climb_move(root, population[j]);
      }
      if (population[j].score > very_best.score) {
        very_best = population[j];
        cout << very_best << "\t" << "time:\t" << get_cpu_time() - start_time
             << endl;
      }
    }

    // move high scoring boards into the best group
    // need to sort them and take highest 10%
    // but I don't want to write a sorting algorithm
    for (int j = 0; j < p_size; j++) {
      if (population[j].score > 5000) {
        best.push_back(population[j]);
      }
    }

    // now we polish our high scorers
    for (size_t j = 0; j < best.size(); j++) {
      best[j] = polish(root, best[j]);
      if (best[j].score > very_best.score) {
        very_best = best[j];
        cout << very_best << "\t" << "time:\t" << get_cpu_time() - start_time
             << endl;
      }
    }
  }
}

//
//
//
//
int main(int argc, char **argv) {
  if (argc < 3) {
    output_commandline_args();
    return 1;
  }

  int dict_arg = 1;
  // check if they've entered a seed
  if (argv[dict_arg][0] == '-') {
    srand(atoi(argv[2]));
    dict_arg = dict_arg + 2;
  } else
    srand(time(0));

  int command_arg = dict_arg + 1;
  string command = argv[command_arg];

  Node dictionary_root;
  if (read_dictionary(dictionary_root, argv[dict_arg])) {  // read dictionary
    build_neighbors();

    // manage various commands
    if (argc <= command_arg + 1) {  // no board arg
      if (command == "sa_probes") {
        steepest_ascent_probes(dictionary_root);
      } else if (command == "hc_probes") {
        hill_climb_probes(dictionary_root);
      } else if (command == "search") {
        search(dictionary_root);
      } else {
        output_commandline_args();
      }
    } else {
      if (command == "score") {
        string s;
        Board b;
        if (argc == command_arg + 2) {  // read in a board if given
          s = argv[dict_arg + 2];
          b = Board(s);
        }

        score_board(dictionary_root, b);
        cout << b << endl;

      } else if (command == "words") {
        string s;
        Board b;
        if (argc == command_arg + 2) {  // read in a board if given
          s = argv[dict_arg + 2];
          b = Board(s);
        }

        score_board(dictionary_root, b);
        dictionary_root.print_words();

      } else if (command == "benchmark") {
        string s;
        Board b;
        if (argc == command_arg + 2) {  // read in a board if given
          s = argv[dict_arg + 2];
          b = Board(s);
        }

        double start_time = get_cpu_time();
        for (int i = 0; i < 1000; i++) score_board(dictionary_root, b);
        cout << fixed;
        cout << setprecision(3);
        cout << "time to score 1000 boards: " << get_cpu_time() - start_time
             << endl;
        cout << b << endl;
      } else if (command == "polishing") {
        string s;
        Board b;
        if (argc == command_arg + 2) {  // read in a board if given
          s = argv[dict_arg + 2];
          b = Board(s);
        }

        double start_time = get_cpu_time();
        score_board(dictionary_root, b);
        Board refined_board = steepest_ascent_double(dictionary_root, b);
        cout << "time: " << get_cpu_time() - start_time << endl;
        cout << refined_board << endl;
      } else if (command == "perturb") {
        read_boards(argv[dict_arg + 2]);
        for (size_t i = 0; i < input_boards.size(); i++) {
          score_board(dictionary_root, input_boards[i]);
          Board polished = polish(dictionary_root, input_boards[i]);
          cout << input_boards[i] << "\t" << polished << endl;
        }
      } else {
        output_commandline_args();
      }
    }
  }

  return 0;
}
