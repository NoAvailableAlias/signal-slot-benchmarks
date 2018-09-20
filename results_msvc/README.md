
# MSVC (Windows)

**_Higher score is better._** _N / (sample size / count)._

### Performance of Thread Safe Libraries

| Library | construct | destruct | connect | emission | combined | threaded | total |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| * nano-signal-slot v2x | 61880 | 7813 | 5825 | 45853 | 3447 | 128 | 124945 |
| ! Pal Sigslot | 63631 | 7825 | 6035 | 41766 | 3446 | 244 | 122948 |
| * fr00b0 nod | 65557 | 9067 | 6651 | 25923 | 3617 | 61 | 110877 |
| ! cpp11nullptr lsignal | 47560 | 3994 | 2220 | 42046 | 1419 | 134 | 97373 |
| * Kosta signals-cpp | 71664 | 6152 | 1779 | 13044 | 1176 | 58 | 93872 |
| * CppFakeIt FastSignals | 55014 | 7437 | 4302 | 18429 | 2496 | 38 | 87716 |
| ! Montellese cpp-signal | 29392 | 5764 | 5400 | 43614 | 2731 | 190 | 87090 |
| * dacap observable | 64656 | 4449 | 6200 | 7994 | 2035 | 19 | 85353 |
| * neolib event | 63534 | 1482 | 1259 | 6465 | 606 | 38 | 73384 |
| ! winglot Signals | 11116 | 3861 | 4038 | 42521 | 1791 | 167 | 63494 |
| * Boost Signals2 | 7870 | 2992 | 2060 | 10448 | 959 | 2 | 24332 |

### Performance of Thread Unsafe Libraries

| Library | construct | destruct | connect | emission | combined | threaded | total |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| jeffomatic jl_signal | 53737 | 17581 | 40136 | 48671 | 11937 | 0 | 172062 |
| nano-signal-slot v1x | 76478 | 10985 | 8743 | 48521 | 4812 | 0 | 149540 |
| Wink-Signals | 72447 | 11593 | 9273 | 50016 | 5206 | 0 | 148535 |
| Montellese cpp-signal | 72580 | 10448 | 8240 | 47651 | 4872 | 0 | 143791 |
| SimpleSignal | 73261 | 9486 | 7735 | 46699 | 4528 | 0 | 141709 |
| nano-signal-slot v2x | 72962 | 10251 | 6579 | 47038 | 4115 | 0 | 140946 |
| Yassi | 72640 | 8797 | 6531 | 48806 | 3894 | 0 | 140667 |
| Pal Sigslot | 71590 | 8231 | 7881 | 44962 | 3900 | 0 | 136564 |
| mwthinker Signal | 62734 | 7518 | 5769 | 47797 | 3346 | 0 | 127165 |
| pbhogan Signals | 61947 | 7069 | 6782 | 47333 | 3550 | 0 | 126682 |
| amc522 Signal11 | 63139 | 7696 | 5426 | 46645 | 3278 | 0 | 126184 |
| vdksoft signals | 63638 | 6659 | 5906 | 46779 | 3185 | 0 | 126166 |
| fr00b0 nod | 70167 | 13608 | 8625 | 27375 | 4918 | 0 | 124693 |
| Ansoulom cpp-observe | 40044 | 12732 | 7594 | 48271 | 4588 | 0 | 113228 |
| joanrieu signal11 | 45049 | 10032 | 8146 | 43754 | 4397 | 0 | 111378 |
| EvilTwin Observer | 64144 | 4459 | 2578 | 20920 | 1586 | 0 | 93687 |
| neolib event | 63633 | 1646 | 1352 | 6643 | 656 | 0 | 73930 |
| supergrover sigslot | 12535 | 2199 | 2651 | 47166 | 1135 | 0 | 65687 |

___
_Size results are the size of object files from release build with Visual Studio 17._

### Metrics of Thread Safe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [* CppFakeIt FastSignals](https://github.com/CppFakeIt/FastSignals) | 764 kb | - | std::vector | X |
| [! Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 780 kb | X | std::forward_list | ! |
| [! Pal Sigslot](https://github.com/palacaze/sigslot) | 783 kb | X | singly linked list | ! |
| [* nano-signal-slot v2x](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 795 kb | X | std::vector | X |
| [* dacap observable](https://github.com/dacap/observable) | 798 kb | - | std::vector | X |
| [! winglot Signals](https://github.com/winglot/Signals) | 812 kb | - | **std::list | ! |
| [* fr00b0 nod](https://github.com/fr00b0/nod) | 870 kb | X | std::vector | X |
| [* Kosta signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 880 kb | X | std::vector | X |
| [! cpp11nullptr lsignal](https://github.com/cpp11nullptr/lsignal) | 985 kb | X | **std::list | ! |
| [* neolib event](https://github.com/i42output/neolib) | 2078 kb | - | **std::unordered_map | X |
| [* Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 2549 kb | - | ? | X |

### Metrics of Thread Unsafe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [nano-signal-slot v1x](https://github.com/NoAvailableAlias/nano-signal-slot/releases/tag/v1.x) | 163 kb | X | singly linked list | - |
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 178 kb | - | doubly linked list | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 207 kb | X | std::vector | - |
| [Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 211 kb | X | std::forward_list | - |
| [vdksoft signals](https://github.com/vdksoft/signals) | 214 kb | - | singly linked list | directive |
| [nano-signal-slot v2x](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 217 kb | X | std::vector | - |
| [Pal Sigslot](https://github.com/palacaze/sigslot) | 221 kb | X | singly linked list | - |
| [SimpleSignal](https://github.com/larspensjo/SimpleSignal) | 242 kb | X | std::vector | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 243 kb | - | std::list | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 253 kb | - | std::list | - |
| [Ansoulom cpp-observe](https://github.com/Ansoulom/cpp-observe) | 266 kb | X | std::vector | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 272 kb | X | std::vector | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 280 kb | X | std::list | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 287 kb | X | std::vector | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 291 kb | X | std::vector | - |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 292 kb | X | std::set | - |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 302 kb | X | std::vector | - |
| [neolib event](https://github.com/i42output/neolib) | 1573 kb | - | **std::unordered_map | - |

| * | ** | ! |
|:-:|:--:|:-:|
| _Library is designed to be thread safe_ | _Uses additional data structures_ | **Library has open thread safety issues** |

Benchmark Algorithms
--------------------

_The individual benchmark algorithms are completely generic through templates._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L27) | Make sure each signal implementation is functioning correctly. |
| [construction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L56) | Time the construction of a Signal to an N number of Foo instances. |
| [destruction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L77) | Time the destruction of a Signal and associated Connections to N number of Foo instances. |
| [connection](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L105) | Time Signal connections to a randomized N number of Foo instances. |
| [emission](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L130) | Time the duration of an N slot emission. |
| [combined](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L157) | Time construction, destruction, connection, and emission together. |
| [threaded](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L182) | Same as the "combined" test except threaded using a shared Signal. |
<br/>
