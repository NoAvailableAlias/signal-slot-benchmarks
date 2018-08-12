
Performance Results
-------------------

**_Higher score is better._** _N / (sample size / count)._

| Library | threaded | combined | emission | connect | destruct | construct | total |
|---------|----------|----------|----------|---------|----------|-----------|-------|
| jeffomatic jl_signal | - | 14402 | 83513 | 66056 | 20536 | 124488 | 308996 |
| Pal Sigslot ST | - | 6131 | 79574 | 13636 | 12923 | 174087 | 286351 |
| mwthinker Signal | - | 3181 | 81663 | 5471 | 8271 | 174375 | 272962 |
| Yassi | - | 2925 | 79241 | 4812 | 8789 | 168052 | 263818 |
| * Pal Sigslot | 1166 | 5440 | 70240 | 10716 | 12937 | 163314 | 263813 |
| Wink-Signals | - | 7271 | 82834 | 15519 | 14730 | 141276 | 261631 |
| * fr00b0 nod | 994 | 3847 | 73628 | 7021 | 9962 | 164957 | 260409 |
| pbhogan Signals | - | 4803 | 52850 | 9975 | 10074 | 179176 | 256877 |
| amc522 Signal11 | - | 3267 | 79348 | 5593 | 9152 | 158937 | 256297 |
| joanrieu signal11 | - | 3978 | 15947 | 10148 | 12002 | 187540 | 229615 |
| * Kosta signals-cpp | 12 | 911 | 27394 | 1279 | 7448 | 192119 | 229164 |
| * lsignal | 883 | 1932 | 72456 | 3114 | 5665 | 142482 | 226531 |
| supergrover sigslot | - | 1330 | 79462 | 4164 | 2092 | 138097 | 225145 |
| EvilTwin Observer | - | 1717 | 47223 | 2741 | 5406 | 141403 | 198489 |
| * winglot Signals | 562 | 2868 | 69693 | 6765 | 5610 | 54596 | 140094 |
| * neosigslot | 410 | 1687 | 13345 | 3871 | 4309 | 90011 | 113634 |
| Boost Signals | - | 1279 | 32548 | 1973 | 5992 | 37808 | 79601 |
| * Boost Signals2 | 56 | 1572 | 17062 | 3668 | 4462 | 15082 | 41902 |

Implementation Info
-------------------

_Size results are the size of object files from release build with Gcc 6.3._

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 37 kb | - | doubly linked list | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 40 kb | X | std::vector | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 42 kb | - | std::list | - |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 43 kb | X | std::set | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 48 kb | X | std::list | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 51 kb | X | std::vector | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 54 kb | - | std::list | - |
| [Pal Sigslot ST](https://github.com/palacaze/sigslot) | 59 kb | X | singly linked list | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 69 kb | X | std::vector | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 72 kb | X | std::vector | - |
| [Boost Signals](http://www.boost.org/doc/libs/1_56_0/doc/html/signals.html) | 96 kb | - | ? | - |
| [winglot Signals](https://github.com/winglot/Signals) | 122 kb | - | *std::list | X |
| [Pal Sigslot](https://github.com/palacaze/sigslot) | 132 kb | X | singly linked list | X |
| [Kosta-Github signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 140 kb | X | std::vector | X |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 148 kb | X | std::vector | X |
| [cpp11nullptr lsignal](https://github.com/cpp11nullptr/lsignal) | 155 kb | X | *std::list | X |
| [neosigslot](http://www.i42.co.uk/stuff/neosigslot.htm) | 224 kb | - | *std::map | X |
| [Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 298 kb | ? | ? | X |

_* Uses additional data structures._

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
