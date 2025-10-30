# CPP-2-poset
## Struktura projektu

| Plik | Opis |
|------|------|
| `named_poset_collections.h` | Nagłówek modułu; deklaracje funkcji C/C++ i przestrzeń nazw `cxx`. |
| `named_poset_collections.cpp` | Implementacja funkcji modułu. |
| `named_poset_collections_example_1.c` | Przykład użycia modułu w języku C. |
| `named_poset_collections_example_2.cpp` | Przykład użycia modułu w języku C++. |
| `main.cpp` | Opcjonalny plik testowy / demonstracyjny (jeśli istnieje). |

## Kompilacja

```bash
# Kompilacja modułu C++
g++ -c -Wall -Wextra -O2 -std=c++23 named_poset_collections.cpp -o named_poset_collections_32.o
g++ -c -Wall -Wextra -O2 -std=c++23 -DN=64 named_poset_collections.cpp -o named_poset_collections_64.o

# Kompilacja przykładu w C
gcc -c -Wall -Wextra -O2 -std=c23 -I. named_poset_collections_example_1.c -o named_poset_collections_example_1.o

# Kompilacja przykładu w C++
g++ -c -Wall -Wextra -O2 -std=c++23 -I. named_poset_collections_example_2.cpp -o named_poset_collections_example_2.o

# Linkowanie plików obiektowych
g++ named_poset_collections_example_1.o named_poset_collections_32.o -o named_poset_collections_example_1
g++ named_poset_collections_example_2.o named_poset_collections_64.o -o named_poset_collections_example_2_a
g++ named_poset_collections_64.o named_poset_collections_example_2.o -o named_poset_collections_example_2_b
