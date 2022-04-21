#include "ExcaliburHash.h"
#include "gtest/gtest.h"
#include <array>

TEST(SmFlatHashMap, EmpltyValuesTest)
{
    // use hash table as map (no values stored at all)
    Excalibur::HashTable<int, nullptr_t> ht;
    EXPECT_TRUE(ht.empty());

    const int kNumElements = 99999;
    for (int i = 1; i < kNumElements; i++)
    {
        auto it = ht.emplace(i);
        EXPECT_TRUE(it.second);
    }
    EXPECT_FALSE(ht.empty());

    for (int i = 1; i < kNumElements; i++)
    {
        bool pos = ht.has(i);
        EXPECT_TRUE(pos);

        bool neg = ht.has(-i);
        EXPECT_FALSE(neg);
    }

    for (int i = 1; i < kNumElements; i++)
    {
        auto it = ht.emplace(i);
        EXPECT_FALSE(it.second);
    }

    for (uint32_t i = 1; i < kNumElements; i++)
    {
        bool isErased = ht.erase(i);
        EXPECT_TRUE(isErased);
    }
    EXPECT_TRUE(ht.empty());
}

TEST(SmFlatHashMap, BasicTest)
{
    // empty hash table
    Excalibur::HashTable<int, int> ht;
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_EQ(ht.capacity(), 0u);

    // emplace elements
    const uint32_t kNumElements = 99999;
    for (uint32_t i = 0; i < kNumElements; i++)
    {
        int k = 256 * i + 1;
        int v = 3 + i;
        auto it = ht.emplace(k, v);
        EXPECT_TRUE(it.second);
    }
    EXPECT_FALSE(ht.empty());
    EXPECT_EQ(ht.size(), kNumElements);
    EXPECT_GE(ht.capacity(), kNumElements);

    // check if those elements exists and have expected value
    for (uint32_t i = 0; i < kNumElements; i++)
    {
        int k = 256 * i + 1;

        auto htVal = ht.find(k);
        ASSERT_NE(htVal, ht.iend());

        int refVal = 3 + i;
        EXPECT_EQ(htVal.value(), refVal);
    }

    // try to emplace the same keys
    for (uint32_t i = 0; i < kNumElements; i++)
    {
        int k = 256 * i + 1;
        auto it = ht.emplace(k, -13);
        EXPECT_FALSE(it.second);

        ASSERT_NE(it.first, nullptr);
        int refVal = 3 + i;
        EXPECT_EQ(*it.first, refVal);
    }
    EXPECT_FALSE(ht.empty());
    EXPECT_EQ(ht.size(), kNumElements);
    EXPECT_GE(ht.capacity(), kNumElements);

    // check if those elements exists and have expected value and then remove then
    for (uint32_t i = 0; i < kNumElements; i++)
    {
        int k = 256 * i + 1;

        auto htVal = ht.find(k);
        ASSERT_NE(htVal, ht.iend());

        int refVal = 3 + i;
        EXPECT_EQ(htVal.value(), refVal);

        bool isErased = ht.erase(k);
        EXPECT_TRUE(isErased);
    }
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_GE(ht.capacity(), 0u);

    ht.emplace(13, 6);
    EXPECT_FALSE(ht.empty());
    EXPECT_EQ(ht.size(), 1u);
    EXPECT_GE(ht.capacity(), 1u);

    ht.clear();
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_GE(ht.capacity(), 0u);

    // clear for hash table that already empty
    ht.clear();
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_GE(ht.capacity(), 0u);
}

TEST(SmFlatHashMap, EmptyHash)
{
    Excalibur::HashTable<int, int> ht;
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_EQ(ht.capacity(), 0u);

    auto v0 = ht.find(0);
    EXPECT_EQ(v0, ht.iend());

    auto v1 = ht.find(13);
    EXPECT_EQ(v1, ht.iend());

    bool e0 = ht.erase(0);
    EXPECT_FALSE(e0);

    bool e1 = ht.erase(13);
    EXPECT_FALSE(e1);
}

TEST(SmFlatHashMap, IteratorTest)
{
    Excalibur::HashTable<int, int> ht;
    EXPECT_TRUE(ht.empty());
    EXPECT_EQ(ht.size(), 0u);
    EXPECT_EQ(ht.capacity(), 0u);

    const int kNumElements = 1333;
    int64_t valuesSum = 0;
    int64_t keysSum = 0;
    for (int i = 0; i < kNumElements; i++)
    {
        int key = i;
        int value = -(key * 2 + key);
        ht.emplace(key, value);
        keysSum += key;
        valuesSum += value;
    }

    // iterators syntax like in Python

    // default (key) iterator
    int64_t keysSumTest = 0;
    for (const int& key : ht)
    {
        keysSumTest += key;
    }
    EXPECT_EQ(keysSum, keysSumTest);

    // keys() iterator
    int64_t keysSumTest2 = 0;
    for (const int& key : ht.keys())
    {
        keysSumTest2 += key;
    }
    EXPECT_EQ(keysSum, keysSumTest2);

    // values() iterator
    int64_t valuesSumTest = 0;
    for (const int& value : ht.values())
    {
        valuesSumTest += value;
    }
    EXPECT_EQ(valuesSum, valuesSumTest);

    // items() iterator
    int64_t keysSumTest3 = 0;
    int64_t valuesSumTest2 = 0;
    for (const auto& [key, value] : ht.items())
    {
        keysSumTest3 += key;
        valuesSumTest2 += value;
    }
    EXPECT_EQ(keysSum, keysSumTest3);
    EXPECT_EQ(valuesSum, valuesSumTest2);

    // complex iterator check
    std::array<uint8_t, kNumElements> visited;
    visited.fill(0);
    for (auto it = ht.ibegin(); it != ht.iend(); ++it)
    {
        int key = it.key();
        ASSERT_GE(key, 0);
        ASSERT_LT(key, int(visited.size()));

        EXPECT_EQ(visited[key], 0);
        visited[key] = 1;

        int val = it.value();
        int refValue = -(key * 2 + key);
        EXPECT_EQ(val, refValue);
    }

    for (uint8_t v : visited)
    {
        EXPECT_EQ(v, 1);
    }
}

struct Bar
{
    int v;
};

namespace Excalibur
{
template <> struct KeyInfo<Bar>
{
    static inline Bar getEmpty() noexcept { return Bar{std::numeric_limits<int>::min()}; }
    static uint64_t hash(const Bar& key) noexcept { return std::hash<int>{}(key.v); }
    static bool isEqual(const Bar& lhs, const Bar& rhs) noexcept { return lhs.v == rhs.v; }
};
} // namespace Excalibur

TEST(SmFlatHashMap, IteratorTestEdgeCases)
{
    Excalibur::HashTable<Bar, nullptr_t> ht;
    EXPECT_TRUE(ht.empty());
    {
        auto it = ht.begin();
        EXPECT_EQ(it, ht.end());
    }

    const int kNumElements = 378;
    int64_t keysSum = 0;
    for (int i = 0; i < kNumElements; i++)
    {
        int kv = i * 3 + 7;
        ht.emplace(Bar{kv});
        keysSum += kv;
    }

    int64_t keysSumTestA = 0;
    int64_t keysSumTestB = 0;
    for (auto it = ht.begin(); it != ht.end(); it++)
    {
        const Bar& itv = *it;
        keysSumTestA += itv.v;
        keysSumTestB += it->v;
    }
    EXPECT_EQ(keysSumTestA, keysSum);
    EXPECT_EQ(keysSumTestB, keysSum);

    Excalibur::HashTable<int, Bar> ht2;
    EXPECT_TRUE(ht2.empty());
    int64_t keysSum2 = 0;
    int64_t valSum2 = 0;
    for (int i = 0; i < kNumElements; i++)
    {
        int key = i * 3 + 7;
        int val = i * 1 + 13;
        ht2.emplace(key, Bar{val});
        keysSum2 += key;
        valSum2 += val;
    }

    int64_t keysSumTestA2 = 0;
    int64_t keysSumTestB2 = 0;
    int64_t valuesSumTestA2 = 0;
    int64_t valuesSumTestB2 = 0;
    for (auto it = ht2.ibegin(); it != ht2.iend(); it++)
    {
        const auto& kvPair = *it;
        keysSumTestA2 += kvPair.first;
        const Bar& itva = kvPair.second;
        valuesSumTestA2 += itva.v;

        keysSumTestB2 += it->first;
        const Bar& itvb = it->second;
        valuesSumTestB2 += itvb.v;
    }
    EXPECT_EQ(keysSumTestA2, keysSum2);
    EXPECT_EQ(keysSumTestB2, keysSum2);
    EXPECT_EQ(valuesSumTestA2, valSum2);
    EXPECT_EQ(valuesSumTestB2, valSum2);

    int64_t valuesSumTestA3 = 0;
    int64_t valuesSumTestB3 = 0;
    for (auto it = ht2.vbegin(); it != ht2.vend(); it++)
    {
        const Bar& itv = *it;
        valuesSumTestA3 += itv.v;
        valuesSumTestB3 += it->v;
    }
    EXPECT_EQ(valuesSumTestA3, valSum2);
    EXPECT_EQ(valuesSumTestB3, valSum2);
}