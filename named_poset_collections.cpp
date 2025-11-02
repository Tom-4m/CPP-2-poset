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
// Type representing a relation as an adjacency matrix
using Relation = std::array<std::bitset<N>, N>;

// Type representing a partially ordered set
struct Poset {
    Relation relation;

    Poset() {
        // Initialize with reflexive relation
        for (size_t i = 0; i < N; ++i) {
            relation[i].set(i);
        }
    }
};

// Type representing a collection of partially ordered sets
using PosetCollection = std::map<std::string, Poset>;

// Map of all collections
using CollectionsMap = std::map<long, PosetCollection>;

// Function returning static collections map (SIOF solution)
CollectionsMap &get_collections() {
    static CollectionsMap collections;
    return collections;
}

// ID counter
long &get_next_id() {
    static long next_id = 0;
    return next_id;
}

// Cache for iterator results to prevent dangling pointers
std::string &get_iterator_cache() {
    static std::string cache;
    return cache;
}

// Finds a collection by ID
PosetCollection *find_collection(long id) {
    auto &collections = get_collections();
    const auto it = collections.find(id);

    return it == collections.end() ? nullptr : &it->second;
}

// Validates name correctness
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

// Transitive closure of relation
void transitive_closure(Relation &rel) {
    for (size_t k = 0; k < N; ++k) {
        for (size_t i = 0; i < N; ++i) {
            if (rel[i][k]) {
                rel[i] |= rel[k];
            }
        }
    }
}

// Check if relation satisfies antisymmetry
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

    // Check for overflow before incrementing
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

    const std::string name_str(name);
    auto &collection = *collection_ptr;

    if (collection.find(name_str) != collection.end()) {
        return false;
    }

    collection[name_str] = Poset();
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

    collection_ptr->erase(std::string(name));
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
    const std::string src_str(name_src);
    const auto src_it = collection.find(src_str);

    if (src_it == collection.end()) {
        return false;
    }

    const std::string dst_str(name_dst);
    collection[dst_str] = src_it->second;
    return true;
}

char const *npc_first_poset(long id) {
    const auto collection_ptr = find_collection(id);
    if (collection_ptr == nullptr || collection_ptr->empty()) {
        return nullptr;
    }

    // Cache the string to prevent dangling pointer
    get_iterator_cache() = collection_ptr->begin()->first;
    return get_iterator_cache().c_str();
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
    const std::string name_str(name);
    auto poset_it = collection.find(name_str);

    if (poset_it  == collection.end()) {
        return nullptr;
    }

    ++poset_it ;
    if (poset_it  == collection.end()) {
        return nullptr;
    }

    // Cache the string to prevent dangling pointer
    get_iterator_cache() = poset_it->first;
    return get_iterator_cache().c_str();
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
    const std::string name_str(name);
    const auto poset_it = collection.find(name_str);

    if (poset_it == collection.end()) {
        return false;
    }

    auto &rel = poset_it->second.relation;

    // Relation already exists
    if (rel[x][y]) {
        return false;
    }

    // Simulate adding relation and transitive closure
    Relation temp_rel = rel;
    temp_rel[x].set(y);
    transitive_closure(temp_rel);

    // Check if the result would be antisymmetric
    if (!is_antisymmetric(temp_rel)) {
        return false;
    }

    // Accept changes
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
    const std::string name_str(name);
    const auto poset_it = collection.find(name_str);

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
    const std::string name_str(name);
    const auto poset_it = collection.find(name_str);

    if (poset_it == collection.end()) {
        return false;
    }

    auto &rel = poset_it->second.relation;

    if (!rel[x][y]) {
        return false;
    }

    // Check if intermediate element exists
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