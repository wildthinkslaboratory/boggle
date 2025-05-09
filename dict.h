#ifndef DICT
#define DICT

#include "board.h"
#include "util.h"

const int ALPHABET_SIZE = 26;

// we store the dictionary in a Trie made of nodes
// children nodes are indexed by alphabet letters
// encoded as ints.

// We increment a global timestamp g_timestamp each
// time we begin a search. When we find a word, we
// can  check to see if we've encountered this word
// before on this search by checking the node's local
// timestamp and comparing it with the global timestamp.
class Node {
 public:
  Node *children[ALPHABET_SIZE];
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

bool read_dictionary(Node &root, char *dict_file);

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