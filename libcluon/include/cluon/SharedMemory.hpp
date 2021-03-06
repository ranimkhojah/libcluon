/*
 * Copyright (C) 2017-2018  Christian Berger
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef CLUON_SHAREDMEMORY_HPP
#define CLUON_SHAREDMEMORY_HPP

#include "cluon/cluon.hpp"
#include "cluon/cluonDataStructures.hpp"

// clang-format off
#ifdef WIN32
    #include <Windows.h>
#else
    #include <pthread.h>
    #include <sys/ipc.h>
#endif
// clang-format on

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <string>
#include <utility>

namespace cluon {

class LIBCLUON_API SharedMemory {
   private:
    SharedMemory(const SharedMemory &) = delete;
    SharedMemory(SharedMemory &&)      = delete;
    SharedMemory &operator=(const SharedMemory &) = delete;
    SharedMemory &operator=(SharedMemory &&) = delete;

   public:
    /**
     * Constructor.
     *
     * @param name Name of the shared memory area; must start with / and must not
     * be longer than NAME_MAX (255) on POSIX or PATH_MAX on WIN32. If the name
     * is missing a leading '/' or is longer than 255, it will be adjusted accordingly.
     * @param size of the shared memory area to create; if size is 0, the class tries to attach to an existing area.
     */
    SharedMemory(const std::string &name, uint32_t size = 0) noexcept;
    ~SharedMemory() noexcept;

    /**
     * @return true when this shared memory area is locked.
     */
    bool isLocked() const noexcept;

    /**
     * This method locks the shared memory area.
     */
    void lock() noexcept;

    /**
     * This method unlocks the shared memory area.
     */
    void unlock() noexcept;

    /**
     * This method waits for being notified from the shared condition.
     */
    void wait() noexcept;

    /**
     * This method notifies all threads waiting on the shared condition.
     */
    void notifyAll() noexcept;

    /**
     * This method sets the time stamp that can be used to
     * express the sample time stamp of the data in residing
     * in the shared memory.
     *
     * This method is only allowed when the shared memory is locked.
     *
     * @param ts TimeStamp.
     * @return true if the timestamp could set; false if the shared memory was not locked.
     */
    bool setTimeStamp(const cluon::data::TimeStamp &ts) noexcept;

    /**
     * This method returns the sample time stamp.
     *
     * This method is only allowed when the shared memory is locked.
     *
     * @return (true, sample time stamp) or (false, 0) in case if the shared memory was not locked.
     */
    std::pair<bool, cluon::data::TimeStamp> getTimeStamp() noexcept;

   public:
    /**
     * @return True if the shared memory area is existing and usable.
     */
    bool valid() noexcept;

    /**
     * @return Pointer to the raw shared memory or nullptr in case of invalid shared memory.
     */
    char *data() noexcept;

    /**
     * @return The size of the shared memory area.
     */
    uint32_t size() const noexcept;

    /**
     * @return Name the shared memory area.
     */
    const std::string name() const noexcept;

#ifdef WIN32
   private:
    void initWIN32() noexcept;
    void deinitWIN32() noexcept;
    void lockWIN32() noexcept;
    void unlockWIN32() noexcept;
    void waitWIN32() noexcept;
    void notifyAllWIN32() noexcept;
#else
   private:
    void initPOSIX() noexcept;
    void deinitPOSIX() noexcept;
    void lockPOSIX() noexcept;
    void unlockPOSIX() noexcept;
    void waitPOSIX() noexcept;
    void notifyAllPOSIX() noexcept;
    bool validPOSIX() noexcept;

    void initSysV() noexcept;
    void deinitSysV() noexcept;
    void lockSysV() noexcept;
    void unlockSysV() noexcept;
    void waitSysV() noexcept;
    void notifyAllSysV() noexcept;
    bool validSysV() noexcept;
#endif

   private:
    std::string m_name{""};
    std::string m_nameForTimeStamping{""};
    uint32_t m_size{0};
    char *m_sharedMemory{nullptr};
    char *m_userAccessibleSharedMemory{nullptr};
    bool m_hasOnlyAttachedToSharedMemory{false};

    std::atomic<bool> m_broken{false};
    std::atomic<bool> m_isLocked{false};

#ifdef WIN32
    HANDLE __conditionEvent{nullptr};
    HANDLE __mutex{nullptr};
    HANDLE __sharedMemory{nullptr};
#else
    int32_t m_fdForTimeStamping{-1};

    bool m_usePOSIX{true};

    // Member fields for POSIX-based shared memory.
#if !defined(__NetBSD__) && !defined(__OpenBSD__)
    int32_t m_fd{-1};
    struct SharedMemoryHeader {
        uint32_t __size;
        pthread_mutex_t __mutex;
        pthread_cond_t __condition;
    };
    SharedMemoryHeader *m_sharedMemoryHeader{nullptr};
#endif

    // Member fields for SysV-based shared memory.
    key_t m_shmKeySysV{0};
    key_t m_mutexKeySysV{0};
    key_t m_conditionKeySysV{0};

    int m_sharedMemoryIDSysV{-1};
    int m_mutexIDSysV{-1};
    int m_conditionIDSysV{-1};
#endif
};
} // namespace cluon

#endif
