#include "dict.h"

#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

int Node::g_timestamp = 0;
int letter_counts[ALPHABET_SIZE] = {[0 ... 24] = 0};

Node::Node() : timestamp(0), is_word(false) {
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    children[i] = NULL;
  }
}

void Node::add_word(const string &s, int i) {
  // end of the word
  if (i == s.length()) {
    is_word = true;
  } else {
    int letter_id = IDX(s[i]);
    if (children[letter_id] == NULL) {
      children[letter_id] = new Node;
    }
    children[letter_id]->add_word(s, ++i);
  }
}

void Node::print_words() const {
  string prefix;
  print_words(prefix);
}

void Node::print_words(string prefix) const {
  if (is_word && Node::g_timestamp > 0) {
    cout << prefix << endl;
  }
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (children[i] != NULL) {
      string new_prefix = prefix;
      new_prefix.push_back(CHR(i));
      children[i]->print_words(new_prefix);
    }
  }
}

bool Node::lookup_word(const string &s, int i) const {
  if (i == s.length()) {
    return is_word;
  } else {
    int letter_id = IDX(s[i]);
    if (children[letter_id] == NULL) {
      return false;
    }

    return children[letter_id]->lookup_word(s, ++i);
  }
}

// This function is the inner loop so I've spent some time
// optimizing it. Readability is sacrificed some for speed.
// Using a pointer to walk neighbors instead of array
// index is slightly faster. Not sure if it warrants the
// reduced readability.
bool used_tiles[25] = {[0 ... 24] = false};
int Node::score_tile(const Board &b, int tile, int depth) {
  int score = 0;
  if (is_word && timestamp != Node::g_timestamp) {
    score += word_score(depth);
    timestamp = Node::g_timestamp;
  }

  used_tiles[tile] = true;
  for (int *npos = neighbors[tile]; *npos != -1; ++npos) {
    if (used_tiles[*npos]) continue;
    if (children[b[*npos]] != NULL) {
      score += children[b[*npos]]->score_tile(b, *npos, depth + 1);
    }
  }
  used_tiles[tile] = false;

  return score;
}

Node::~Node() {
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (children[i] != NULL) delete children[i];
  }
}

string clean_string(string &s) {
  string clean;
  if (s.length() < 3) return clean;
  for (size_t i = 0; i < s.length(); i++) {
    if (!isalpha(s[i])) return string();
    clean.push_back(tolower(s[i]));
  }
  return clean;
}

bool read_dictionary(Node &root, char *dict_file) {
  string dict_word;
  fstream dictionary(dict_file);
  if (!dictionary) {
    cerr << "no dictionary file" << endl;
    return false;
  }
  while (getline(dictionary, dict_word)) {
    dict_word = clean_string(dict_word);
    if (dict_word.length() > 0) root.add_word(dict_word, 0);
  }
  return true;
}
