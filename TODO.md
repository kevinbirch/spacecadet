
* type specific macros for pattern functions
  * https://github.com/rxi/map
* bad examples: https://bitbucket.org/manvscode/libcollections/src
* good examples: http://opensource.apple.com/source/CF/
* marginal examples: http://sourceforge.net/p/c-algorithms/code/ci/master/tree/
* good api: http://cairographics.org/manual/
* hashtable
  * bug: keys are leaked on calls to `remove()`, return key/value pair instead of just value
  * riterate(_keys, _values)
  * `clear()` should free keys and values
  * `free()` should call clear
  * comparisons to other impls
  * http://preshing.com/20130107/this-hash-table-is-faster-than-a-judy-array/
* add murmur3 hash
* vector
  * switch insert to reallocate
  * capacity: http://grepcode.com/file/repository.grepcode.com/java/root/jdk/openjdk/6-b14/java/util/ArrayList.java#ArrayList.ensureCapacity%28int%29
  * riterate
  * rfind
  * rcontains
  * rany
  * rmap, rmap_into
  * rreduce
  * fill,fill_range
  * delete range
  * reverse/reverse_copy
  * sort/sort_copy
  * shuffle/shuffle_copy
  * sublist
  * tail
  * slice
  * take, drop
  * take\_while, drop\_while
  * with_values?
  * add_values?
  * remove_if?
* t-vector - typed vector
  * array of typed (sized) objects, not pointers
  * use copy semantics
  * use stack instances, not pointers
  * use handles for reference
* primitive map, set, vector, list
  * https://github.com/DRMacIver/intmap
  * http://hackage.haskell.org/package/containers-0.5.3.1/docs/Data-IntMap.html
* set, bloom filter, count min sketch
* list, dl list
* string
  * http://bstring.cvs.sourceforge.net/viewvc/bstring/tree/bstrlib.txt?pathrev=HEAD
  * http://pastebin.com/CuzYMSxE
* bitset
  * http://roaringbitmap.org/
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
* cycle detection
  * http://en.wikipedia.org/wiki/Cycle_detection#Brent.27s_algorithm
* b/b+ tree
  * https://code.google.com/p/high-concurrency-btree/
  * http://www.codeproject.com/Articles/316816/RaptorDB-The-Key-Value-Store-V2
  * http://bartoszmilewski.com/2013/11/25/functional-data-structures-in-c-trees/
  * http://www.tokutek.com/wp-content/uploads/2012/11/Fractal-Tree-Tech-and-the-Art-of-Indexing.mov
  * https://en.wikipedia.org/wiki/Log-structured_merge-tree
* garbage collector
  * http://web.engr.illinois.edu/~maplant2/gc.html
