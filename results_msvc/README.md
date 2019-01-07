
# MSVC (Windows)

**_Higher score is better._** _[8, 256] / (sample size / count)._

### Performance of Thread Safe Libraries

| Library | [construct] | destruct | connect | emission | combined | threaded | score |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| * nano-signal-slot v2x | 146127 | 6548 | 7626 | 76373 | 3210 | 66 | 93822 |
| ! Pal Sigslot | 173750 | 8760 | 6513 | 71279 | 3677 | 278 | 90508 |
| ! Montellese cpp-signal | 49166 | 5547 | 6189 | 74666 | 2720 | 164 | 89286 |
| ! winglot Signals | 14517 | 3504 | 4123 | 75309 | 1653 | 140 | 84728 |
| ! cpp11nullptr lsignal | 159804 | 4845 | 2343 | 73981 | 1547 | 127 | 82842 |
| * fr00b0 nod | 191200 | 11588 | 8582 | 40919 | 4503 | 27 | 65618 |
| * CppFakeIt FastSignals | 138503 | 5020 | 5608 | 27332 | 2140 | 33 | 40133 |
| * Kosta signals-cpp | 227593 | 7232 | 1025 | 16958 | 808 | 43 | 26065 |
| * dacap observable | 208903 | 4539 | 6317 | 10039 | 2123 | 40 | 23058 |
| * Boost Signals2 | 13516 | 3544 | 2146 | 14152 | 1119 | 6 | 20968 |
| * neolib event | 193905 | 1911 | 1342 | 4524 | 662 | 43 | 8483 |
| [Interactive Graph](https://plot.ly/~NoAvailableAlias/1/) |

### Performance of Thread Unsafe Libraries

| Library | [construct] | destruct | connect | emission | combined | threaded | score |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| jeffomatic jl_signal | 117251 | 15621 | 64947 | 81209 | 10978 | 0 | 172755 |
| Wink-Signals | 223990 | 11666 | 12995 | 83574 | 5405 | 0 | 113639 |
| Ansoulom cpp-observe | 75058 | 12809 | 7957 | 80497 | 4640 | 0 | 105904 |
| Montellese cpp-signal | 230891 | 9537 | 9985 | 78710 | 4568 | 0 | 102800 |
| SimpleSignal | 226410 | 11385 | 8507 | 78096 | 4784 | 0 | 102773 |
| nano-signal-slot v2x | 200184 | 8718 | 8707 | 78646 | 3920 | 0 | 99991 |
| Yassi | 223746 | 5451 | 8446 | 81744 | 2756 | 0 | 98398 |
| amc522 Signal11 | 155427 | 9314 | 5567 | 78607 | 3421 | 0 | 96909 |
| vdksoft signals | 202948 | 12013 | 3582 | 78632 | 2654 | 0 | 96882 |
| mwthinker Signal | 206927 | 6391 | 6236 | 80668 | 3004 | 0 | 96299 |
| Pal Sigslot | 188151 | 8709 | 8088 | 75015 | 4143 | 0 | 95956 |
| joanrieu signal11 | 188207 | 11524 | 8387 | 70308 | 4692 | 0 | 94912 |
| pbhogan Signals | 203434 | 7762 | 6440 | 73569 | 3441 | 0 | 91213 |
| supergrover sigslot | 18972 | 2988 | 2718 | 80122 | 1330 | 0 | 87158 |
| fr00b0 nod | 196915 | 18432 | 11770 | 43156 | 6501 | 0 | 79860 |
| EvilTwin Observer | 205417 | 4087 | 2765 | 27634 | 1536 | 0 | 36022 |
| copperspice cs_signal | 23505 | 1855 | 2012 | 8429 | 838 | 0 | 13134 |
| neolib event | 195453 | 2104 | 1451 | 4455 | 708 | 0 | 8718 |

___
_Size results are the size of object files from release build with Visual Studio 17._

### Metrics of Thread Safe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [* CppFakeIt FastSignals](https://github.com/CppFakeIt/FastSignals) | 748 kb | - | std::vector | X |
| [! Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 754 kb | X | std::forward_list | ! |
| [! Pal Sigslot](https://github.com/palacaze/sigslot) | 758 kb | X | singly linked list | ! |
| [* nano-signal-slot v2x](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 769 kb | X | std::vector | X |
| [* dacap observable](https://github.com/dacap/observable) | 774 kb | - | std::vector | X |
| [! winglot Signals](https://github.com/winglot/Signals) | 787 kb | - | **std::list | ! |
| [* fr00b0 nod](https://github.com/fr00b0/nod) | 845 kb | X | std::vector | X |
| [* Kosta signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 854 kb | X | std::vector | X |
| [! cpp11nullptr lsignal](https://github.com/cpp11nullptr/lsignal) | 958 kb | X | **std::list | ! |
| [* neolib event](https://github.com/i42output/neolib) | 2320 kb | - | **std::unordered_map | X |
| [* Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 2518 kb | - | ? | X |

### Metrics of Thread Unsafe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 152 kb | - | doubly linked list | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 179 kb | X | std::vector | - |
| [Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 185 kb | X | std::forward_list | - |
| [nano-signal-slot v2x](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 187 kb | X | std::vector | - |
| [vdksoft signals](https://github.com/vdksoft/signals) | 188 kb | - | singly linked list | directive |
| [Pal Sigslot](https://github.com/palacaze/sigslot) | 196 kb | X | singly linked list | - |
| [SimpleSignal](https://github.com/larspensjo/SimpleSignal) | 215 kb | X | std::vector | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 216 kb | - | std::list | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 228 kb | - | std::list | - |
| [Ansoulom cpp-observe](https://github.com/Ansoulom/cpp-observe) | 242 kb | X | std::vector | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 244 kb | X | std::vector | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 254 kb | X | std::list | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 261 kb | X | std::vector | - |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 265 kb | X | std::set | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 267 kb | X | std::vector | - |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 277 kb | X | std::vector | - |
| [copperspice cs_signal](https://github.com/copperspice/cs_signal) | 452 kb | - | **RCU List | DIY |
| [neolib event](https://github.com/i42output/neolib) | 1815 kb | - | **std::unordered_map | - |

| * | ** | ! | [] |
|:-:|:--:|:-:|:--:|
| _Supports thread-safety_ | _Uses other containers_ | **Has thread-safety issues** | Excluded from final score |

Benchmark Algorithms
--------------------

_The individual benchmark algorithms are completely generic through templates._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L19) | Make sure each signal implementation is functioning correctly. |
| [construction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L48) | Time the construction of a Signal to an N number of Foo instances. |
| [destruction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L69) | Time the destruction of a Signal and associated Connections to N number of Foo instances. |
| [connection](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L95) | Time Signal connections to a randomized N number of Foo instances. |
| [emission](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L118) | Time the duration of an N slot emission. |
| [combined](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L143) | Time construction, destruction, connection, and emission together. |
| [threaded](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L166) | Same as the "combined" test except threaded using a shared Signal. |
<br/>
