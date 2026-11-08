#pragma once

#include <functional>
#include <memory>
#include <utility>
#include "dictionary.hpp"
//#include "array_sequence.hpp"

template <typename key, typename value, typename hash = std::hash<key>, typename key_equal = std::equal_to<key>>
class hash_table : public dictionary<key, value>
{
private:
    struct node
    {
        std::pair<const key, value> value_;
        size_t hash_;
        std::unique_ptr<node> next;

        template <typename K, typename V>
        node(K&& key_, V&& value_, size_t hash_value) : value_(std::forward<K>(key_), std::forward<V>(value_)), hash_(hash_value) {}
    };

    array_sequence<std::unique_ptr<node>> buckets_;

    size_t size_ = 0;
    float max_load_factor_ = 0.9375f;
    float min_load_factor_ = 0.25f;

    hash hash_;
    key_equal equal_;

    void rehash(size_t new_bucket_count);

    size_t get_bucket_index(size_t hash_value) const;

    node* find_node(const key& key, size_t hash_value);
    const node* find_node(const key& key, size_t hash_value) const;

    std::unique_ptr<node> clone_chain(const node* source) const;

public:
    explicit hash_table(size_t bucket_count = 8, const hash& hash_func = hash{}, const key_equal& equal_func = key_equal{}) : buckets_(bucket_count == 0 ? 1 : bucket_count), hash_(hash_func), equal_(equal_func) {}
    hash_table(const hash_table& other);
    hash_table(hash_table&& other);

    hash_table& operator=(const hash_table& other);
    hash_table& operator=(hash_table&& other);

    ~hash_table() override = default;

    bool insert(const std::pair<const key, value>& item) override;
    bool insert(std::pair<const key, value>&& item) override;

    bool erase(const key& key) override;

    bool contains(const key& key) const override;

    value& at(const key& key) override;
    const value& at(const key& key) const override;

    value& operator[](const key& key);

    size_t size() const noexcept override;
    bool empty() const noexcept override;
    void clear() noexcept override;

    size_t bucket_count() const noexcept;

    float load_factor() const noexcept;

    float max_load_factor() const noexcept;
    void max_load_factor(float factor);

    float min_load_factor() const noexcept;
    void min_load_factor(float factor);

    void reserve(size_t count);
};

