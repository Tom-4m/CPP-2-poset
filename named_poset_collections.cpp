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
} // namespace

namespace cxx {
extern "C" {

long npc_new_collection() {
    long &next_id = get_next_id();

    if (next_id > LONG_MAX) {
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

    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return false;
    }

    std::string name_str(name);
    auto &collection = coll_it->second;

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

    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return;
    }

    coll_it->second.erase(std::string(name));
}

bool npc_copy_poset(long id, char const *name_dst, char const *name_src) {
    if (!is_valid_name(name_dst) || name_src == nullptr) {
        return false;
    }

    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return false;
    }

    auto &collection = coll_it->second;
    std::string src_str(name_src);
    auto src_it = collection.find(src_str);

    if (src_it == collection.end()) {
        return false;
    }

    std::string dst_str(name_dst);
    collection[dst_str] = src_it->second;
    return true;
}

char const *npc_first_poset(long id) {
    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end() || coll_it->second.empty()) {
        return nullptr;
    }

    return coll_it->second.begin()->first.c_str();
}

char const *npc_next_poset(long id, char const *name) {
    if (name == nullptr) {
        return nullptr;
    }

    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return nullptr;
    }

    auto &collection = coll_it->second;
    std::string name_str(name);
    auto it = collection.find(name_str);

    if (it == collection.end()) {
        return nullptr;
    }

    ++it;
    if (it == collection.end()) {
        return nullptr;
    }

    return it->first.c_str();
}

bool npc_add_relation(long id, char const *name, size_t x, size_t y) {
    if (name == nullptr || x >= N || y >= N) {
        return false;
    }

    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return false;
    }

    auto &collection = coll_it->second;
    std::string name_str(name);
    auto poset_it = collection.find(name_str);

    if (poset_it == collection.end()) {
        return false;
    }

    auto &rel = poset_it->second.relation;

    if (rel[x][y]) {
        return false;
    }

    rel[x].set(y);
    transitive_closure(rel);
    return true;
}

bool npc_is_relation(long id, char const *name, size_t x, size_t y) {
    if (name == nullptr || x >= N || y >= N) {
        return false;
    }

    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return false;
    }

    auto &collection = coll_it->second;
    std::string name_str(name);
    auto poset_it = collection.find(name_str);

    if (poset_it == collection.end()) {
        return false;
    }

    return poset_it->second.relation[x][y];
}

bool npc_remove_relation(long id, char const *name, size_t x, size_t y) {
    if (name == nullptr || x >= N || y >= N || x == y) {
        return false;
    }

    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return false;
    }

    auto &collection = coll_it->second;
    std::string name_str(name);
    auto poset_it = collection.find(name_str);

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
    auto &collections = get_collections();
    auto coll_it = collections.find(id);

    if (coll_it == collections.end()) {
        return 0;
    }

    return coll_it->second.size();
}
}
} // namespace cxx