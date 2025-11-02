#include "named_poset_collections.h"
#include <map>
#include <string>
#include <bitset>
#include <array>
#include <climits>

#ifndef N
#define N 32
#endif

namespace {
// Adjacency matrix representation of a relation.
using Relation = std::array<std::bitset<N>, N>;

// A partially ordered set with its relation.
struct Poset {
    Relation relation;

    Poset() {
        // Reflexive relation: (i, i) for all i.
        for (size_t i = 0; i < N; ++i) {
            relation[i].set(i);
        }
    }
};

// Collection maps poset names to their data.
// std::map guarantees that pointers to keys remain stable
using PosetCollection = std::map<std::string, Poset>;

// Global storage for all collections.
using CollectionsMap = std::map<long, PosetCollection>;

// Avoids static initialization order fiasco.
CollectionsMap &get_collections() {
    static CollectionsMap collections;
    return collections;
}

// Generates unique collection identifiers.
long &get_next_id() {
    static long next_id = 0;
    return next_id;
}

// Returns pointer to collection or nullptr if not found.
PosetCollection *find_collection(long id) {
    auto &collections = get_collections();
    const auto it = collections.find(id);
    return it == collections.end() ? nullptr : &it->second;
}

// Checks if name contains only allowed characters: [a-zA-Z0-9_].
bool is_valid_name(char const *name) {
    if (name == nullptr || name[0] == '\0') {
        return false;
    }

    for (size_t i = 0; name[i] != '\0'; ++i) {
        char c = name[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_')) {
            return false;
        }
    }

    return true;
}

// Computes transitive closure using Floyd-Warshall algorithm.
void transitive_closure(Relation &rel) {
    for (size_t k = 0; k < N; ++k) {
        for (size_t i = 0; i < N; ++i) {
            if (rel[i][k]) {
                rel[i] |= rel[k];
            }
        }
    }
}

// Verifies antisymmetry: if (i,j) and (j,i) exist, then i == j.
bool is_antisymmetric(const Relation &rel) {
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            if (rel[i][j] && rel[j][i]) {
                return false;
            }
        }
    }
    return true;
}
} // namespace

extern "C" {
namespace cxx {

long npc_new_collection() {
    long &next_id = get_next_id();

    if (next_id == LONG_MAX) {
        return -1;
    }

    long id = next_id++;
    get_collections()[id] = PosetCollection();
    return id;
}

void npc_delete_collection(long id) {
    get_collections().erase(id);
}

bool npc_new_poset(long id, char const *name) {
    if (!is_valid_name(name)) {
        return false;
    }

    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return false;
    }

    auto &collection = *collection_ptr;

    // Poset name must be unique within collection.
    if (collection.find(name) != collection.end()) {
        return false;
    }

    collection[name] = Poset();
    return true;
}

void npc_delete_poset(long id, char const *name) {
    if (name == nullptr) {
        return;
    }

    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return;
    }

    collection_ptr->erase(name);
}

bool npc_copy_poset(long id, char const *name_dst, char const *name_src) {
    if (!is_valid_name(name_dst) || name_src == nullptr) {
        return false;
    }

    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return false;
    }

    auto &collection = *collection_ptr;
    const auto src_it = collection.find(name_src);

    if (src_it == collection.end()) {
        return false;
    }

    // Overwrites destination if it already exists.
    collection[name_dst] = src_it->second;
    return true;
}

char const *npc_first_poset(long id) {
    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr || collection_ptr->empty()) {
        return nullptr;
    }

    // Safe: string lives in map until element is removed.
    return collection_ptr->begin()->first.c_str();
}

char const *npc_next_poset(long id, char const *name) {
    if (name == nullptr) {
        return nullptr;
    }

    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return nullptr;
    }

    auto &collection = *collection_ptr;
    auto poset_it = collection.find(name);

    if (poset_it == collection.end()) {
        return nullptr;
    }

    ++poset_it;
    if (poset_it == collection.end()) {
        return nullptr;
    }

    // Safe: string lives in map until element is removed.
    return poset_it->first.c_str();
}

bool npc_add_relation(long id, char const *name, size_t x, size_t y) {
    if (name == nullptr || x >= N || y >= N) {
        return false;
    }

    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return false;
    }

    auto &collection = *collection_ptr;
    const auto poset_it = collection.find(name);

    if (poset_it == collection.end()) {
        return false;
    }

    auto &rel = poset_it->second.relation;

    if (rel[x][y]) {
        return false;
    }

    // Test if adding (x,y) preserves antisymmetry after closure.
    Relation temp_rel = rel;
    temp_rel[x].set(y);
    transitive_closure(temp_rel);

    if (!is_antisymmetric(temp_rel)) {
        return false;
    }

    rel = temp_rel;
    return true;
}

bool npc_is_relation(long id, char const *name, size_t x, size_t y) {
    if (name == nullptr || x >= N || y >= N) {
        return false;
    }

    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return false;
    }

    auto &collection = *collection_ptr;
    const auto poset_it = collection.find(name);

    if (poset_it == collection.end()) {
        return false;
    }

    return poset_it->second.relation[x][y];
}

bool npc_remove_relation(long id, char const *name, size_t x, size_t y) {
    if (name == nullptr || x >= N || y >= N || x == y) {
        return false;
    }

    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return false;
    }

    auto &collection = *collection_ptr;
    const auto poset_it = collection.find(name);

    if (poset_it == collection.end()) {
        return false;
    }

    auto &rel = poset_it->second.relation;

    if (!rel[x][y]) {
        return false;
    }

    // Can only remove direct edges (no intermediate path exists).
    for (size_t z = 0; z < N; ++z) {
        if (z != x && z != y && rel[x][z] && rel[z][y]) {
            return false;
        }
    }

    rel[x].reset(y);
    return true;
}

size_t npc_size() {
    return get_collections().size();
}

size_t npc_poset_size() {
    return N;
}

size_t npc_collection_size(long id) {
    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr) {
        return 0;
    }

    return collection_ptr->size();
}
} // namespace cxx
}

#undef N