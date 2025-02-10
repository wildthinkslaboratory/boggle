#ifndef DICT
#define DICT
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "board.h"
#include "util.h"
using namespace std;

class Node {
 public:
  Node *children[26];
  bool is_word;
  int timestamp;
  static int g_timestamp;

  Node();
  void add_word(const string &s, int i);
  void print_words() const;
  bool lookup_word(const string &s, int i) const;
  int score_tile(const Board &b, int tile, int depth);
  ~Node();

 private:
  void print_words(string prefix) const;
};

bool read_dictionary(Node &root);

inline int word_score(int l) {
  switch (l) {
    case 0:
      return 0;
    case 1:
      return 0;
    case 2:
      return 0;
    case 3:
    case 4:
      return 1;
    case 5:
      return 2;
    case 6:
      return 3;
    case 7:
      return 5;
    default:
      return 11;
  }
}

#endif