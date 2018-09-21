
# MSVC (Windows)

**_Higher score is better._** _[8, 128] / (sample size / count)._

### Performance of Thread Safe Libraries

| Library | construct | destruct | connect | emission | combined | threaded | total |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| ! Pal Sigslot | 125777 | 8667 | 5887 | 64411 | 3470 | 266 | 208479 |
| * nano-signal-slot v2x | 115402 | 7448 | 5816 | 70373 | 3193 | 76 | 202309 |
| * fr00b0 nod | 129692 | 11296 | 8102 | 35240 | 4309 | 25 | 188664 |
| ! cpp11nullptr lsignal | 102679 | 4372 | 2292 | 65882 | 1481 | 119 | 176824 |
| * Kosta signals-cpp | 149810 | 7234 | 1177 | 16495 | 913 | 46 | 175676 |
| * dacap observable | 139509 | 4617 | 5751 | 10291 | 2075 | 42 | 162284 |
| * CppFakeIt FastSignals | 103698 | 7599 | 5311 | 23588 | 2674 | 26 | 142897 |
| * neolib event | 131924 | 1827 | 1326 | 4238 | 640 | 44 | 140000 |
| ! Montellese cpp-signal | 44846 | 6100 | 6017 | 67446 | 2895 | 178 | 127483 |
| ! winglot Signals | 12309 | 3745 | 4110 | 68162 | 1700 | 149 | 90175 |
| * Boost Signals2 | 11712 | 3443 | 2028 | 13419 | 1061 | 5 | 31669 |
| [Interactive Graph](https://plot.ly/~NoAvailableAlias/1/) |

### Performance of Thread Unsafe Libraries

| Library | construct | destruct | connect | emission | combined | threaded | total |
|---------|-----------|----------|---------|----------|----------|----------|-------|
| nano-signal-slot v1x | 167640 | 12265 | 10105 | 72938 | 5525 | 0 | 268473 |
| jeffomatic jl_signal | 96591 | 18847 | 56596 | 74872 | 12844 | 0 | 259750 |
| Wink-Signals | 153561 | 13170 | 11199 | 76016 | 5672 | 0 | 259618 |
| Montellese cpp-signal | 155507 | 10822 | 9603 | 72912 | 4915 | 0 | 253759 |
| SimpleSignal | 156376 | 10228 | 8712 | 70443 | 4659 | 0 | 250418 |
| Yassi | 154765 | 8708 | 7816 | 73733 | 3785 | 0 | 248808 |
| nano-signal-slot v2x | 147127 | 9740 | 6448 | 71960 | 3781 | 0 | 239056 |
| Pal Sigslot | 144024 | 8719 | 7505 | 68303 | 4090 | 0 | 232642 |
| mwthinker Signal | 136564 | 7595 | 6474 | 73049 | 3448 | 0 | 227130 |
| vdksoft signals | 139598 | 6376 | 4163 | 71449 | 2489 | 0 | 224075 |
| pbhogan Signals | 136437 | 7954 | 7018 | 67331 | 3685 | 0 | 222426 |
| joanrieu signal11 | 125499 | 11445 | 8434 | 65201 | 4718 | 0 | 215296 |
| fr00b0 nod | 141315 | 17913 | 11339 | 37225 | 6080 | 0 | 213872 |
| amc522 Signal11 | 120526 | 8378 | 5681 | 70554 | 3343 | 0 | 208483 |
| EvilTwin Observer | 138807 | 4469 | 2579 | 26669 | 1593 | 0 | 174116 |
| Ansoulom cpp-observe | 65788 | 13888 | 7984 | 72320 | 4999 | 0 | 164979 |
| neolib event | 136577 | 2001 | 1399 | 4223 | 680 | 0 | 144880 |
| supergrover sigslot | 15560 | 1850 | 2755 | 72903 | 1005 | 0 | 94073 |

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
| [* neolib event](https://github.com/i42output/neolib) | 2345 kb | - | **std::unordered_map | X |
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
| [neolib event](https://github.com/i42output/neolib) | 1840 kb | - | **std::unordered_map | - |

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
