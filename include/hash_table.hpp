#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <cmath>
#include <stdexcept>
#include "dictionary.hpp"
#include "array_sequence.hpp"
#include "list_sequence.hpp"

#define INIT_SIZE 0
#define INIT_MIN_LOAD_FACTOR 0.25f
#define INIT_MAX_LOAD_FACTOR 0.9375f

template <typename key, typename value, typename hash = std::hash<key>, typename key_equal = std::equal_to<key>>
class hash_table : public dictionary<key, value>
{
private:
    array_sequence<list_sequence<std::pair<key, value>>> buckets_;

    size_t size_ = INIT_SIZE;
    float max_load_factor_ = INIT_MAX_LOAD_FACTOR;
    float min_load_factor_ = INIT_MIN_LOAD_FACTOR;

    hash hash_;
    key_equal equal_;

    void rehash(size_t new_bucket_count);

    size_t get_bucket_index(size_t hash_value) const;

    size_t find_index(const list_sequence<std::pair<key, value>>& bucket, const key& key_) const;

public:
    explicit hash_table(size_t bucket_count = 8, const hash& hash_func = hash{}, const key_equal& equal_func = key_equal{}) : buckets_(bucket_count == 0 ? 1 : bucket_count), hash_(hash_func), equal_(equal_func) {}
    hash_table(const hash_table& other) : buckets_(other.buckets_), size_(other.size_), max_load_factor_(other.max_load_factor_), min_load_factor_(other.min_load_factor_), hash_(other.hash_), equal_(other.equal_) {}
    hash_table(hash_table&& other) : buckets_(std::move(other.buckets_)), size_(std::exchange(other.size_, 0)), max_load_factor_(other.max_load_factor_), min_load_factor_(other.min_load_factor_), hash_(std::move(other.hash_)), equal_(std::move(other.equal_)) {}

    hash_table& operator=(const hash_table& other);
    hash_table& operator=(hash_table&& other);

    ~hash_table() override = default;

    bool insert(const std::pair<const key, value>& item) override;
    bool insert(std::pair<const key, value>&& item) override;

    bool erase(const key& key_) override;

    bool contains(const key& key_) const override;

    value& at(const key& key_) override;
    const value& at(const key& key_) const override;

    value& operator[](const key& key_);

    bool operator==(const hash_table& other) const;

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

template <typename key, typename value, typename hash, typename key_equal>
hash_table<key, value, hash, key_equal>& hash_table<key, value, hash, key_equal>::operator=(const hash_table& other) {
    if (this == &other) return *this;

    hash_table temp(other);
    *this = std::move(temp);

    return *this;
}

template <typename key, typename value, typename hash, typename key_equal>
hash_table<key, value, hash, key_equal>& hash_table<key, value, hash, key_equal>::operator=(hash_table&& other) {
    if (this == &other) return *this;

    buckets_ = std::move(other.buckets_);
    size_ = std::exchange(other.size_, 0);

    max_load_factor_ = other.max_load_factor_;
    min_load_factor_ = other.min_load_factor_;

    hash_ = std::move(other.hash_);
    equal_ = std::move(other.equal_);

    return *this;
}

template <typename key, typename value, typename hash, typename key_equal>
bool hash_table<key, value, hash, key_equal>::insert(const std::pair<const key, value>& item) {
    if (buckets_.size() == 0) rehash(8);

    size_t hash_value = hash_(item.first);
    size_t index = get_bucket_index(hash_value);

    if (find_index(buckets_[index], item.first) != buckets_[index].size()) return false;

    if (static_cast<float>(size_ + 1) / buckets_.size() > max_load_factor_) {
        rehash(buckets_.size() * 2);
        index = get_bucket_index(hash_value);
    }

    buckets_[index].append(std::pair<key, value>(item.first, item.second));

    ++size_;
    return true;
}

template <typename key, typename value, typename hash, typename key_equal>
bool hash_table<key, value, hash, key_equal>::insert(std::pair<const key, value>&& item) {
    if (buckets_.size() == 0) rehash(8);

    size_t hash_value = hash_(item.first);
    size_t index = get_bucket_index(hash_value);

    if (find_index(buckets_[index], item.first) != buckets_[index].size()) return false;

    if (static_cast<float>(size_ + 1) / buckets_.size() > max_load_factor_) {
        rehash(buckets_.size() * 2);
        index = get_bucket_index(hash_value);
    }

    buckets_[index].append(std::pair<key, value>(item.first, std::move(item.second)));

    ++size_;
    return true;
}

template <typename key, typename value, typename hash, typename key_equal>
bool hash_table<key, value, hash, key_equal>::erase(const key& key_) {
    if (size_ == 0 || buckets_.size() == 0) return false;

    size_t hash_value = hash_(key_);
    size_t index = get_bucket_index(hash_value);

    size_t element_index = find_index(buckets_[index], key_);

    if (element_index == buckets_[index].size()) return false;

    buckets_[index].remove(element_index);
    --size_;

    if (buckets_.size() > 1 &&
        static_cast<float>(size_) / buckets_.size() <= min_load_factor_) {
        rehash(buckets_.size() / 2);
    }

    return true;
}

template <typename key, typename value, typename hash, typename key_equal>
bool hash_table<key, value, hash, key_equal>::contains(const key& key_) const {
    if (size_ == 0 || buckets_.size() == 0) return false;
    size_t index = get_bucket_index(hash_(key_));

    return find_index(buckets_[index], key_) != buckets_[index].size();
}

template <typename key, typename value, typename hash, typename key_equal>
value& hash_table<key, value, hash, key_equal>::at(const key& key_) {
    if (size_ == 0 || buckets_.size() == 0) throw std::out_of_range("key not found");

    size_t index = get_bucket_index(hash_(key_));
    size_t element_index = find_index(buckets_[index], key_);

    if (element_index == buckets_[index].size()) throw std::out_of_range("key not found");

    return buckets_[index][element_index].second;
}

template <typename key, typename value, typename hash, typename key_equal>
const value& hash_table<key, value, hash, key_equal>::at(const key& key_) const {
    if (size_ == 0 || buckets_.size() == 0) throw std::out_of_range("key not found");

    size_t index = get_bucket_index(hash_(key_));
    size_t element_index = find_index(buckets_[index], key_);

    if (element_index == buckets_[index].size()) throw std::out_of_range("key not found");

    return buckets_[index][element_index].second;
}

template <typename key, typename value, typename hash, typename key_equal>
value& hash_table<key, value, hash, key_equal>::operator[](const key& key_) {
    if (!contains(key_)) insert(std::pair<const key, value>(key_, value{}));

    return at(key_);
}

template <typename key, typename value, typename hash, typename key_equal>
size_t hash_table<key, value, hash, key_equal>::size() const noexcept {
    return size_;
}

template <typename key, typename value, typename hash, typename key_equal>
bool hash_table<key, value, hash, key_equal>::empty() const noexcept {
    return size_ == 0;
}

template <typename key, typename value, typename hash, typename key_equal>
void hash_table<key, value, hash, key_equal>::clear() noexcept {
    for (size_t i = 0; i < buckets_.size(); ++i) buckets_[i].clear();

    size_ = 0;
}

template <typename key, typename value, typename hash, typename key_equal>
size_t hash_table<key, value, hash, key_equal>::bucket_count() const noexcept {
    return buckets_.size();
}

template <typename key, typename value, typename hash, typename key_equal>
float hash_table<key, value, hash, key_equal>::max_load_factor() const noexcept {
    return max_load_factor_;
}

template <typename key, typename value, typename hash, typename key_equal>
void hash_table<key, value, hash, key_equal>::max_load_factor(float factor) {
    if (!(factor > min_load_factor_)) throw std::invalid_argument("invalid max_load_factor");
    max_load_factor_ = factor;

    if (buckets_.size() != 0 && static_cast<float>(size_) / buckets_.size() > max_load_factor_) rehash(static_cast<size_t>(std::ceil(static_cast<double>(size_) / max_load_factor_)));
}

template <typename key, typename value, typename hash, typename key_equal>
float hash_table<key, value, hash, key_equal>::min_load_factor() const noexcept {
    return min_load_factor_;
}

template <typename key, typename value, typename hash, typename key_equal>
void hash_table<key, value, hash, key_equal>::min_load_factor(float factor) {
    if (!(factor >= 0.0f && factor < max_load_factor_)) throw std::invalid_argument("invalid min_load_factor");

    min_load_factor_ = factor;
}

template <typename key, typename value, typename hash, typename key_equal>
void hash_table<key, value, hash, key_equal>::reserve(size_t count) {
    size_t new_bucket_count = static_cast<size_t>(std::ceil(static_cast<double>(count) / max_load_factor_));

    if (new_bucket_count > buckets_.size()) rehash(new_bucket_count);
}

template <typename key, typename value, typename hash, typename key_equal>
float hash_table<key, value, hash, key_equal>::load_factor() const noexcept {
    return buckets_.size() == 0 ? 0.0f : static_cast<float>(size_) / buckets_.size();
}

template <typename key, typename value, typename hash, typename key_equal>
void hash_table<key, value, hash, key_equal>::rehash(size_t new_bucket_count) {
    if (new_bucket_count == 0) new_bucket_count = 1;
    size_t min_bucket_count = static_cast<size_t>(std::ceil(static_cast<double>(size_) / max_load_factor_));

    if (new_bucket_count < min_bucket_count) new_bucket_count = min_bucket_count;
    if (new_bucket_count == buckets_.size()) return;

    array_sequence<list_sequence<std::pair<key, value>>> new_buckets(new_bucket_count);

    for (size_t i = 0; i < buckets_.size(); ++i) {
        for (auto& item : buckets_[i]) {
            size_t index = hash_(item.first) % new_bucket_count;

            new_buckets[index].append(std::move(item));
        }

        buckets_[i].clear();
    }

    buckets_ = std::move(new_buckets);
}

template <typename key, typename value, typename hash, typename key_equal>
size_t hash_table<key, value, hash, key_equal>::get_bucket_index(size_t hash_value) const {
    return hash_value % buckets_.size();
}

template <typename key, typename value, typename hash, typename key_equal>
bool hash_table<key, value, hash, key_equal>::operator==(const hash_table& other) const {
    if (this == &other) return true;
    if (size_ != other.size_) return false;

    for (const auto& bucket : buckets_) {
        for (const auto& item : bucket) {
            size_t index = other.get_bucket_index(other.hash_(item.first));

            const auto& other_bucket = other.buckets_[index];
            size_t element_index = other.find_index(other_bucket, item.first);

            if (element_index == other_bucket.size())
                return false;

            if (!(item.second == other_bucket[element_index].second))
                return false;
        }
    }

    return true;
}

template <typename key, typename value, typename hash, typename key_equal>
size_t hash_table<key, value, hash, key_equal>::find_index(const list_sequence<std::pair<key, value>>& bucket, const key& key_) const {
    for (size_t i = 0; i < bucket.size(); ++i) if (equal_(bucket[i].first, key_)) return i;

    return bucket.size();
}