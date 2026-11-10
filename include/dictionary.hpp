#pragma once

#include <utility>

template <typename key, typename value>
class dictionary
{
public:
    virtual ~dictionary() = default;

    virtual bool insert(const std::pair<const key, value>& item) = 0;
    virtual bool insert(std::pair<const key, value>&& item) = 0;

    virtual bool erase(const key& key_) = 0;

    virtual bool contains(const key& key_) const = 0;

    virtual value& at(const key& key_) = 0;
    virtual const value& at(const key& key_) const = 0;

    virtual size_t size() const noexcept = 0;
    virtual bool empty() const noexcept = 0;

    virtual void clear() noexcept = 0;
};