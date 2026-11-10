#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <utility>

#include "hash_table.hpp"

struct constant_hash {
    size_t operator()(int) const noexcept {
        return 0;
    }
};

struct modulo_hash {
    size_t operator()(int value) const noexcept {
        return value % 10;
    }
};

struct modulo_equal {
    bool operator()(int left, int right) const noexcept {
        return left % 10 == right % 10;
    }
};

TEST(hash_table, default_ctor) {
    hash_table<int, std::string> table;

    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(table.bucket_count(), 8);
    EXPECT_FLOAT_EQ(table.load_factor(), 0.0f);
    EXPECT_FLOAT_EQ(table.max_load_factor(), 0.9375f);
    EXPECT_FLOAT_EQ(table.min_load_factor(), 0.25f);
}

TEST(hash_table, value_ctor) {
    hash_table<int, std::string> table(16);

    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.bucket_count(), 16);
}

TEST(hash_table, value_ctor_zero_bucket_count) {
    hash_table<int, std::string> table(0);

    EXPECT_EQ(table.bucket_count(), 1);
}

TEST(hash_table, insert_lvalue) {
    hash_table<int, std::string> table;
    const std::pair<const int, std::string> item(1, "one");

    EXPECT_TRUE(table.insert(item));
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.at(1), "one");
}

TEST(hash_table, insert_rvalue) {
    hash_table<int, std::string> table;
    std::pair<const int, std::string> item(1, "one");

    EXPECT_TRUE(table.insert(std::move(item)));
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.at(1), "one");
}

TEST(hash_table, insert_duplicate) {
    hash_table<int, std::string> table;
    const std::pair<const int, std::string> first(1, "one");
    const std::pair<const int, std::string> second(1, "another");

    EXPECT_TRUE(table.insert(first));
    EXPECT_FALSE(table.insert(second));
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.at(1), "one");
}

TEST(hash_table, insert_duplicate_rvalue_does_not_move_value) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});

    std::pair<const int, std::string> item(1, "another");
    EXPECT_FALSE(table.insert(std::move(item)));
    EXPECT_EQ(item.second, "another");
    EXPECT_EQ(table.at(1), "one");
}

TEST(hash_table, contains) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});

    EXPECT_TRUE(table.contains(1));
    EXPECT_FALSE(table.contains(2));
}

TEST(hash_table, at) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});

    EXPECT_EQ(table.at(1), "one");
    table.at(1) = "updated";
    EXPECT_EQ(table.at(1), "updated");
}

TEST(hash_table, at_const) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});
    const hash_table<int, std::string>& const_table = table;

    EXPECT_EQ(const_table.at(1), "one");
}

TEST(hash_table, at_missing_key) {
    hash_table<int, std::string> table;

    EXPECT_THROW(table.at(1), std::out_of_range);
}

TEST(hash_table, at_const_missing_key) {
    const hash_table<int, std::string> table;

    EXPECT_THROW(table.at(1), std::out_of_range);
}

TEST(hash_table, subscript_existing_key) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});

    EXPECT_EQ(table[1], "one");
    table[1] = "updated";
    EXPECT_EQ(table.at(1), "updated");
    EXPECT_EQ(table.size(), 1);
}

TEST(hash_table, subscript_missing_key) {
    hash_table<int, int> table;

    EXPECT_EQ(table[5], 0);
    EXPECT_EQ(table.size(), 1);
    table[5] = 10;
    EXPECT_EQ(table.at(5), 10);
}

TEST(hash_table, erase) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});
    table.insert({2, "two"});

    EXPECT_TRUE(table.erase(1));
    EXPECT_FALSE(table.contains(1));
    EXPECT_TRUE(table.contains(2));
    EXPECT_EQ(table.size(), 1);
}

TEST(hash_table, erase_missing_key) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});

    EXPECT_FALSE(table.erase(2));
    EXPECT_EQ(table.size(), 1);
}

TEST(hash_table, erase_empty_table) {
    hash_table<int, std::string> table;

    EXPECT_FALSE(table.erase(1));
    EXPECT_TRUE(table.empty());
}

TEST(hash_table, collisions) {
    hash_table<int, std::string, constant_hash> table;

    EXPECT_TRUE(table.insert({1, "one"}));
    EXPECT_TRUE(table.insert({2, "two"}));
    EXPECT_TRUE(table.insert({3, "three"}));

    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.at(1), "one");
    EXPECT_EQ(table.at(2), "two");
    EXPECT_EQ(table.at(3), "three");
}

TEST(hash_table, custom_key_equal) {
    hash_table<int, int, modulo_hash, modulo_equal> table;

    EXPECT_TRUE(table.insert({1, 10}));
    EXPECT_FALSE(table.insert({11, 110}));
    EXPECT_TRUE(table.contains(21));
    EXPECT_EQ(table.at(31), 10);
    EXPECT_TRUE(table.erase(41));
    EXPECT_TRUE(table.empty());
}

TEST(hash_table, erase_collision_chain) {
    hash_table<int, int, constant_hash> table;
    table.insert({1, 10});
    table.insert({2, 20});
    table.insert({3, 30});

    EXPECT_TRUE(table.erase(3));
    EXPECT_TRUE(table.erase(2));
    EXPECT_TRUE(table.erase(1));
    EXPECT_TRUE(table.empty());
}

TEST(hash_table, size_and_empty) {
    hash_table<int, int> table;

    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.empty());

    table.insert({1, 10});
    EXPECT_EQ(table.size(), 1);
    EXPECT_FALSE(table.empty());
}

TEST(hash_table, clear) {
    hash_table<int, int> table;
    table.insert({1, 10});
    table.insert({2, 20});
    size_t bucket_count = table.bucket_count();

    table.clear();

    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(table.bucket_count(), bucket_count);
    EXPECT_FALSE(table.contains(1));
}

TEST(hash_table, load_factor) {
    hash_table<int, int> table(8);
    table.insert({1, 10});
    table.insert({2, 20});
    table.insert({3, 30});

    EXPECT_FLOAT_EQ(table.load_factor(), 3.0f / 8.0f);
}

TEST(hash_table, automatic_rehash_on_insert) {
    hash_table<int, int> table(8);

    for (int i = 0; i < 8; ++i)
        EXPECT_TRUE(table.insert({i, i * 10}));

    EXPECT_EQ(table.bucket_count(), 16);
    EXPECT_LE(table.load_factor(), table.max_load_factor());

    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(table.at(i), i * 10);
}

TEST(hash_table, reserve) {
    hash_table<int, int> table(8);
    table.reserve(20);
    size_t bucket_count = table.bucket_count();

    EXPECT_GE(bucket_count, 22);

    for (int i = 0; i < 20; ++i)
        EXPECT_TRUE(table.insert({i, i}));

    EXPECT_EQ(table.bucket_count(), bucket_count);
    EXPECT_LE(table.load_factor(), table.max_load_factor());
}

TEST(hash_table, reserve_does_not_shrink) {
    hash_table<int, int> table(16);
    table.reserve(1);

    EXPECT_EQ(table.bucket_count(), 16);
}

TEST(hash_table, max_load_factor) {
    hash_table<int, int> table;
    table.insert({1, 10});
    table.insert({2, 20});
    table.insert({3, 30});
    table.insert({4, 40});
    table.insert({5, 50});
    table.insert({6, 60});

    table.max_load_factor(0.5f);

    EXPECT_FLOAT_EQ(table.max_load_factor(), 0.5f);
    EXPECT_GE(table.bucket_count(), 12);
    EXPECT_LE(table.load_factor(), table.max_load_factor());
}

TEST(hash_table, max_load_factor_invalid) {
    hash_table<int, int> table;

    EXPECT_THROW(table.max_load_factor(0.25f), std::invalid_argument);
    EXPECT_THROW(table.max_load_factor(0.1f), std::invalid_argument);
}

TEST(hash_table, min_load_factor) {
    hash_table<int, int> table;
    table.min_load_factor(0.1f);

    EXPECT_FLOAT_EQ(table.min_load_factor(), 0.1f);
}

TEST(hash_table, min_load_factor_invalid) {
    hash_table<int, int> table;

    EXPECT_THROW(table.min_load_factor(-0.1f), std::invalid_argument);
    EXPECT_THROW(table.min_load_factor(table.max_load_factor()), std::invalid_argument);
}

TEST(hash_table, automatic_rehash_on_erase) {
    hash_table<int, int> table(8);
    table.min_load_factor(0.5f);

    for (int i = 0; i < 4; ++i)
        table.insert({i, i * 10});

    size_t old_bucket_count = table.bucket_count();
    EXPECT_TRUE(table.erase(0));

    EXPECT_LT(table.bucket_count(), old_bucket_count);
    EXPECT_EQ(table.size(), 3);
    EXPECT_TRUE(table.contains(1));
    EXPECT_TRUE(table.contains(2));
    EXPECT_TRUE(table.contains(3));
    EXPECT_LE(table.load_factor(), table.max_load_factor());
}

TEST(hash_table, copy_ctor) {
    hash_table<int, std::string, constant_hash> table;
    table.insert({1, "one"});
    table.insert({2, "two"});
    table.insert({3, "three"});

    hash_table<int, std::string, constant_hash> copy(table);

    EXPECT_EQ(copy.size(), table.size());
    EXPECT_EQ(copy.bucket_count(), table.bucket_count());
    EXPECT_EQ(copy.at(1), "one");
    EXPECT_EQ(copy.at(2), "two");
    EXPECT_EQ(copy.at(3), "three");

    copy.erase(2);
    EXPECT_TRUE(table.contains(2));
    EXPECT_FALSE(copy.contains(2));
}

TEST(hash_table, copy_assignment) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});
    table.insert({2, "two"});

    hash_table<int, std::string> assigned;
    assigned.insert({100, "hundred"});
    assigned = table;

    EXPECT_EQ(assigned.size(), 2);
    EXPECT_EQ(assigned.at(1), "one");
    EXPECT_EQ(assigned.at(2), "two");
    EXPECT_FALSE(assigned.contains(100));

    assigned.erase(1);
    EXPECT_TRUE(table.contains(1));
}

TEST(hash_table, copy_self_assignment) {
    hash_table<int, int> table;
    table.insert({1, 10});

    table = table;

    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.at(1), 10);
}

TEST(hash_table, move_ctor) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});
    table.insert({2, "two"});

    hash_table<int, std::string> moved(std::move(table));

    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(moved.size(), 2);
    EXPECT_EQ(moved.at(1), "one");
    EXPECT_EQ(moved.at(2), "two");
}

TEST(hash_table, move_ctor_reuses_source) {
    hash_table<int, int> table;
    table.insert({1, 10});

    hash_table<int, int> moved(std::move(table));
    EXPECT_TRUE(table.empty());

    EXPECT_TRUE(table.insert({2, 20}));
    EXPECT_EQ(table.at(2), 20);
    EXPECT_EQ(moved.at(1), 10);
}

TEST(hash_table, move_assignment) {
    hash_table<int, std::string> table;
    table.insert({1, "one"});
    table.insert({2, "two"});

    hash_table<int, std::string> assigned;
    assigned.insert({100, "hundred"});
    assigned = std::move(table);

    EXPECT_TRUE(table.empty());
    EXPECT_EQ(assigned.size(), 2);
    EXPECT_EQ(assigned.at(1), "one");
    EXPECT_EQ(assigned.at(2), "two");
    EXPECT_FALSE(assigned.contains(100));
}

TEST(hash_table, move_self_assignment) {
    hash_table<int, int> table;
    table.insert({1, 10});

    table = std::move(table);

    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.at(1), 10);
}

TEST(hash_table, erase_middle_and_tail_of_collision_chain) {
    hash_table<int, int, constant_hash> table;

    table.insert({1, 10});
    table.insert({2, 20});
    table.insert({3, 30});
    table.insert({4, 40});

    EXPECT_TRUE(table.erase(2));
    EXPECT_TRUE(table.contains(1));
    EXPECT_TRUE(table.contains(3));
    EXPECT_TRUE(table.contains(4));

    EXPECT_TRUE(table.erase(1));
    EXPECT_FALSE(table.contains(1));
    EXPECT_TRUE(table.contains(3));
    EXPECT_TRUE(table.contains(4));

    EXPECT_EQ(table.size(), 2);
}

TEST(hash_table, collisions_after_rehash) {
    hash_table<int, int, constant_hash> table(8);

    for (int i = 0; i < 8; ++i)
        EXPECT_TRUE(table.insert({i, i * 10}));

    EXPECT_GT(table.bucket_count(), 8);

    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(table.at(i), i * 10);

    EXPECT_EQ(table.size(), 8);
}

TEST(hash_table, duplicate_and_missing_key_in_collision_chain) {
    hash_table<int, int, constant_hash> table;

    table.insert({1, 10});
    table.insert({2, 20});
    table.insert({3, 30});

    EXPECT_FALSE(table.insert({2, 200}));
    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.at(2), 20);

    EXPECT_FALSE(table.contains(100));
    EXPECT_THROW(table.at(100), std::out_of_range);
}

TEST(hash_table, automatic_rehash_at_min_load_factor) {
    hash_table<int, int> table(8);

    table.insert({1, 10});
    table.insert({2, 20});
    table.insert({3, 30});

    EXPECT_TRUE(table.erase(1));

    EXPECT_EQ(table.bucket_count(), 4);
    EXPECT_EQ(table.size(), 2);
    EXPECT_FLOAT_EQ(table.load_factor(), 0.5f);

    EXPECT_EQ(table.at(2), 20);
    EXPECT_EQ(table.at(3), 30);
}