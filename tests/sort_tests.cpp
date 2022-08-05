#include "algorithms/sorting/bubble_sort.hpp"
#include "algorithms/sorting/bucket_sort.hpp"
#include "algorithms/sorting/merge_sort.hpp"
#include "algorithms/sorting/quick_sort.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <random>

using namespace testing;
using namespace algorithms::sorting;

template <class T> using sort_function = std::function<void(std::vector<T>&)>;

template <class T> class SortTest : public TestWithParam<sort_function<T>>
{
    using Super = TestWithParam<sort_function<T>>;

  protected:
    void sort(std::vector<T>& values) { Super::GetParam()(values); }
};

class SortIntegers : public SortTest<int>
{
};

TEST_P(SortIntegers, EmptySet)
{
    std::vector<int> values;
    sort(values);

    ASSERT_THAT(values, IsEmpty());
}

TEST_P(SortIntegers, OneValue)
{
    std::vector<int> values {0};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0));
}

TEST_P(SortIntegers, TwoValuesSame)
{
    std::vector<int> values {0, 0};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 0));
}

TEST_P(SortIntegers, TwoValuesSorted)
{
    std::vector<int> values {0, 1};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 1));
}

TEST_P(SortIntegers, TwoValuesUnsorted)
{
    std::vector<int> values {1, 0};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 1));
}

TEST_P(SortIntegers, ThreeValuesSame)
{
    std::vector<int> values {0, 0, 0};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 0, 0));
}

TEST_P(SortIntegers, ThreeValuesSorted)
{
    std::vector<int> values {0, 1, 2};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 1, 2));
}

TEST_P(SortIntegers, ThreeValuesUnsorted)
{
    std::vector<int> values {1, 0, 2};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 1, 2));
}

TEST_P(SortIntegers, TenValuesSame)
{
    std::vector<int> values {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
}

TEST_P(SortIntegers, TenValuesSorted)
{
    std::vector<int> values {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
}

TEST_P(SortIntegers, TenValuesUnsorted)
{
    std::vector<int> values {1, 0, 2, 5, 9, 3, 8, 7, 6, 4};
    sort(values);

    ASSERT_THAT(values, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
}

TEST_P(SortIntegers, TenValuesSomeDuplicates)
{
    std::vector<int> values {1, 3, 2, 5, 9, 3, 3, 7, 6, 4};
    sort(values);

    ASSERT_THAT(values, ElementsAre(1, 2, 3, 3, 3, 4, 5, 6, 7, 9));
}

TEST_P(SortIntegers, LargeSets)
{
    // Always seed the engine at the start of this so we sort the same
    // sets of values each time.
    std::mt19937_64 reng(0);
    std::uniform_int_distribution dist(0, 10000);

    for (uint32_t i = 0; i < 50u; i++)
    {
        SCOPED_TRACE("Value set " + std::to_string(i));

        std::vector<int> values(100);
        for (int& v : values)
        {
            v = dist(reng);
        }

        auto answer = values;
        std::sort(answer.begin(), answer.end());
        sort(values);

        ASSERT_THAT(values, ElementsAreArray(answer));
    }
}

INSTANTIATE_TEST_SUITE_P(AllSorts, SortIntegers,
                         testing::Values([](std::vector<int>& x) { bubble_sort(x.begin(), x.end()); },
                                         [](std::vector<int>& x) { merge_sort(x.begin(), x.end()); },
                                         [](std::vector<int>& x) { bucket_sort(x.begin(), x.end()); },
                                         [](std::vector<int>& x) { quick_sort(x.begin(), x.end()); }));

TEST(BucketSortStrings, Simple)
{
    std::vector<std::string> values = {"bcde", "abcd", "defg", "cdef"};
    auto sorted_values              = values;

    bucket_sort(values.begin(), values.end());
    std::sort(sorted_values.begin(), sorted_values.end());

    ASSERT_THAT(values, ElementsAreArray(sorted_values));
}

TEST(BucketSortStrings, Duplicates)
{
    std::vector<std::string> values = {"bcde", "abcd", "abcd", "defg", "cdef", "bcde"};
    auto sorted_values              = values;

    bucket_sort(values.begin(), values.end());
    std::sort(sorted_values.begin(), sorted_values.end());

    ASSERT_THAT(values, ElementsAreArray(sorted_values));
}

TEST(BucketSortStrings, Ragged)
{
    std::vector<std::string> values = {"bcdefgh", "abcdefg", "abcd", "defghi", "cdef", "bcdefg"};
    auto sorted_values              = values;

    bucket_sort(values.begin(), values.end());
    std::sort(sorted_values.begin(), sorted_values.end());

    ASSERT_THAT(values, ElementsAreArray(sorted_values));
}

TEST(BucketSortStrings, CommonPrefix)
{
    std::vector<std::string> values = {"ABCD-12345", "ABCD-23456", "ABCD-34567"};
    auto sorted_values              = values;

    bucket_sort(values.begin(), values.end());
    std::sort(sorted_values.begin(), sorted_values.end());

    ASSERT_THAT(values, ElementsAreArray(sorted_values));
}

TEST(BucketSortStrings, CommonPrefixRagged)
{
    std::vector<std::string> values = {"ABCD-12345", "ABCD-2345678", "ABCD-345678"};
    auto sorted_values              = values;

    bucket_sort(values.begin(), values.end());
    std::sort(sorted_values.begin(), sorted_values.end());

    ASSERT_THAT(values, ElementsAreArray(sorted_values));
}
