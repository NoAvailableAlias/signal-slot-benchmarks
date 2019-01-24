
# MSVC (Windows)

**_Higher score is better._** _[8, 256] / (sample size / count)._

### Performance of Thread Safe Libraries

| Library | [constr] | [destr] | conn | disconn | reconn | emit | all | threaded | score |
|---------|----------|---------|------|---------|--------|------|----------|----------|-------|
| * nano-signal-slot ts | 145061 | 13522 | 7356 | 8161 | 9562 | 78441 | 4334 | 147 | 108000 |
| ! Montellese cpp-signal | 48280 | 7053 | 5909 | 5425 | 5934 | 74646 | 2499 | 164 | 94576 |
| ! nano-signal-slot tss | 145724 | 13676 | 7305 | 8179 | 9274 | 60377 | 4157 | 142 | 89435 |
| ! winglot Signals | 13978 | 4423 | 4015 | 3388 | 4074 | 75892 | 1620 | 130 | 89119 |
| * fr00b0 nod | 189352 | 36229 | 8469 | 12168 | 14357 | 37221 | 5901 | 25 | 78141 |
| * palacaze sigslot | 172249 | 12379 | 5763 | 16827 | 5270 | 20606 | 3605 | 257 | 52329 |
| * CppFakeIt FastSignals | 137485 | 35360 | 5482 | 5562 | 9419 | 27008 | 2935 | 29 | 50436 |
| * Kosta signals-cpp | 218117 | 7048 | 1012 | 19526 | 930 | 16817 | 823 | 43 | 39151 |
| * Boost Signals2 | 11975 | 3435 | 2158 | 16607 | 1895 | 13830 | 61 | 6 | 34558 |
| * neolib event | 189786 | 5824 | 1319 | 2147 | 1396 | 4502 | 715 | 44 | 10123 |
| [Interactive Graph](https://plot.ly/~NoAvailableAlias/1/) (NOT UPDATED YET) |

### Performance of Thread Unsafe Libraries

| Library | [constr] | [destr] | conn | disconn | reconn | emit | all | threaded | score |
|---------|----------|---------|------|---------|--------|------|----------|----------|-------|
| jeffomatic jl_signal | 115893 | 46854 | 66126 | 15442 | 65516 | 81337 | 11026 | 0 | 239449 |
| Ansoulom cpp-observe | 73095 | 24516 | 7525 | 13246 | 10444 | 79439 | 5590 | 0 | 116242 |
| nano-signal-slot st | 196514 | 18487 | 8587 | 10325 | 11419 | 79847 | 5505 | 0 | 115684 |
| Montellese cpp-signal | 229122 | 15266 | 9474 | 9239 | 9474 | 80378 | 4440 | 0 | 113004 |
| Yassi | 226784 | 15753 | 8091 | 5508 | 14336 | 81096 | 3568 | 0 | 112600 |
| fr00b0 nod | 196181 | 36310 | 12130 | 19105 | 26506 | 38401 | 9665 | 0 | 105807 |
| mwthinker Signal | 199555 | 9283 | 5968 | 6183 | 5897 | 80266 | 2852 | 0 | 101166 |
| vdksoft signals | 197301 | 16120 | 4008 | 12086 | 3893 | 78225 | 2650 | 0 | 100862 |
| amc522 Signal11 | 152987 | 11159 | 5302 | 9121 | 5397 | 76877 | 3354 | 0 | 100052 |
| pbhogan Signals | 201551 | 10391 | 6459 | 7687 | 6341 | 72442 | 3426 | 0 | 96355 |
| supergrover sigslot | 17998 | 3299 | 2620 | 2989 | 2615 | 79977 | 1285 | 0 | 89486 |
| palacaze sigslot | 187073 | 13109 | 7097 | 16849 | 6222 | 21285 | 4007 | 0 | 55460 |
| EvilTwin Observer | 203020 | 5744 | 2614 | 3963 | 3046 | 26889 | 1611 | 0 | 38123 |
| copperspice cs_signal | 23309 | 1923 | 1874 | 1755 | 1941 | 8321 | 804 | 0 | 14695 |
| neolib event | 190989 | 5905 | 1432 | 2397 | 1528 | 4464 | 767 | 0 | 10589 |

___
_Size results are the size of object files from release build with Visual Studio 17._

### Metrics of Thread Safe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [* nano-signal-slot ts](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 756 kb | X | std::vector | X |
| [* CppFakeIt FastSignals](https://github.com/CppFakeIt/FastSignals) | 767 kb | - | std::vector | X |
| [! Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 777 kb | X | std::forward_list | ! |
| [! nano-signal-slot tss](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 780 kb | X | std::vector | ! |
| [! winglot Signals](https://github.com/winglot/Signals) | 806 kb | - | **std::list | ! |
| [* palacaze sigslot](https://github.com/palacaze/sigslot) | 830 kb | X | singly linked list | X |
| [* fr00b0 nod](https://github.com/fr00b0/nod) | 864 kb | X | std::vector | X |
| [* Kosta signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 873 kb | X | std::vector | X |
| [* neolib event](https://github.com/i42output/neolib) | 2338 kb | - | **std::unordered_map | X |
| [* Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 2587 kb | - | ? | X |

### Metrics of Thread Unsafe Libraries

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 167 kb | - | doubly linked list | - |
| [nano-signal-slot st](https://github.com/NoAvailableAlias/nano-signal-slot/tree/rework) | 169 kb | X | std::vector | - |
| [Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 202 kb | X | std::forward_list | - |
| [vdksoft signals](https://github.com/vdksoft/signals) | 211 kb | - | singly linked list | directive |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 229 kb | - | std::list | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 248 kb | - | std::list | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 266 kb | X | std::vector | - |
| [Ansoulom cpp-observe](https://github.com/Ansoulom/cpp-observe) | 267 kb | X | std::vector | - |
| [palacaze sigslot](https://github.com/palacaze/sigslot) | 270 kb | X | singly linked list | - |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 284 kb | X | std::set | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 289 kb | X | std::vector | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 292 kb | X | std::vector | - |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 294 kb | X | std::vector | - |
| [copperspice cs_signal](https://github.com/copperspice/cs_signal) | 470 kb | - | **RCU List | DIY |
| [neolib event](https://github.com/i42output/neolib) | 1831 kb | - | **std::unordered_map | - |

| * | ** | ! | [] |
|:-:|:--:|:-:|:--:|
| _Supports thread-safety_ | _Uses other containers_ | **Has thread-safety issues** | Excluded from final score |

Benchmark Algorithms
--------------------

_The individual benchmark algorithms are completely generic through the use of templates._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L19) | Make sure each signal implementation is functioning correctly. |
| [construction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L48) | Time the construction of a Signal to an N number of Foo instances. |
| [destruction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L69) | Time the destruction of a Signal followed by N number of Foo instances. |
| [connection](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L95) | Time Signal connections to a randomized N number of Foo instances. |
| [disconnect](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L118) | Time disconnecting N number of Foo instances from a single Signal. |
| [reconnect](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L145) | Time reconnecting N number of Foo instances to a global Signal. |
| [emit](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L169) | Time the duration of an N slot emission. |
| [all](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L194) | Time all previous benchmarks together in one combined benchmark. |
| [threaded](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L218) | Same as the previous benchmark but is now threaded. |
<br/>
