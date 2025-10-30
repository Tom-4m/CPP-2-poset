#ifndef NAMED_POSET_COLLECTIONS_H
#define NAMED_POSET_COLLECTIONS_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdbool>

extern "C" {
namespace cxx {
#else
#include <stddef.h>
#include <stdbool.h>
#endif

// Creates a new, empty collection of named partially ordered sets
long npc_new_collection();

// Deletes a collection with the given identifier
void npc_delete_collection(long id);

// Creates a new partially ordered set in the collection
bool npc_new_poset(long id, char const *name);

// Deletes a partially ordered set from the collection
void npc_delete_poset(long id, char const *name);

// Copies a partially ordered set
bool npc_copy_poset(long id, char const *name_dst, char const *name_src);

// Returns the name of the first set in the collection
char const *npc_first_poset(long id);

// Returns the name of the next set in the collection
char const *npc_next_poset(long id, char const *name);

// Adds a relation between elements
bool npc_add_relation(long id, char const *name, size_t x, size_t y);

// Checks if a relation exists between elements
bool npc_is_relation(long id, char const *name, size_t x, size_t y);

// Removes a relation between elements
bool npc_remove_relation(long id, char const *name, size_t x, size_t y);

// Returns the number of existing collections
size_t npc_size();

// Returns the size of the partially ordered set (N)
size_t npc_poset_size();

// Returns the number of sets in the collection
size_t npc_collection_size(long id);

#ifdef __cplusplus
}
}
#endif

#endif // NAMED_POSET_COLLECTIONS_H