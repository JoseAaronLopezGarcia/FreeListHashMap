# FreeListHashMap
Extremely Fast Hashtable implementation making use of the most advanced collision resolution algorithms; linear probing, coalesced chaining, binary search tree, cuckoo hashing, Hopscotch hashing and ALS hybrid algorithm.

- Linear probing: allows the algorithm to find closeby empty nodes to improve cache coherency.

- Coalesced chaining: auxiliary data structures are formed by linking the table nodes to eachother rather than making use of external structures. This effectively eliminates much of the malloc and pointer overhead from traditional 

- Binary Search Tree: nodes within a collision chain are linked to form a binary tree, allowing for faster resolution of collisions.

- Cuckoo hashing: a newly inserted element will displace an already inserted element when it is known to result in a speedup.

- Hopscotch hashing: this technique limits the number of probing iterations so that it will never go beyond log(table_size).

- ALS: this algorithm was created to allow for the usage of both chaining and probing in the same hashtable.

The name FreeListHashMap comes from the fact that all free nodes are linked together to form a list, allowing the algorithm to always find an empty spot in O(1) time even if linear probing fails.
