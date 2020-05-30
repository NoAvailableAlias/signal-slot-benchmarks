# **Public API**

## **`class signal<void(ArgTs...)>`**

**`signal`** manages a list of connections to slots and invokes the slots during its emissions. It performs the necessary synchronization to ensure safety in multithreaded environments.

### Methods:

-----------------------

**`signal();`**

Constructs a new `signal` instance with no connected slots.

_Throws an exception if memory allocation fails._

-----------------------

**`~signal() noexcept;`**

Destroys the `signal` instance and disconnects all connected slots, if any.

If destructor is invoked during signal emission (even if a called slot destroys the signal), emission stops immediately, all slots are disconnected, and the last thread using the signal performs cleanup.

-----------------------

1. **`template<typename Ty, typename Fn>`**  
**`unsigned connect(Ty * object, Fn slot, exec type = {});`**

2. **`template<typename Fn>`**  
**`unsigned connect(Fn slot);`**

Connects the `signal` to the given **`slot`**.

1. Connects object and method, or `context` and callable.
2. Connects callable

**`slot`** is any callable that can be invoked with argument types specified in the `signal`'s signature. A signal-slot type mismatch results in a compilation error.

If **`object`** or **`slot`** or both are null pointers, connection is not established and id of value `0` is returned _(see below)_.

If **`slot`** is a pointer to method, **`object`** must be a pointer to object the method should be called on. If **`slot`** is any other callable and **`object`** is still provided, it must specify the associated `context` object _(see `context` class)_.

If object's class has unambiguous and accessible `context` base class, the object obtains the following additional features:
* **`slot`** will be invoked in the thread `context` object lives in (default behavior);
* once the `context` object gets destroyed, its associated slots will no longer be reachable for signal emissions.

If `context` object is provided, **`type`** may specify the connection type: synchronous or asynchronous. The default is neither, so the signal detects how to call the target slot automatically, based on the `context`'s thread affinity. However, **`type`** may be explicitly set as `exec::sync` or `exec::async`.

If connection has been established successfully, returns the connection's id value that can be used to disconnect the connection later; otherwise, returns `0`. _Note!_ Disconnection by id is used mainly for lambdas and other targets that do not have accessible equality comparison operator. For equality comparable targets it is more convenient to use disconnection by value _(see `disconnect()` method)_.

_Throws an exception if memory allocation fails._

-----------------------

1. **`template<typename Ty, typename Fn>`**  
**`bool disconnect(Ty * object, Fn slot) noexcept;`**

2. **`template<typename Fn>`**  
**`bool disconnect(Fn slot) noexcept;`**

3. **`template<typename Fn>`**  
**`bool disconnect(unsigned id) noexcept;`**

4. **`template<typename Fn>`**  
**`void disconnect() noexcept;`**

Disconnects the `signal` from the given **`slot`**.

1. Disconnects object and method.
2. Disconnects callable.
3. Disconnects connection identified by **`id`**.
4. Disconnects all slots.

**`slot`** is any callable that can be invoked with argument types specified in the `signal`'s signature. A signal-slot type mismatch results in a compilation error.

If **`object`** or **`slot`** or both are null pointers, disconnection is not performed and `false` is returned.

If **`slot`** is a pointer to method, **`object`** must be a pointer to object the method should be called on.

If **`slot`** is not equality comparable, disconnection is not performed and `false` is returned. Two slots are equal, if they are of exactly the same static type with accessible equality comparison operator, and the result of their comparison is `true`.

_Note!_ If **`slot`** is not a pointer to method and is associated with some `context` object, that `context` object must be omitted. The reason is that for non pointer-to-member slots, the `context` object is not a part of the slot, but rather just a helper object that does not participate in the slot invocation itself.

If disconnection is successful returns `true`; otherwise, returns `false`.

-----------------------

**`void emit(ArgTs ... args) const;`**

**`void operator()(ArgTs ... args) const;`**

Emits the `signal`. The two methods are identical.

During signal emission all connected slots are invoked with provided arguments. The exact invocation order is unspecified.

Each slot in the signal's list is invoked synchronously or asynchronously depending on an associated `context` object, if any, and depending on execution type explicitly specified in `connect` method. If no execution type was specified, the default rule is used:

> if the `signal` is emitted from the same thread the `context` object lives in, the `context`'s slots are invoked synchronously; otherwise the call is transferred into `context`'s thread and invoked asynchronously.

If no `context` object is associated with a slot, the slot is always invoked synchronously.

*Note!* During asynchronous slot invocations arguments and some necessary data must be copied into dynamic memory.

These methods are lock-free, except when an unreachable "expired" `context` has been detected. In this case the internal lock has to be acquired in order to disconnect the unreachable slot.

If the `signal` is blocked, the methods do nothing.

_Throws an exception if memory allocation fails._

-----------------------

**`bool block(bool yes = true) noexcept;`**

Blocks or unblocks the `signal`.

Returns `true`, if the signal was blocked prior to the call, `false` otherwise.

-----------------------

**`bool blocked() const noexcept;`**

Checks whether the `signal` is blocked.

Returns `true`, if the signal is blocked, `false` otherwise.

_Note!_ The result of this method call is only an approximation. If the signal is blocked and unblocked by multiple threads, the returned value may not reflect the current blocking state.

-----------------------

**`signal(const signal &) = delete;`**

**`signal(signal &&) = delete;`**

**`signal & operator=(const signal &) = delete;`**

**`signal & operator=(signal &&) = delete;`**

`signal` is neither copy | move constructible nor copy | move assignable.

-----------------------

## **`class context`**

Base class `context` provides context for slot invocations. It associates thread affinity with an instance of a derived class and performs its automatic lifetime tracking.

A `context` object always belongs to the thread that created it. Therefore, all signal emissions targeting this object will be executed in the thread it belongs to (default behavior). This design guarantees absence of any race conditions when the `context`'s slots are invoked as a result of signal emissions from multiple threads;
all slot invocations will be serialized.

**Note!** All actions taken on a `context` object (such as destruction or direct method calls) should be performed in the thread the `context` belongs to.

A class inherited from `context` can provide thread affinity and automatic lifetime tracking not only to its own methods, but also to any callable that is associated with the class instance in `signal::connect()` method.

### Methods:

-----------------------

**`context();`**

Constructs a `context` object and associates it with the calling thread.

_Throws an exception if memory allocation fails._

-----------------------

**`~context() noexcept;`**

Destroys the `context` and releases any acquired resources.

-----------------------

**`context(const context &) = delete;`**

**`context(context &&) = delete;`**

**`context & operator=(const context &) = delete;`**

**`context & operator=(context &&) = delete;`**

`context` is neither copy | move constructible nor copy | move assignable.

-----------------------

## **`enum exec`**

1. **`exec::sync`**
2. **`exec::async`**

Enumeration that specifies slot execution type.

1. Slot is always executed synchronously.
2. Slot is always executed asynchronously.

This enumeration should be used with extreme care! Synchronous execution of slots belonging to different threads may lead to race conditions.

-----------------------

## **Related functions**

1. **`bool signals_execute();`**

2. **`bool signals_execute(unsigned number);`**

Executes signal emissions received in the current thread.

1. Executes at most one slot call.
2. Executes at most **`number`** of slot calls.

This method never blocks; if there are no pending slot calls for the current thread it just returns.

In general, any thread that wishes to receive cross-thread signal emissions just calls this method in a loop. Such design allows easily integrate cross-thread signals into any event loop (for example, window messaging) that exists in a target application or framework.

Returns `true` if at least one slot call has been executed, `false` otherwise.

-----------------------

# Lite version
The **`lite`** (single-threaded) version of the library is located in `vdk::lite` namespace. It provides APIs nearly identical to the multithreaded version. The differences specific to the `lite` version are:

1. There is no `exec` enumeration.
2. There is no thread affinity associated with a `context` instance.
3. All slots are always invoked synchronously.

_Note!_ `lite` version should not be used in a multithreaded environment. Accessing the same library's objects from multiple threads, directly or indirectly, results in catastrophic race conditions and completely unpredictable behavior.

-----------------------

## Notes:

### Memory resources

The library allocates dynamic memory through the centralized global memory resource. By default, this memory resource uses plain `operator new` and `operator delete`. It is possible to install a custom user-provided memory resource instead.

A custom memory resource can be installed through the experimental API - `vdk::memory::signals::memory()` function that accepts and stores pointer to an instance of a class inherited from `vdk::memory::signals::memory_resource`.

_Note!_ The memory resource can be installed only once. This avoids many problems that could otherwise arise from incompatible memory resources trying to deallocate each other's memory.