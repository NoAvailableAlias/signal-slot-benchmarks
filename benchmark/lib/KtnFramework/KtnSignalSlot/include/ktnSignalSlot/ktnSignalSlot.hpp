/*******************************************************************************
 * This file is part of the KtnSignalSlot project.
 *
 * Copyright 2019-2020 Tien Dat Nguyen
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

#ifndef KTNSIGNALSLOT_KTNSIGNALSLOT_HPP
#define KTNSIGNALSLOT_KTNSIGNALSLOT_HPP
#include <algorithm>
#include <atomic>
#include <mutex>
#include <set>
#include <vector>

namespace ktn::SignalSlot {
class ktnSignalReceiverBase;

enum class ReceiverAdditionResult { Duplicate, Failure, Success };

class ktnSignalBase {
public:
    ktnSignalBase() {}
    virtual ~ktnSignalBase() = default;

    virtual void CleanUp(ktnSignalReceiverBase *eReceiver) = 0;
    void RemoveReceiver(ktnSignalReceiverBase *eReceiver) {
        CleanUp(eReceiver);
    }
};

class ktnSignalReceiverBase {
public:
    virtual ~ktnSignalReceiverBase() {
        for (auto &signal : m_Signals) { signal->RemoveReceiver(this); }
    }

    const std::vector<ktnSignalBase *> Signals() const {
        return m_Signals;
    }

    inline void AddSignal(ktnSignalBase *eSignal) {
        auto iterator(std::find(m_Signals.begin(), m_Signals.end(), eSignal));
        if (iterator == m_Signals.end()) { m_Signals.emplace_back(eSignal); }
    }

    inline void RemoveSignal(ktnSignalBase *eSignal) {
        auto iterator(std::find(m_Signals.begin(), m_Signals.end(), eSignal));
        if (iterator != m_Signals.end()) { m_Signals.erase(iterator); }
    }

private:
    std::vector<ktnSignalBase *> m_Signals;
};

/**
 * @brief The ConnectionResult enum enumerates possible results for the Connect functions.
 */
enum class ConnectionResult { //
    Duplicate, //!< Connection already exists.
    Failure, //!< Connection does not exists, and no new connection was created.
    Success //!< New connection was created.
};

struct ConnectionResultWithId {
    /**
     * @brief ConnectionResultWithId is the constructor.
     * Note that since ID and Result are const,
     * no other constructor is provided.
     * @param eID
     * @param eResult
     */
    ConnectionResultWithId(size_t eID, ConnectionResult eResult) : ID(eID), Result(eResult) {}

    /**
     * @brief ID is the ID of the resulting connection.
     * Is 0 when no connection is created.
     */
    const size_t ID;

    /**
     * @brief Result
     * @see ConnectionResult
     */
    const ConnectionResult Result;
};

/**
 * @brief The DisconnectionResult enum enumerates possible results for the Disconnect functions.
 */
enum class DisconnectionResult {
    NotNeccessary, //!< Removal was not neccessary. Either the connection doesn't exist, or is being removed.
    Success //!< The slot was removed.
};

/**
 * @brief The ktnSignal class is the template class for all signals.
 */
template <typename... Args>
class ktnSignal final : public ktnSignalBase {
public:
    /**
     * @brief ~ktnSignal is the default destructor for a signal.
     * It deregister the signal from the static manager's signal list.
     */
    ~ktnSignal() final {
        m_Destroying = true;
        std::lock_guard guard(m_Mutex_CurrentConnections);
        for (auto &connection : m_CurrentConnections) {
            if (connection.Slot->Instance() != nullptr) { //
                connection.Slot->Instance()->RemoveSignal(this);
            }
            delete connection.Slot;
        }
    }

    /**
     * @brief operator() is used to emit the signal.
     * @param eArgs is the argument pack to be used by the slots.
     */
    void operator()(Args... eArgs) noexcept {
        if (m_CallingCurrentSlots) return;
        {
            m_CallingCurrentSlots = true;
            auto guardConnection = std::lock_guard(m_Mutex_CurrentConnections);

            for (auto &connection : m_CurrentConnections) {
                if (connection.PendingRemoval) { continue; }
                try {
                    connection.Slot->Execute(eArgs...);
                } catch (std::exception &e) { fprintf(stderr, "Exception thrown while calling slot: %s", e.what()); }
            }
            m_CallingCurrentSlots = false;
        }
        if (!m_ConnectionsModifiedDuringEmission) { return; }

        auto guard_Connections = std::lock_guard(m_Mutex_CurrentConnections);
        auto guard_PendingConnections = std::lock_guard(m_Mutex_PendingConnections);
        auto guard_Slots = std::lock_guard(m_Mutex_Slots);

        auto connectionRemovable = [this](Connection &connection) -> bool {
            if (connection.PendingRemoval) {
                delete connection.Slot;
                return true;
            }
            return false;
        };

        auto iterator = std::remove_if(m_CurrentConnections.begin(), m_CurrentConnections.end(), connectionRemovable);
        if (iterator != m_CurrentConnections.end()) { m_CurrentConnections.erase(iterator, m_CurrentConnections.end()); }

        m_CurrentConnections.reserve(m_PendingConnections.size());
        m_CurrentConnections.insert(m_CurrentConnections.end(), m_PendingConnections.begin(), m_PendingConnections.end());
        m_PendingConnections.clear();

        m_ConnectionsModifiedDuringEmission = false;
    }

    /**
     * @brief CleanUp removes all connections to a specified receiver.
     * @param eReceiver is the receiver to which all connections are to be removed.
     */
    inline void CleanUp(ktnSignalReceiverBase *eReceiver) final {
        DisconnectReceiver(eReceiver);
    }

    /**
     * @brief Connect connects the signal to a member function of class C's instance eInstance.
     * @param eInstance is the instance of class C which is connected to this signal.
     * @param eFunction is the function in class C that is used as the slot.
     */
    template <class C, typename N>
    [[nodiscard]] ConnectionResultWithId Connect(C *eInstance, N (C::*eFunction)(Args...)) {
        if (m_Destroying) { return {0, ConnectionResult::Failure}; }
        if (eInstance == nullptr) { return {0, ConnectionResult::Failure}; } // invalid pointer
        auto tempSlot = new ktnSlot_ClassMember<C, N>(eInstance, eFunction);
        if (m_CallingCurrentSlots) {
            auto guard_PendingConnections = std::lock_guard(m_Mutex_PendingConnections);
            auto guard_Slots = std::lock_guard(m_Mutex_Slots);
            size_t id = 0;
            bool isDuplicate = false;

            _CheckDuplicate_NoLock(tempSlot, id, isDuplicate);
            if (isDuplicate) {
                delete tempSlot;
                return {id, ConnectionResult::Duplicate};
            }

            id = _NextUnusedConnectionID();
            eInstance->AddSignal(this);
            m_PendingConnections.emplace_back(Connection{id, tempSlot});
            m_ConnectionsModifiedDuringEmission = true;
            return {id, ConnectionResult::Success};
        } else {
            auto guard_Connections = std::lock_guard(m_Mutex_CurrentConnections);
            auto guard_PendingConnections = std::lock_guard(m_Mutex_PendingConnections);
            size_t id = 0;
            bool isDuplicate = false;

            if (m_CurrentConnections.empty()) {
                eInstance->AddSignal(this);
                m_CurrentConnections.emplace_back(Connection{1, tempSlot});
                return {1, ConnectionResult::Success};
            }

            _CheckDuplicate_NoLock(tempSlot, id, isDuplicate);
            if (isDuplicate) {
                delete tempSlot;
                return {id, ConnectionResult::Duplicate};
            }

            id = _NextUnusedConnectionID();
            eInstance->AddSignal(this);
            m_CurrentConnections.emplace_back(Connection{id, tempSlot});
            return {id, ConnectionResult::Success};
        }
    }

    /**
     * @brief Connect connects the slot to a free function or a stateless lambda.
     * @param eFunction is the function that is used as the slot.
     * Note that the connection to a lambda can only be removed by using the returned ID.
     */
    template <typename N>
    [[nodiscard]] ConnectionResultWithId Connect(N (*eFunction)(Args...)) {
        if (m_Destroying) { return {0, ConnectionResult::Failure}; }
        auto tempSlot = new ktnSlot_FreeFunction<N>(eFunction);

        if (m_CallingCurrentSlots) {
            auto guard_PendingConnections = std::lock_guard(m_Mutex_PendingConnections);
            size_t id = 0;
            bool isDuplicate = false;

            _CheckDuplicate_NoLock(tempSlot, id, isDuplicate);
            if (isDuplicate) {
                delete tempSlot;
                return {id, ConnectionResult::Duplicate};
            }

            id = _NextUnusedConnectionID();
            m_PendingConnections.emplace_back(Connection{id, tempSlot});
            m_ConnectionsModifiedDuringEmission = true;
            return {id, ConnectionResult::Success};
        } else {
            auto guard_Connections = std::lock_guard(m_Mutex_CurrentConnections);
            auto guard_PendingConnections = std::lock_guard(m_Mutex_PendingConnections);
            size_t id = 0;
            bool isDuplicate = false;

            if (m_CurrentConnections.empty()) {
                m_CurrentConnections.emplace_back(Connection{1, tempSlot});
                return {1, ConnectionResult::Success};
            }

            _CheckDuplicate_NoLock(tempSlot, id, isDuplicate);
            if (isDuplicate) {
                delete tempSlot;
                return {id, ConnectionResult::Duplicate};
            }

            id = _NextUnusedConnectionID();
            m_CurrentConnections.emplace_back(Connection{id, tempSlot});
            return {id, ConnectionResult::Success};
        }
    }

    /**
     * @brief ConnectionIDs
     * @return The list of current connections' IDs.
     */
    [[nodiscard]] inline std::set<size_t> ConnectionIDs() const {
        if (!m_CallingCurrentSlots) {
            std::lock_guard guard(m_Mutex_CurrentConnections);
            return _ConnectionIDs_NoLock();
        }
        return _ConnectionIDs_NoLock();
    }

    /**
     * @brief Disconnect disconnects the signal from all slots of class C's instance eInstance.
     * @param eInstance is the signal receiver to which all connections are to be removed.
     */
    template <class C>
    [[nodiscard]] DisconnectionResult DisconnectReceiver(C *eReceiver) {
        if (m_Destroying) { return DisconnectionResult::NotNeccessary; }
        if (eReceiver == nullptr) { return DisconnectionResult::NotNeccessary; }
        if (m_CurrentConnections.empty()) { return DisconnectionResult::NotNeccessary; }

        if (m_CallingCurrentSlots) {
            auto guard = std::lock_guard(m_Mutex_Slots);
            bool removableConnectionsFound = false;
            for (auto &connection : m_CurrentConnections) {
                if (eReceiver == connection.Slot->Instance()) {
                    connection.PendingRemoval = true;
                    m_ConnectionsModifiedDuringEmission = true;
                    removableConnectionsFound = true;
                }
            }
            if (removableConnectionsFound) return DisconnectionResult::Success;
            return DisconnectionResult::NotNeccessary;
        }

        auto guard_connections = std::lock_guard(m_Mutex_CurrentConnections);
        auto guard_slot = std::lock_guard(m_Mutex_Slots);
        bool removableConnectionsFound = false;
        auto receiverMatch = [this, &eReceiver, &removableConnectionsFound](Connection &connection) {
            if (connection.Slot->Instance() == eReceiver) {
                delete connection.Slot;
                removableConnectionsFound = true;
                return true;
            }
            return false;
        };

        auto iterator = std::remove_if(m_CurrentConnections.begin(), m_CurrentConnections.end(), receiverMatch);

        if (!removableConnectionsFound) { return DisconnectionResult::NotNeccessary; }

        m_CurrentConnections.erase(iterator, m_CurrentConnections.end());
        return DisconnectionResult::Success;
    }

    /**
     * @brief Disconnect disconnects the signal from a member function of class C's instance eInstance.
     * @param eInstance is the instance of class C which is connected to this signal.
     * @param eFunction is the function in class C that will be disconnected.
     */
    template <class C, typename N>
    [[nodiscard]] DisconnectionResult Disconnect(C *eInstance, N (C::*eFunction)(Args...)) {
        if (m_Destroying) { return DisconnectionResult::NotNeccessary; }
        ktnSlot_ClassMember<C, N> tempSlot(eInstance, eFunction);

        if (m_CallingCurrentSlots) {
            auto guard = std::lock_guard(m_Mutex_Slots);
            for (auto &connection : m_CurrentConnections) {
                if (tempSlot == connection.Slot) {
                    connection.PendingRemoval = true;
                    m_ConnectionsModifiedDuringEmission = true;
                    return DisconnectionResult::Success;
                }
            }
            return DisconnectionResult::NotNeccessary;
        }

        auto guard_connections = std::lock_guard(m_Mutex_CurrentConnections);
        auto guard_slot = std::lock_guard(m_Mutex_Slots);
        auto connectionMatch = [this, &tempSlot](Connection &connection) {
            if (tempSlot == connection.Slot) {
                connection.Slot->Instance()->RemoveSignal(this);
                delete connection.Slot;
                return true;
            }
            return false;
        };

        auto iterator = std::remove_if(m_CurrentConnections.begin(), m_CurrentConnections.end(), connectionMatch);

        // This check must be done, because erasing end() blocks indefinitely
        if (iterator == m_CurrentConnections.end()) { return DisconnectionResult::NotNeccessary; }
        m_CurrentConnections.erase(iterator);

        return DisconnectionResult::Success;
    }

    /**
     * @brief Disconnect disconnects the slot from a free function.
     * @param eFunction is the free function that will be disconnected.
     */
    template <typename N>
    [[nodiscard]] DisconnectionResult Disconnect(N (*eFunction)(Args...)) {
        if (m_Destroying) { return DisconnectionResult::NotNeccessary; }
        ktnSlot_FreeFunction<N> tempSlot(eFunction);
        if (m_CallingCurrentSlots) {
            auto guard = std::lock_guard(m_Mutex_Slots);
            for (auto &connection : m_CurrentConnections) {
                if (tempSlot == connection.Slot) {
                    connection.PendingRemoval = true;
                    m_ConnectionsModifiedDuringEmission = true;
                    return DisconnectionResult::Success;
                }
            }
            return DisconnectionResult::NotNeccessary;
        }

        auto guard_connections = std::lock_guard(m_Mutex_CurrentConnections);
        auto guard_slot = std::lock_guard(m_Mutex_Slots);
        auto connectionMatch = [this, &tempSlot](Connection &connection) {
            if (tempSlot == connection.Slot) {
                delete connection.Slot;
                return true;
            }
            return false;
        };

        auto iterator = std::remove_if(m_CurrentConnections.begin(), m_CurrentConnections.end(), connectionMatch);

        // This check must be done, because erasing end() blocks indefinitely
        if (iterator == m_CurrentConnections.end()) { return DisconnectionResult::NotNeccessary; }
        m_CurrentConnections.erase(iterator);

        return DisconnectionResult::Success;
    }

    /**
     * @brief DisconnectByID disconnects a slot from a connection based on its ID.
     * While it can be used to remove all kinds of connections, it is the only way to remove connections to lambdas,
     * without using DisconnectAll()
     * @param eConnectionID is the ID of the connection to be disconnected.
     */
    [[nodiscard]] DisconnectionResult DisconnectByID(const size_t &eConnectionID) {
        if (m_Destroying) { return DisconnectionResult::NotNeccessary; }
        if (m_CallingCurrentSlots) {
            auto guard = std::lock_guard(m_Mutex_Slots);
            for (auto &connection : m_CurrentConnections) {
                if (eConnectionID == connection.ID) {
                    connection.PendingRemoval = true;
                    m_ConnectionsModifiedDuringEmission = true;
                    return DisconnectionResult::Success;
                }
            }
            return DisconnectionResult::NotNeccessary;
        }

        auto guard_connections = std::lock_guard(m_Mutex_CurrentConnections);
        auto guard_slot = std::lock_guard(m_Mutex_Slots);
        auto idMatch = [this, &eConnectionID](Connection &connection) {
            if (connection.ID == eConnectionID) {
                if (connection.Slot->Instance() != nullptr) { connection.Slot->Instance()->RemoveSignal(this); }
                delete connection.Slot;
                return true;
            }
            return false;
        };

        auto i = std::find_if(m_CurrentConnections.begin(), m_CurrentConnections.end(), idMatch);

        // This check must be done, because erasing end() blocks indefinitely
        if (i == m_CurrentConnections.end()) { return DisconnectionResult::NotNeccessary; }

        m_CurrentConnections.erase(i);
        return DisconnectionResult::Success;
    }

    /**
     * @brief DisconnectAll disconnects the signal from all slots.
     */
    void DisconnectAll() {
        if (m_Destroying) { return; }
        if (m_CallingCurrentSlots) {
            auto guard_slot = std::lock_guard(m_Mutex_Slots);
            for (auto &connection : m_CurrentConnections) { connection.PendingRemoval = true; }
            m_ConnectionsModifiedDuringEmission = true;
            return;
        }

        auto guard_connections = std::lock_guard(m_Mutex_CurrentConnections);
        auto guard_slot = std::lock_guard(m_Mutex_Slots);
        for (auto &connection : m_CurrentConnections) {
            if (connection.Slot->Instance() != nullptr) { connection.Slot->Instance()->RemoveSignal(this); }
            delete connection.Slot;
        }
        m_CurrentConnections.clear();
    }

private:
    /**
     * @brief The IktnSlot class describe the interface of different slot types.
     */
    class IktnSlot {
    public:
        virtual ~IktnSlot() = default;
        virtual bool operator==(IktnSlot *eSlot) = 0;

        virtual void Execute(Args...) = 0;
        virtual ktnSignalReceiverBase *Instance() = 0;
    }; // class IktnSlot

    /**
     * @brief The ktnSlot_ClassMember class describes the slot corresponding to an object's member function.
     */
    template <class Receiver, typename SlotReturnType>
    class ktnSlot_ClassMember final : public IktnSlot {
    public:
        ktnSlot_ClassMember(Receiver *eInstance, SlotReturnType (Receiver::*eFunction)(Args...)) {
            m_Instance = eInstance;
            m_Function = eFunction;
        }

        ~ktnSlot_ClassMember() final = default;

        inline bool operator==(IktnSlot *eSlot) final {
            auto eSlotCast = dynamic_cast<ktnSlot_ClassMember *>(eSlot);
            if (eSlotCast == nullptr) { return false; } // type mismatch
            return m_Instance == eSlotCast->m_Instance && m_Function == eSlotCast->m_Function;
        }

        /**
         * @brief Execute executes the function.
         * @param eArguments is the argument pack given when the signal emits.
         */
        inline void Execute(Args... eArguments) final {
            (m_Instance->*m_Function)(eArguments...);
        }

        /**
         * @brief Instance
         * @return the pointer to the associated receiver instance.
         */
        inline ktnSignalReceiverBase *Instance() final {
            return m_Instance;
        }

    private:
        Receiver *m_Instance = nullptr;
        SlotReturnType (Receiver::*m_Function)(Args...) = nullptr;
    }; // class ktnSlot_ClassMember

    /**
     * @brief The ktnSlot_FreeFunction class describes the slot corresponding to a free function.
     */
    template <typename SlotReturnType>
    class ktnSlot_FreeFunction final : public IktnSlot {
    public:
        ~ktnSlot_FreeFunction() final = default;
        explicit ktnSlot_FreeFunction(SlotReturnType (*eFunction)(Args...)) {
            m_Function = eFunction;
        }

        inline bool operator==(IktnSlot *eSlot) final {
            auto slotCast = dynamic_cast<ktnSlot_FreeFunction *>(eSlot);
            if (slotCast == nullptr) { return false; } // type mismatch
            return m_Function == slotCast->m_Function;
        }

        /**
         * @brief Execute executes the function.
         * @param eArguments is the argument pack given when the signal emits.
         */
        inline void Execute(Args... eArguments) final {
            m_Function(eArguments...);
        }

        /**
         * @brief Instance
         * @return nullptr since there is no instance attached.
         */
        inline ktnSignalReceiverBase *Instance() final {
            return nullptr;
        }

        SlotReturnType (*m_Function)(Args...) = nullptr;
    }; // class ktnSlot_FreeFunction

    /**
     * @brief The Connection struct contains the ID of the connection,
     * and the associated slot.
     */
    struct Connection {
        size_t ID; //!< The ID of the connection.
        IktnSlot *Slot = nullptr; //!< The associated slot.
        bool PendingRemoval = false;
    };

    inline void _CheckDuplicate_NoLock(IktnSlot *eSlot, size_t &ID, bool &IsDuplicate) const {
        for (auto &connection : m_CurrentConnections) {
            if (*eSlot == connection.Slot) {
                ID = connection.ID;
                IsDuplicate = true;
            };
        }

        for (auto &connection : m_PendingConnections) {
            if (*eSlot == connection.Slot) {
                ID = connection.ID;
                IsDuplicate = true;
            };
        }
    }

    [[nodiscard]] inline std::set<size_t> _ConnectionIDs_NoLock() const {
        std::set<size_t> connectionIDs;
        for (auto &connection : m_CurrentConnections) { connectionIDs.insert(connection.ID); }
        for (auto &connection : m_PendingConnections) { connectionIDs.insert(connection.ID); }
        return connectionIDs;
    }

    /**
     * @brief _NextUnusedConnectionID finds a suitable ID for a new connection.
     * @return  the lowest number that can be assigned to a new connection.
     */
    [[nodiscard]] inline size_t _NextUnusedConnectionID() const {
        auto connections = _ConnectionIDs_NoLock();
        if (connections.empty()) { return 1; }
        auto maxConnectionID = *std::max_element(connections.begin(), connections.end());
        for (size_t i = 0; i < maxConnectionID; ++i) {
            if (connections.end() == std::find(connections.begin(), connections.end(), i + 1)) { return i + 1; }
        }
        return maxConnectionID + 1;
    }

    std::mutex mutable m_Mutex_CurrentConnections;
    std::mutex mutable m_Mutex_PendingConnections;
    std::mutex mutable m_Mutex_Slots;

    /**
     * @brief m_CurrentConnections is the list of connections.
     */
    std::vector<Connection> m_CurrentConnections;

    /**
     * @brief m_PendingConnections is the list of connections added during signal emission.
     * These will be added to the connection list once all the previously registered slots are called.
     */
    std::vector<Connection> m_PendingConnections;

    /**
     * @brief m_ConnectionsModifiedDuringEmission is true when a connection is added or marked as PendingRemoval during a signal emission.
     */
    std::atomic<bool> m_ConnectionsModifiedDuringEmission = false;

    /**
     * @brief m_Destroying only turns true in the destructor.
     * During destruction, any attempt to connect or disconnect will fail.
     */
    std::atomic<bool> m_Destroying = false;

    std::atomic<bool> m_CallingCurrentSlots = false;
}; // namespace ktn::SignalSlot
} // namespace ktn::SignalSlot
#endif // KTNSIGNALSLOT_KTNSIGNALSLOT_HPP
