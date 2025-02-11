### Rules of Boggle

The rules of boggle can be found [here](https://en.wikipedia.org/wiki/Boggle).

### Scoring the Board

To score a board we need to explore all paths through adjacent tiles on the board and check to see if the path corresponds to a word in the given dictionary. Valid paths cannot repeat individual tiles and duplicate words are not allowed. The scoring function needs to be really fast if we want to use local search methods to find a high scoring board. Here are the optimizations I tried.

The ideal data structure to store the dictionary is a [Trie](https://en.wikipedia.org/wiki/Trie). This allows us to walk a path in the board and the Trie at the same time. We can abandon our path and backtrack when the path prefix cannot be extended to a word in the Trie. This makes scoring a path linear in the length of the path. If the dictionary were stored in an array or a set then the time complexity would scale as linear or log in the size of the dictionary.

A nice way to avoid repeated words is to store a flag in each Trie node which can be set to say that a word has been found. If this word is encountered again, the flag tells us not to count it. However, you need to clear these flags between calls to score a board. This can be done efficiently by keeping a list of pointers to these flags. Are more clever approach is to store a timestamp in each node. We keep a global timestamp and we increment it before scoring a board. When a word is found we check it's timestamp, if it's less than the global timestamp, the word is new and we can count it. We then update the node timestamp to the current global timestamp. If we encounter this node again during this board scoring, we know this word was already found and not to count it.

I cached the indexes of each tiles neighboring tiles to avoid computing them in the scoring loop. These are generated and cached before beginning the search. I experimented with stl **vector**, stl **array** and standard C arrays for my scoring loop data structures. The **vectors** were predictably and significantly slower. The **array** was only slightly slower than C style arrays and they produce more readable and organized code so opted for them in some cases with the remaining data structures using C style arrays.

### Steepest Ascent

For my steepest ascent move on a board, I scored every letter change to every tile and took the maximal board. This gives every board a neighborhood of size 25 X 26. Testing with both `dictSmall.txt` and `dictBig.txt` a steepest ascent climb hits it's maximum at around 20 iterations. It rarely continues to improve beyond 30 iterations.

```
hsngdcaiesprtsrseatedmbld	6036
tersnoseigprtndeaiesdclpr	6066
```

I got to boards in the low 6000's and lots of boards in the 5000's. Next I will try to get better boards by messing with these high scoring boards.
