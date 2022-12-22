#pragma once

#include <assert.h>
#include <string.h>
#include <memory/alloc.h>
#include <container/hash_table_types.h>

template <typename T>
inline bool hash_table_init(HashTable<T>* table, Allocator* allocator, int32_t hash_count = 64)
{
    if (table == nullptr || hash_count <= 0)
    {
        return false;
    }

    table->count        = 0;
    table->capacity     = 0;
    table->hashs        = nullptr;
    table->hash_count   = hash_count;
    table->nexts        = nullptr;
    table->keys         = nullptr;
    table->values       = nullptr;
    table->allocator    = allocator;

    return true;
}

template <typename T>
inline HashTable<T>* hash_table_new(Allocator* allocator, int32_t hash_count = 64, bool precreate_hashs = true)
{
    int32_t hashs_size = (!!precreate_hashs) * (hash_count * sizeof(int32_t));
    int32_t block_size = sizeof(HashTable<T>) + hashs_size;

    int32_t* hashs = (int32_t*)memory_alloc(allocator, nullptr, block_size, alignof(int32_t));
    HashTable<T>* table = (HashTable<T>*)((uint8_t*)hashs + hashs_size);
    if (hash_table_init(table, allocator, hash_count))
    {
        memory_free(allocator, table);
        return NULL;
    }

    table->hashs = precreate_hashs ? hashs : NULL;
    return table;
}

template <typename T>
inline void hash_table_free(HashTable<T>* table)
{
    memory_free(table->allocator, table->nexts);
    memory_free(table->allocator, table->hashs);
    hash_table_init(table, table->allocator, table->hash_count);
}

// Clear the this
template <typename T>
inline void hash_table_clear(HashTable<T>* table)
{
    table->count = 0;
}

// Unref the this
template <typename T>
inline void hash_table_detach(HashTable<T>* table)
{
    hash_table_init(table, table->allocator, table->hash_count);
}

// Find index of entry with key
template <typename T>
int32_t hash_table_index_of(const HashTable<T>* table, uint32_t key, int32_t* out_hash = nullptr, int32_t* out_prev = nullptr)
{
    if (!table->hashs)
    {
        return -1;
    }

    int32_t hash = (int32_t)(key % (uint32_t)table->hash_count);
    int32_t curr = table->hashs[hash];
    int32_t prev = -1;

    while (curr > -1)
    {
        if (table->keys[curr] == key)
        {
            break;
        }

        prev = curr;
        curr = table->nexts[curr];
    }

    if (out_hash) *out_hash = hash;
    if (out_prev) *out_prev = prev;
    return curr;
}

// Determine if hash table contains the entry with key
template <typename T>
inline bool hash_table_contains_key(const HashTable<T>* table, uint32_t key)
{
    return hash_table_index_of(table, key) > -1;
}

// Get value of entry with key
template <typename T>
inline const T& hash_table_get_value(const HashTable<T>* table, uint32_t key)
{
    int32_t curr = hash_table_index_of(table, key);
    return table->values[curr];
}

// Get value of entry with key
template <typename T>
inline const T& hash_table_get_value(const HashTable<T>* table, uint32_t key, const T& default_value)
{
    int32_t curr = hash_table_index_of(table, key);
    return (curr > -1) ? table->values[curr] : default_value;
}

// Get value of entry with key. If entry exists return true, false otherwise.
template <typename T>
inline bool hash_table_try_get_value(const HashTable<T>* table, uint32_t key, T* out_value)
{
    int32_t curr = hash_table_index_of(table, key);
    if (curr > -1)
    {
        *out_value = table->values[curr];
        return true;
    }
    else
    {
        return false;
    }
}

// Get value entry, if not exists create new. 
// Return true if success, false otherwise.
template <typename T>
inline bool hash_table_get_value_or_new_entry(HashTable<T>* table, uint32_t key, T** outValueSlot)
{
    int32_t hash, prev;
    int32_t curr = hash_table_index_of(table, key, &hash, &prev);

    if (curr < 0)
    {
        if (table->hashs == nullptr)
        {
            const int32_t hash_count = table->hash_count;

            table->hashs = (uint32_t*)memory_alloc(table->allocator, hash_count * sizeof(*table->hashs), alignof(uint32_t));
            assert(table->hashs != nullptr && "Out of memory");
            
            // Simple loop, can be unroll by compiler
            for (int32_t i = 0; i < hash_count; i++)
            {
                table->hashs[i] = -1;
            }

            // Re-calculate hash
            prev = -1;
            hash = (int32_t)(key % (uint32_t)hash_count);
        }

        if (table->count + 1 > table->capacity)
        {
            const int32_t old_capacity = table->capacity;
            const int32_t new_capacity = old_capacity > 0 ? old_capacity * 2 : 32;

            void* new_nexts  = memory_alloc(table->allocator, table->nexts, new_capacity * (sizeof(int32_t) + sizeof(uint32_t) + sizeof(T)));
            void* new_keys   = (uint8_t*)new_nexts + new_capacity * sizeof(int32_t);
            void* new_values = (uint8_t*)new_keys + new_capacity * sizeof(uint32_t);
            assert(new_nexts != nullptr && "Out of memory");

            if (!new_nexts)
            {
                return false;
            }
            else
            {
                //memmove(new_nexts, table->nexts, old_capacity * sizeof(int32_t));
                memmove(new_keys, table->keys, old_capacity * sizeof(uint32_t));
                memmove(new_values, table->values, old_capacity * sizeof(T));
            
                table->capacity = new_capacity;
                table->nexts    = (int32_t*)new_nexts;
                table->keys     = (uint32_t*)new_keys;
                table->values   = (T*)new_values;
            }
        }

        curr = table->count;
        if (prev > -1)
        {
            table->nexts[prev] = curr;
        }
        else
        {
            table->hashs[hash] = curr;
        }
        table->nexts[curr] = -1;
        table->keys[curr] = key;
        table->count++;
    }

    *outValueSlot = &table->values[curr];
    return true;
}

// Get value entry, if not exists create new.
// Return a reference to value entry if success, otherwise abort the process.
template <typename T>
inline T& hash_table_get_value_or_new_entry(HashTable<T>* table, uint32_t key)
{
    T* inner_value;
    if (!hash_table_get_value_or_new_entry(table, key, &inner_value))
    {
        assert(false && "Out of memory.");
    }
    return *inner_value;
}

// Set entry's value, if not exists create new
template <typename T>
inline bool hash_table_set_value(HashTable<T>* table, uint32_t key, const T& value)
{
    T* inner_value;
    if (hash_table_get_value_or_new_entry(table, key, &inner_value))
    {
        *inner_value = value;
        return true;
    }
    else
    {
        return false;
    }
}

// Remove the entry at given index, hash entry, and previous entry
template <typename T>
bool hash_table_erase(HashTable<T>* table, int32_t curr, int32_t hash, int32_t prev)
{
    if (curr > -1)
    {
        if (prev > -1)
        {
            table->nexts[prev] = -1;
        }
        else
        {
            table->hashs[hash] = -1;
        }

        if (curr < table->count - 1)
        {
            int32_t last = table->count - 1;
            table->nexts[curr]  = table->nexts[last];
            table->keys[curr]   = table->keys[last];
            table->values[curr] = table->values[last];

            hash_table_index_of(table, table->keys[curr], &hash, &prev);
            if (prev > -1)
            {
                table->nexts[prev] = curr;
            }
            else
            {
                table->hashs[hash] = curr;
            }
        }

        table->count--;
        return true;
    }
    else
    {
        return false;
    }
}

// Remove an entry that has given key
template <typename T>
inline bool hash_table_remove(HashTable<T>* table, uint32_t key)
{
    int32_t prev;
    int32_t hash;
    int32_t curr = hash_table_index_of(table, key, &hash, &prev);
    return hash_table_erase(curr, hash, prev);
}

// Remove the entry at given index
template <typename T>
inline bool hash_table_erase(HashTable<T>* table, int32_t index)
{
    if (index > -1 && index < table->count)
    {
        return hash_table_remove(table->keys[index]);
    }
    else
    {
        return false;
    }
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
