### Introduction

Boggle is a game played with a 5 by 5 grid of lettered tiles. Points are scored by finding paths in the letter grid that spell words. The rules of boggle can be found [here](https://en.wikipedia.org/wiki/Boggle). The goal of this code is to find the highest scoring boggle board for a given dictionary of words. The size of the search space is $25^{26}$, so it is too large to check all possible solutions. If we assume a normal sized dictionary, finding the optimal solution is unlikely with current computing resources and algorithms. Proving a solution is optimal is even harder. The best strategies for this kind of problem is a local search method like hill climbing or genetic algorithms. This code implements a steepest ascent hill climbing method to find high scoring boards.

### Scoring the Board

To score a board we need to explore all paths through adjacent tiles on the board and check to see if the path corresponds to a word in the given dictionary. Valid paths cannot repeat individual tiles and duplicate words are not allowed. The scoring function needs to be really fast if we want to use local search methods to find a high scoring board. Here are the optimizations I implemented.

The ideal data structure to store the dictionary is a [Trie](https://en.wikipedia.org/wiki/Trie). This allows us to walk a path in the board and the Trie at the same time. We can abandon our path and backtrack when the path prefix cannot be extended to a word in the Trie. This makes scoring a path linear in the length of the path. If the dictionary were stored in an array or a set then the time complexity would scale as linear or log in the size of the dictionary.

A nice way to avoid repeated words is to store a flag in each Trie node which can be set to say that a word has been found. If this word is encountered again, the flag tells us not to count it. However, you need to clear these flags between calls to score a board. This can be done efficiently by keeping a list of pointers to these flags. A better approach is to store a timestamp in each node. We keep a global timestamp and we increment it before scoring a board. When a word is found we check it's timestamp, if it's less than the global timestamp, the word is new and we can count it. We then update the node timestamp to the current global timestamp. If we encounter this node again during this board scoring, we know this word was already found and we don't count it.

I cached the indexes of each tiles neighboring tiles to avoid computing them in the scoring loop.
These are generated and cached before beginning the search. We keep an array of **bool** to keep track of which tiles are in use
during scoring so that we don't repeat a tile in a path. I experimented with stl **vector**, stl **array** and standard C arrays for my scoring loop data structures. The **vectors** were predictably and significantly slower. The **array** was only slightly slower than C style arrays and they produce more readable and organized code so opted for them in some cases with the remaining data structures using C style arrays.

Next try move Node class to Node struct

### Local Moves and Neighborhoods

I tried a variety of local moves. The standard hill climbing move is to randomly pick a tile and
flip it to a randomly selected letter. We do this until we achieve a better scoring board.
A steepest ascent move looks at all possible tiles/letter flips and takes the highest scoring board.
We also investigated a first improvement flip. It is the same as the steepest ascent
function but stops as soon as an improved board is found rather than investigating all possible neighbors.
As currently implemented, this approach is biased towards tiles and letters with low indexes.
We then ran a series of 200 probes in each style and looked at the average and
standard deviation of the score achieved, number of iterations to maximal score and the time
required. We then took the average score divided by the time per move times the average number of iterations to max.

| stat               | hill climb | best first | steepest ascent |
| ------------------ | ---------- | ---------- | --------------- |
| time per move      | 0.01       | 0.02       | 0.07            |
| score              | 4201       | 4559       | 4118            |
| itertations to max | 75         | 92         | 22              |
| score / time       | 5601       | 2478       | 2674            |

The hill climb approach did the best under this metric. Steepest ascent might be useful for polishing or finishing high scoring boards.

High scoring boards from our probe experiment.

```
hsngdcaiesprtsrseatedmbld	6036
tersnoseigprtndeaiesdclpr	6066
rclpseiaeidntrdgiesanrstb	6016
```

I got three boards in the low 6000's and lots of boards in the 5000's. Next I will try to get better boards by messing with these high scoring boards.

### Can peak solutions be improved by perturbations?

Next we try to improve boards that have hit a maximum through hill climbing or steepest ascent. We perturb a board by flipping a number of tiles randomly.
For each board we try a perturbation and then we use steepest ascent to climb from the perturbed board. We do this whole process a number of times, steepest ascent on a few different perturbations. We then report the improvement for the board.

We'll do this on a set of 72 boards found in the probe experiments. All boards have scores exceeding 5000.

Perturbations followed by hill climbing produced no improvements at all. Perturbations followed by steepest ascent produced significant improvements. If we make 8 random tile changes followed by steepest ascent, then do that 20 times and take the best board. The 72 original boards had an average score of 5294. After perturbing them, they had an average score of 5656 with an average improvement of 326.

We now have quite a lot of boards in the 6000s.

```
dhcrsesaegpltndeaiesmrtsr	SCORE:	6141
gdesgentrnrtaieselpsdapot	SCORE:	6026
dhcrsesaegpltndeaiesmrtsr	SCORE:	6141
rsngseetipdtralnisecgemsd	6028
sogstpinerlatidcstandereg	6038
uoseddplcrieaiesrtndatesg	6044
igdsrseneprtrilaetasdscnp	6058
sedgspinerlatimcertadrsel	6059
gsedgentrirtiamselpedapsr	6092
rstgsdenalliredpatesmsctr	6095
insdiotesgcrtneeaiersplds	6100
rdlprseiaegnrtseatenmscid	6137
rclpseiaeidntrdgietanrssc	6197
dhcrsesaegprtndeaiesbltsr	6221
nsladgitesdnrtrseiaersplb	6229
rttsreaiegplrndseatedchsr	6247
sclpdeiaesdntrpgeitosresu	SCORE:	6606	time:	1086.26
```

### Putting it together in a local search algorithm

It seems important to hit a large number of starting points either by starting lots of different random boards and hill climbing on the or having
substantial perturbation and down hill moves as we go along. Once good boards have been found that are locally maximal, we need to perturb them and then polish them with steepest ascent climbing.

We will start with a large population of boards, which we'll climb on in parallel using our efficient hill climbing moves. Boards that show potential will be moved to a different group for polishing with perturbation and steepest ascent. We'll continue in this fashion moving both groups forward, adding new boards to the general population and shifting promising boards to the polishing group when appropriate.
