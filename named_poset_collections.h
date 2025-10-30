#ifndef NAMED_POSET_COLLECTIONS_H
#define NAMED_POSET_COLLECTIONS_H

#include <cstddef>

#ifdef __cplusplus
#include <cstdbool>
namespace cxx {
extern "C" {
#else
#include <stdbool.h>
#endif

// Tworzy nową, pustą kolekcję nazwanych zbiorów częściowo uporządkowanych
long npc_new_collection();

// Usuwa kolekcję o podanym identyfikatorze
void npc_delete_collection(long id);

// Tworzy nowy zbiór częściowo uporządkowany w kolekcji
bool npc_new_poset(long id, char const *name);

// Usuwa zbiór częściowo uporządkowany z kolekcji
void npc_delete_poset(long id, char const *name);

// Kopiuje zbiór częściowo uporządkowany
bool npc_copy_poset(long id, char const *name_dst, char const *name_src);

// Zwraca nazwę pierwszego zbioru w kolekcji
char const *npc_first_poset(long id);

// Zwraca nazwę następnego zbioru w kolekcji
char const *npc_next_poset(long id, char const *name);

// Dodaje relację między elementami
bool npc_add_relation(long id, char const *name, size_t x, size_t y);

// Sprawdza czy istnieje relacja między elementami
bool npc_is_relation(long id, char const *name, size_t x, size_t y);

// Usuwa relację między elementami
bool npc_remove_relation(long id, char const *name, size_t x, size_t y);

// Zwraca liczbę istniejących kolekcji
size_t npc_size();

// Zwraca rozmiar zbioru częściowo uporządkowanego (N)
size_t npc_poset_size();

// Zwraca liczbę zbiorów w kolekcji
size_t npc_collection_size(long id);

#ifdef __cplusplus
}
}
#endif

#endif // NAMED_POSET_COLLECTIONS_H