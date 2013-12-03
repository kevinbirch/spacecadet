
* bad examples: https://bitbucket.org/manvscode/libcollections/src
* good examples: http://opensource.apple.com/source/CF/
* marginal examples: http://sourceforge.net/p/c-algorithms/code/ci/master/tree/
* hashtable
  * bug: keys are leaked on calls to `remove()`, return key/value pair instead of just value
  * `clear()` should free keys and values
  * `free()` should call clear
* add murmur3 hash
* vector
  * filter/filter_not
  * remove_if
  * reverse/reverse_copy
  * sort/sort_copy
  * shuffle/shuffle_copy
  * fill,fill_range
  * delete range
  * sublist
  * tail
  * slice
  * take, drop
  * take\_while, drop\_while
  * with_values?
  * add_values?
* set, bloom filter, count min sketch
* list, dl list
* string
* bitset
* queue, deque, ring
* trie, burst trie
* heap, priority queue
* arena, zone, slab allocator
* logging
* hash array mapped tree
* linear probing hashtable impl?
* improved linenoise fork
* flexible printf?
* rv style message broker?
