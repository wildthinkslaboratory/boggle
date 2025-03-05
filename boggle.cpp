
#include <fstream>
#include <iostream>
#include <string>

#include "board.h"
#include "dict.h"
#include "util.h"
using namespace std;

// number of times to try to improve a board with polishing
const int POLISH_ATTEMPTS = 20;
// number of flips in a board perturbation
const int PERTURB_FLIPS = 8;

// //
// //
// //
// int good_letters[19] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'k',
//                         'l', 'm', 'n', 'o', 'p', 'r', 's', 't', 'u'};

void output_commandline_args() {
  cout << "usage: [-seed <int>] <dictionary_file> <command> <board>" << endl;
  cout << "Seed is optional" << endl;
  cout << "Available command:" << endl;
  cout << "\t\tscore <board>" << endl;
  cout << "\t\twords <board>" << endl;
  cout << "\t\tbenchmark <board>" << endl;
  cout << "\t\tpolish <board>" << endl;
  cout << "\t\tsa_probes\t\t # run 100 probes using steepest ascent, save in "
          "sa_probes.dat"
       << endl;
  cout << "\t\thc_probes\t\t # run 100 probes using hill climbing, save in "
          "hc_probes.dat"
       << endl;
  cout << "\t\tperturb <boards_file>\t # read a list of boards then perturb "
          "and polish them"
       << endl;
  cout << "\t\tsearch\t\t\t # search for a high scoring board" << endl;
}

// to score a board, we walk through each board tile and tally
// up the score for all words that start that tile
void score_board(Node &root, Board &b) {
  Node::g_timestamp++;
  int score = 0;
  for (int i = 0; i < BOARD_SIZE; i++) {
    score += root.children[b[i]]->score_tile(b, i, 1);
  }
  b.score = score;
}

// find the steepest ascent neighbor for this board.  We consider
// all boards created by changing a single tile on the board to a new letter
// and we return the highest scoring neighbor.
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

// Run a series of probes using steepest ascent
// Output the probes to a file
//
void steepest_ascent_probes(Node &root, const int num_probes) {
  ofstream outfile;
  outfile.open("sa_probes.dat", std::ios_base::app);

  for (size_t j = 0; j < num_probes; j++) {
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

// Run a series of probes using first uphill
// Output the probes to a file
//
void first_up_probes(Node &root, const int num_probes) {
  ofstream outfile;
  outfile.open("fu_probes.dat", std::ios_base::app);

  for (size_t j = 0; j < num_probes; j++) {
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

// Run a series of probes using hill climbing
// Output the probes to a file
//
void hill_climb_probes(Node &root, const int num_probes) {
  ofstream outfile;
  outfile.open("hc_probes.dat", std::ios_base::app);

  for (size_t j = 0; j < num_probes; j++) {
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

// randomly flip some tiles in the board
void perturb(Board &b, int num_flips) {
  for (int k = 0; k < num_flips; k++) {
    // randomly flip a tile
    int index = rand() % 25;  // random tile and letter
    int letter = rand() % 26;
    b[index] = letter;
  }
}

// randomly perturb a board and then apply
// steepest ascent to the perturbed board
Board polish(Node &root, const Board &iboard) {
  int original_score = iboard.score;
  Board best = iboard;
  for (int k = 0; k < POLISH_ATTEMPTS; k++) {
    Board b = iboard;
    perturb(b, PERTURB_FLIPS);

    for (int j = 0; j < 15; j++) {
      b = steepest_ascent_move(root, b);
      if (b.score > best.score) {
        best = b;
      }
    }
  }
  return best;
}

const int MAX_ITERATIONS = 20;

void search(Node &root) {
  Board very_best;

  int p_size = 600;  // population size
  vector<Board> best;

  // main loop
  // we can have a max iterations and a max time
  double start_time = get_cpu_time();
  for (int i = 0; i < MAX_ITERATIONS; i++) {
    // create a population of boards
    Board population[p_size];

    // first we hill climb on our boards
    // we do a full probe that requires 95 iterations to reach maximums
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

    // We polish boards with score over 5000
    // this is a good number for the dictBig.txt
    // would be better if this adjusted depending on the
    // dictionary.
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
        steepest_ascent_probes(dictionary_root, 100);
      } else if (command == "hc_probes") {
        hill_climb_probes(dictionary_root, 100);
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
      } else if (command == "polish") {
        string s;
        Board b;
        if (argc == command_arg + 2) {  // read in a board if given
          s = argv[dict_arg + 2];
          b = Board(s);
        }

        score_board(dictionary_root, b);
        cout << b << endl;
        double start_time = get_cpu_time();
        Board polished_board = polish(dictionary_root, b);
        double time = get_cpu_time() - start_time;
        cout << polished_board << endl;
        cout << "time: " << time << endl;
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
