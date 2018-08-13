
# MSVC (Windows)

**_Higher score is better._** _N / (sample size / count)._

### Performance of Thread Safe Libraries

| Library | construct | destruct | connect | emission | combined | threaded | total |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| * fr00b0 nod | 67259 | 8591 | 6451 | 44790 | 3713 | 251 | 131055 |
| * Pal Sigslot | 68411 | 7486 | 5628 | 42958 | 3246 | 232 | 127962 |
| * nano-signal-slot v2x | 66270 | 4812 | 3815 | 46187 | 2174 | 129 | 123387 |
| * lsignal | 50251 | 3816 | 2122 | 42472 | 1343 | 128 | 100132 |
| * Kosta signals-cpp | 71257 | 5323 | 1321 | 13505 | 954 | 1 | 92360 |
| * winglot Signals | 9846 | 3543 | 3921 | 44499 | 1566 | 163 | 63537 |
| * Boost Signals2 | 7562 | 2940 | 2128 | 10533 | 963 | 3 | 24129 |
| * neolib signal | 5709 | 2882 | 2679 | 9610 | 1039 | 107 | 22027 |

### Performance of Thread Unsafe Libraries

| Library | construct | destruct | connect | emission | combined | threaded | total |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| jeffomatic jl_signal | 55956 | 15743 | 40392 | 49661 | 11576 | 0 | 173327 |
| nano-signal-slot v1x | 75478 | 11375 | 9454 | 49145 | 5567 | 0 | 151019 |
| Wink-Signals | 75247 | 11892 | 8870 | 49544 | 5102 | 0 | 150655 |
| cpp-signal | 75124 | 10209 | 8353 | 48363 | 4847 | 0 | 146897 |
| Yassi | 74469 | 8026 | 6133 | 50331 | 3642 | 0 | 142601 |
| Pal Sigslot ST | 70105 | 7829 | 6886 | 47510 | 3917 | 0 | 136247 |
| nano-signal-slot v2x | 73316 | 6435 | 4497 | 49197 | 2533 | 0 | 135978 |
| vdksoft signals | 65443 | 5917 | 6486 | 49027 | 3072 | 0 | 129944 |
| mwthinker Signal | 64940 | 7051 | 5653 | 48785 | 3273 | 0 | 129703 |
| amc522 Signal11 | 65807 | 7756 | 5526 | 47092 | 3319 | 0 | 129502 |
| joanrieu signal11 | 58101 | 9824 | 8090 | 43983 | 4233 | 0 | 124231 |
| pbhogan Signals | 65729 | 6984 | 6310 | 41281 | 3552 | 0 | 123855 |
| EvilTwin Observer | 64878 | 4335 | 2485 | 20524 | 1545 | 0 | 93767 |
| supergrover sigslot | 11826 | 2043 | 2566 | 48972 | 1089 | 0 | 66496 |
| Boost Signals | 8608 | 3266 | 1214 | 15136 | 760 | 0 | 28985 |
| neolib signal | 6344 | 4793 | 3714 | 11988 | 1442 | 0 | 28281 |

___
_Size results are the size of object files from release build with Visual Studio 17._

### Metrics of Thread Safe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [* Pal Sigslot](https://github.com/palacaze/sigslot) | 840 kb | X | singly linked list | X |
| [* Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 843 kb | X | std::forward_list | X |
| [* nano-signal-slot v2x](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 852 kb | X | std::forward_list | X |
| [* winglot Signals](https://github.com/winglot/Signals) | 869 kb | - | **std::list | X |
| [* fr00b0 nod](https://github.com/fr00b0/nod) | 919 kb | X | std::vector | X |
| [* Kosta signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 925 kb | X | std::vector | X |
| [* cpp11nullptr lsignal](https://github.com/cpp11nullptr/lsignal) | 1035 kb | X | **std::list | X |
| [* neolib signal](https://github.com/i42output/neolib) | 1288 kb | X | **std::unordered_map | X |
| [* Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 2604 kb | - | ? | X |

### Metrics of Thread Unsafe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [nano-signal-slot v1x](https://github.com/NoAvailableAlias/nano-signal-slot) | 164 kb | X | singly linked list | - |
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 179 kb | - | doubly linked list | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 205 kb | X | std::vector | - |
| [vdksoft signals](https://github.com/vdksoft/signals) | 209 kb | - | singly linked list | * |
| [Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 212 kb | X | std::forward_list | - |
| [Pal Sigslot](https://github.com/palacaze/sigslot) | 222 kb | X | singly linked list | - |
| [nano-signal-slot v2x](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 229 kb | X | std::forward_list | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 244 kb | - | std::list | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 247 kb | - | std::list | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 274 kb | X | std::list | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 274 kb | X | std::vector | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 275 kb | X | std::vector | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 282 kb |  |  |  |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 286 kb | X | std::set | - |
| [Boost Signals](http://www.boost.org/doc/libs/1_56_0/doc/html/signals.html) | 455 kb | - | ? | - |
| [neolib signal](https://github.com/i42output/neolib) | 644 kb | X | **std::unordered_map | - |

_* Library is thread safe._
<br/>
_** Uses additional data structures._

Benchmark Algorithms
--------------------

_The individual benchmark algorithms are completely generic through templates._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L21) | Make sure each signal implementation is functioning correctly. |
| [construction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L50) | Time the construction of a Signal to an N number of Foo instances. |
| [destruction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L71) | Time the destruction of a Signal and associated Connections to N number of Foo instances. |
| [connection](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L101) | Time Signal connections to a randomized N number of Foo instances. |
| [emission](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L129) | Time the duration of an N slot emission. |
| [combined](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L159) | Time construction, destruction, connection, and emission together. |
| [threaded](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L186) | Same as the "combined" test except threaded using a shared Signal. |
<br/>
