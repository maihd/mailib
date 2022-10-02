#pragma once

#include <assert.h>
#include <string.h>
#include <Memory/Allocations.h>
#include <Container/HashTableTypes.h>

template <typename T>
inline bool HashTable_Init(HashTable<T>* table, Allocator* allocator, int32_t hashCount = 64)
{
    if (table == nullptr || hashCount <= 0)
    {
        return false;
    }

    table->count        = 0;
    table->capacity     = 0;
    table->hashs        = nullptr;
    table->hashCount    = hashCount;
    table->nexts        = nullptr;
    table->keys         = nullptr;
    table->values       = nullptr;
    table->allocator    = allocator;

    return true;
}

template <typename T>
inline HashTable<T>* HashTable_New(Allocator* allocator, int32_t hashCount = 64, bool precreateHashs = true)
{
    int32_t hashsSize = (!!precreateHashs) * (hashCount * sizeof(int32_t));
    int32_t blockSize = sizeof(HashTable<T>) + hashsSize;

    int32_t* hashs = (int32_t*)Memory_Allocate(allocator, nullptr, blockSize, alignof(int32_t));
    HashTable<T>* table = (HashTable<T>*)((uint8_t*)hashs + hashsSize);
    if (HashTable_Init(table, allocator, hashCount))
    {
        Memory_Free(allocator, table);
        return NULL;
    }

    table->hashs = precreateHashs ? hashs : NULL;
    return table;
}

template <typename T>
inline void HashTable_Free(HashTable<T>* table)
{
    Memory_Free(table->allocator, table->nexts);
    Memory_Free(table->allocator, table->hashs);
    HashTable_Init(table, table->allocator, table->hashCount);
}

// Clear the this
template <typename T>
inline void HashTable_Clear(HashTable<T>* table)
{
    table->count = 0;
}

// Unref the this
template <typename T>
inline void HashTable_Detach(HashTable<T>* table)
{
    HashTable_Init(table, table->allocator, table->hashCount);
}

// Find index of entry with key
template <typename T>
int32_t HashTable_IndexOf(const HashTable<T>* table, uint32_t key, int32_t* outHash = nullptr, int32_t* outPrev = nullptr)
{
    if (!table->hashs)
    {
        return -1;
    }

    int32_t hash = (int32_t)(key % (uint32_t)table->hashCount);
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

    if (outHash) *outHash = hash;
    if (outPrev) *outPrev = prev;
    return curr;
}

// Determine if hash table contains the entry with key
template <typename T>
inline bool HashTable_ContainsKey(const HashTable<T>* table, uint32_t key)
{
    return HashTable_IndexOf(table, key) > -1;
}

// Get value of entry with key
template <typename T>
inline const T& HashTable_GetValue(const HashTable<T>* table, uint32_t key)
{
    int32_t curr = HashTable_IndexOf(table, key);
    return table->values[curr];
}

// Get value of entry with key
template <typename T>
inline const T& HashTable_GetValue(const HashTable<T>* table, uint32_t key, const T& defaultValue)
{
    int32_t curr = HashTable_IndexOf(table, key);
    return (curr > -1) ? table->values[curr] : defaultValue;
}

// Get value of entry with key. If entry exists return true, false otherwise.
template <typename T>
inline bool HashTable_TryGetValue(const HashTable<T>* table, uint32_t key, T* outValue)
{
    int32_t curr = HashTable_IndexOf(table, key);
    if (curr > -1)
    {
        *outValue = table->values[curr];
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
inline bool HashTable_GetValueOrNewSlot(HashTable<T>* table, uint32_t key, T** outValueSlot)
{
    int32_t hash, prev;
    int32_t curr = HashTable_IndexOf(table, key, &hash, &prev);

    if (curr < 0)
    {
        if (table->hashs == nullptr)
        {
            const int32_t hashCount = table->hashCount;

            table->hashs = (uint32_t*)Memory_Allocate(table->allocator, hashCount * sizeof(*table->hashs), alignof(uint32_t));
            assert(table->hashs != nullptr && "Out of memory");
            
            // Simple loop, can be unroll by compiler
            for (int32_t i = 0; i < hashCount; i++)
            {
                table->hashs[i] = -1;
            }

            // Re-calculate hash
            prev = -1;
            hash = (int32_t)(key % (uint32_t)hashCount);
        }

        if (table->count + 1 > table->capacity)
        {
            const int32_t oldCapacity = table->capacity;
            const int32_t newCapactiy = oldCapacity > 0 ? oldCapacity * 2 : 32;

            void* newNexts  = Memory_Allocate(table->allocator, table->nexts, newCapactiy * (sizeof(int32_t) + sizeof(uint32_t) + sizeof(T)));
            void* newKeys   = (uint8_t*)newNexts + newCapactiy * sizeof(int32_t);
            void* newValues = (uint8_t*)newKeys + newCapactiy * sizeof(uint32_t);
            assert(newNexts != nullptr && "Out of memory");

            if (!newNexts)
            {
                return false;
            }
            else
            {
                //memmove(newNexts, table->nexts, oldCapacity * sizeof(int32_t));
                memmove(newKeys, table->keys, oldCapacity * sizeof(uint32_t));
                memmove(newValues, table->values, oldCapacity * sizeof(T));
            
                table->capacity = newCapactiy;
                table->nexts    = (int32_t*)newNexts;
                table->keys     = (uint32_t*)newKeys;
                table->values   = (T*)newValues;
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
inline T& HashTable_GetValueOrNewSlot(HashTable<T>* table, uint32_t key)
{
    T* innerValue;
    if (!HashTable_GetValueOrNewSlot(table, key, &innerValue))
    {
        assert(false && "Out of memory.");
    }
    return *innerValue;
}

// Set entry's value, if not exists create new
template <typename T>
inline bool HashTable_SetValue(HashTable<T>* table, uint32_t key, const T& value)
{
    T* innerValue;
    if (HashTable_GetValueOrNewSlot(table, key, &innerValue))
    {
        *innerValue = value;
        return true;
    }
    else
    {
        return false;
    }
}

// Remove the entry at given index, hash entry, and previous entry
template <typename T>
bool HashTable_Erase(HashTable<T>* table, int32_t curr, int32_t hash, int32_t prev)
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

            HashTable_IndexOf(table, table->keys[curr], &hash, &prev);
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
inline bool HashTable_Remove(HashTable<T>* table, uint32_t key)
{
    int32_t prev;
    int32_t hash;
    int32_t curr = HashTable_IndexOf(table, key, &hash, &prev);
    return HashTable_Erase(curr, hash, prev);
}

// Remove the entry at given index
template <typename T>
inline bool HashTable_Erase(HashTable<T>* table, int32_t index)
{
    if (index > -1 && index < table->count)
    {
        return HashTable_Remove(table->keys[index]);
    }
    else
    {
        return false;
    }
}

//! LEAVE AN EMPTY LINE HERE, REQUIRE BY GCC/G++
