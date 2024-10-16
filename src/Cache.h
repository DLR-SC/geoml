/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 RISC Software GmbH
*/

#pragma once

#include <optional>
#include <mutex>


namespace geoml
{
    template <typename T>
    class ScopeLockedReference {
    public:
        ScopeLockedReference(T& reference, std::mutex& mutex)
            : m_reference(reference), m_mutex(mutex)
        {
        }

        ~ScopeLockedReference() {
            m_mutex.unlock();
        }

        ScopeLockedReference(ScopeLockedReference&&) = default;

        T& operator*() const { return m_reference; }
        T* operator->() const { return &m_reference; }

    private:
        T& m_reference;
        std::mutex& m_mutex;
    };

    template <typename CacheStruct, typename CpacsClass>
    class Cache
    {
    public:
        typedef void(CpacsClass::* BuildFunc)(CacheStruct&) const;

        Cache(CpacsClass& instance, BuildFunc buildFunc)
            : m_instance(instance), m_buildFunc(buildFunc)
        {
        }

        // constructs the cache and gives direct access to it
        // the access is guarded for the lifetime of the returned ScopeLockedReference
        // prefer to rely on the build function for updating the cache
        ScopeLockedReference<CacheStruct> writeAccess() {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_cache)
                m_cache.emplace();
            return ScopeLockedReference<CacheStruct>(*m_cache, *lock.release());
        }

        const CacheStruct& value() const
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            if (!m_cache) {
                m_cache.emplace();
                try {
                    (m_instance.*m_buildFunc)(*m_cache);
                }
                catch (...) {
                    m_cache.reset();
                    throw;
                }
            }
            return m_cache.value();
        }

        const CacheStruct& operator*() const { return value(); }
        const CacheStruct* operator->() const { return &value(); }

        void clear() const
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            m_cache.reset();
        }

    private:
        CpacsClass& m_instance;
        BuildFunc m_buildFunc;
        mutable std::mutex m_mutex;
        mutable std::optional<CacheStruct> m_cache;
    };
}
