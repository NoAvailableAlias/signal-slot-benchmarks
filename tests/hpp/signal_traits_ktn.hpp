#pragma once

#include <KtnFramework/KtnSignalSlot/include/ktnSignalSlot/ktnSignalSlot.hpp>

#include <memory>

struct signal_traits_ktn
{
    static constexpr bool has_signal_empty_test = true;
    static constexpr bool has_connection_connected_test = true;
    static constexpr bool has_disconnect_all = true;
    static constexpr bool has_swap = false;
    static constexpr bool will_deadlock_if_recursively_modified = false;
    static constexpr bool is_intrusive = false;

    template <typename Signature>
    struct resolve_signal;

    template <typename... Args>
    struct resolve_signal<void(Args...)>
    {
        using type = ktn::SignalSlot::ktnSignal<Args...>;
    };

    template <typename Signature>
    using signal = typename resolve_signal<Signature>::type;

    /**
     * @brief The PseudoConnection struct contains the ID and the receiver whose slot is connected.
     * The library allows connection to free functions and function pointers (lambdas with no capture are OK),
     * but lambdas with captures are not allowed. Unfortunately the tests use exactly that.
     */
    struct PseudoConnection
    {
        size_t ID;
        std::shared_ptr<ktn::SignalSlot::ktnSignalReceiverBase> Receiver;
    };

    using connection = PseudoConnection;

    class ReceiverCommon : public ktn::SignalSlot::ktnSignalReceiverBase
    {
    };

    inline static std::vector<std::shared_ptr<ReceiverCommon>> Receivers;
    static void initialize()
    {
        Receivers.size(); // triggers creation
    }
    static void terminate()
    {
        Receivers.clear();
    }

    template <typename Signal>
    static bool empty(Signal& s)
    {
        return s.ConnectionIDs().size() == 0;
    }

    template <typename F, typename... Args>
    static connection connect(ktn::SignalSlot::ktnSignal<Args...>& signal, F&& f)
    {
        struct Receiver : public ReceiverCommon
        {
            explicit Receiver(F&& f) : ActualF(std::forward<F>(f))
            {
            }

            void CallActualF(Args... args)
            {
                ActualF(args...);
            }
            F ActualF;
        };
        const auto receiver = std::make_shared<Receiver>(std::forward<F>(f));
        Receivers.emplace_back(receiver);
        auto result = signal.Connect(receiver.get(), &Receiver::CallActualF);

        connection pseudoConnection;
        pseudoConnection.ID = result.ID;
        pseudoConnection.Receiver = receiver;
        return pseudoConnection;
    }

    template <typename Signal, typename... Args>
    static void trigger(Signal& s, Args&&... args)
    {
        s(args...);
    }

    template <typename Signal>
    static bool connected(Signal& s, const connection& c)
    {
        auto connections = s.ConnectionIDs();
        auto it = std::find(connections.begin(), connections.end(), c);
        return it != connections.end();
    }

    template <typename Signal>
    static void disconnect(Signal& s, connection& c)
    {
        printf("removing connection %lu\r\n", c.ID);
        auto result = s.DisconnectByID(c.ID);

        auto it = std::remove_if(Receivers.begin(), Receivers.end(), [&c](std::shared_ptr<ReceiverCommon> receiver) {
            if (c.Receiver == receiver)
            {
                return true;
            }
            return false;
        });
        if (it != Receivers.end())
            Receivers.erase(it, Receivers.end());
    }

    template <typename Signal>
    static void disconnect_all_slots(Signal& s)
    {
        s.DisconnectAll();
    }
};
