
# MSVC (Windows)

**_Higher score is better._** _[2, 64] / [(sample size / count)](../../benchmark.hpp#L11)._

### Performance of Thread Safe Libraries

| Library | [constr] | [destr] | conn | disconn | reconn | emit | all | threaded | score |
|---------|----------|---------|------|---------|--------|------|----------|----------|-------|
| * nano-signal-slot ts | 39557 | 14125 | 10594 | 17476 | 16822 | 129100 | 8274 | 642 | 182908 |
| * palacaze sigslot | 4196 | 3153 | 9367 | 12258 | 14673 | 107786 | 6208 | 535 | 150826 |
| * Montellese cpp-signal | 18760 | 5905 | 8737 | 10005 | 9056 | 115558 | 4610 | 516 | 148483 |
| * cpp11nullptr lsignal | 2135 | 2668 | 4357 | 7860 | 4275 | 112647 | 2808 | 399 | 132344 |
| * fr00b0 nod | 17703 | 17011 | 12324 | 22018 | 26667 | 54304 | 10161 | 396 | 125871 |
| * KtnSignalSlot | 6070 | 4856 | 2095 | 9097 | 2546 | 104033 | 1760 | 85 | 119616 |
| * CppFakeIt FastSignals | 4540 | 4545 | 7666 | 17374 | 17561 | 46393 | 7289 | 140 | 96425 |
| * Kosta signals-cpp | 19775 | 6384 | 3841 | 43041 | 3261 | 36403 | 2686 | 322 | 89554 |
| * nano-signal-slot tss | 4464 | 3013 | 8727 | 7416 | 13815 | 23167 | 3580 | 373 | 57079 |
| * tripleslash rocket | 913 | 1195 | 6632 | 9722 | 6649 | 25859 | 3488 | 400 | 52750 |
| * dacap observable | 14061 | 1432 | 11004 | 9531 | 10932 | 15833 | 3880 | 40 | 51219 |
| * Boost Signals2 | 316 | 598 | 3439 | 7064 | 2900 | 28426 | 1878 | 160 | 43866 |
| * neolib event | 17817 | 649 | 2291 | 4268 | 2708 | 5854 | 1258 | 91 | 16470 |

### Performance of Thread Unsafe Libraries

| Library | [constr] | [destr] | conn | disconn | reconn | emit | all | threaded | score |
|---------|----------|---------|------|---------|--------|------|----------|----------|-------|
| jeffomatic jl_signal | 31498 | 24112 | 106333 | 42418 | 105078 | 136499 | 24950 | 0 | 415278 |
| vdksoft signals st | 3751 | 5419 | 10467 | 162614 | 6152 | 125584 | 4758 | 0 | 309575 |
| Nuclex Events | 67049 | 63226 | 23401 | 22885 | 22320 | 135934 | 11067 | 0 | 215607 |
| Wink-Signals | 51907 | 64460 | 14565 | 24157 | 27097 | 136024 | 12145 | 0 | 213988 |
| nano-signal-slot st | 51845 | 29221 | 11604 | 20100 | 19311 | 135631 | 9766 | 0 | 196413 |
| Yassi | 52005 | 39981 | 10653 | 15775 | 23361 | 135338 | 9154 | 0 | 194281 |
| palacaze sigslot | 38989 | 19400 | 12778 | 16132 | 21009 | 134924 | 8994 | 0 | 193838 |
| Ansoulom cpp-observe | 51788 | 37464 | 9674 | 23671 | 14719 | 135312 | 8473 | 0 | 191849 |
| Montellese cpp-signal | 62018 | 23367 | 13639 | 16372 | 13829 | 135478 | 7617 | 0 | 186934 |
| fr00b0 nod | 30259 | 22519 | 16246 | 35819 | 50960 | 58418 | 16242 | 0 | 177685 |
| tripleslash rocket st | 1706 | 2756 | 11152 | 18416 | 11216 | 130377 | 6473 | 0 | 177635 |
| mwthinker Signal | 5069 | 6781 | 10556 | 13158 | 10128 | 137227 | 5639 | 0 | 176708 |
| pbhogan Signals | 4364 | 6876 | 12674 | 12002 | 12490 | 133215 | 5987 | 0 | 176367 |
| amc522 Signal11 | 27037 | 33226 | 9820 | 15072 | 10277 | 130864 | 5741 | 0 | 171775 |
| SimpleSignal | 4897 | 6766 | 9994 | 9745 | 9020 | 137345 | 4619 | 0 | 170724 |
| joanrieu signal11 | 2678 | 3837 | 13051 | 17369 | 13122 | 107489 | 6980 | 0 | 158011 |
| supergrover sigslot | 2623 | 3664 | 4458 | 5027 | 4210 | 134384 | 2106 | 0 | 150185 |
| nano-signal-slot sts | 51810 | 30695 | 11852 | 20611 | 19506 | 83177 | 8785 | 0 | 143932 |
| iscool::signals | 41598 | 47904 | 9460 | 42772 | 9668 | 43760 | 346 | 0 | 106007 |
| EvilTwin Observer | 4444 | 4692 | 4319 | 8106 | 5196 | 66553 | 3197 | 0 | 87371 |
| libsigcplusplus | 39133 | 37808 | 1931 | 3174 | 2005 | 72139 | 1201 | 0 | 80450 |
| Boost Signals2 st | 286 | 592 | 3655 | 9734 | 3097 | 44228 | 2265 | 0 | 62979 |
| copperspice cs_signal | 8071 | 2266 | 2985 | 3018 | 3021 | 13124 | 1347 | 0 | 23495 |
| neolib event | 18640 | 664 | 2488 | 4842 | 2908 | 5974 | 1344 | 0 | 17556 |

___
_Size results are the size of object files from x64 release build using Visual Studio 19._

### Metrics of Thread Safe Libraries

| Library | Build Size | Signal Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:|:-----------:| -------------- |:-----------:|
| [* dacap observable](https://github.com/dacap/observable) | 845 kb | 184 b | - | std::vector | X |
| [* CppFakeIt FastSignals](https://github.com/CppFakeIt/FastSignals) | 855 kb | 16 b | - | std::vector | X |
| [* nano-signal-slot ts](https://github.com/NoAvailableAlias/nano-signal-slot) | 856 kb | 32 b | X | std::vector | X |
| [* nano-signal-slot tss](https://github.com/NoAvailableAlias/nano-signal-slot) | 873 kb | 48 b | X | std::vector | X |
| [* Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 874 kb | 96 b | X | std::forward_list | X |
| [* Kosta signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 915 kb | 96 b | X | std::vector | X |
| [* palacaze sigslot](https://github.com/palacaze/sigslot) | 917 kb | 104 b | X | std::vector | X |
| [* fr00b0 nod](https://github.com/fr00b0/nod) | 924 kb | 144 b | X | std::vector | X |
| [* cpp11nullptr lsignal](https://github.com/cpp11nullptr/lsignal) | 998 kb | 128 b | X | **std::list | X |
| [* KtnSignalSlot](https://gitlab.com/KtnFramework/Libraries/KtnSignalSlot.git) | 1071 kb | 304 b | X | std::vector | X |
| [* tripleslash rocket](https://github.com/tripleslash/rocket) | 1109 kb | 24 b | X | intrusive list | X |
| [* Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 2158 kb | 32 b | - | ? | X |
| [* neolib event](https://github.com/i42output/neolib) | 2582 kb | 136 b | - | **std::unordered_map | X |

### Metrics of Thread Unsafe Libraries

| Library | Build Size | Signal Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:|:-----------:| -------------- |:-----------:|
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 210 kb | 40 b | - | doubly linked list | - |
| [vdksoft signals st](https://github.com/vdksoft/signals) | 223 kb | 8 b | - | doubly linked list | - |
| [nano-signal-slot st](https://github.com/NoAvailableAlias/nano-signal-slot) | 235 kb | 24 b | X | std::vector | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 246 kb | 24 b | X | std::vector | - |
| [Montellese cpp-signal](https://github.com/Montellese/cpp-signal) | 247 kb | 16 b | X | std::forward_list | - |
| [Nuclex Events](http://blog.nuclex-games.com/2019/10/nuclex-signal-slot-benchmarks/) | 253 kb | 40 b | X | dynamic array | - |
| [nano-signal-slot sts](https://github.com/NoAvailableAlias/nano-signal-slot) | 254 kb | 24 b | X | std::vector | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 276 kb | 40 b | - | std::list | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 285 kb | 32 b | - | std::list | - |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 293 kb | 16 b | X | std::set | - |
| [libsigcplusplus](https://github.com/libsigcplusplus/libsigcplusplus) | 295 kb | 16 b | - | std::list | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 316 kb | 32 b | X | std::list | - |
| [palacaze sigslot](https://github.com/palacaze/sigslot) | 319 kb | 48 b | X | std::vector | - |
| [Ansoulom cpp-observe](https://github.com/Ansoulom/cpp-observe) | 321 kb | 24 b | X | std::vector | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 322 kb | 24 b | X | std::vector | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 335 kb | 40 b | X | std::vector | - |
| [SimpleSignal](https://github.com/larspensjo/SimpleSignal) | 346 kb | 16 b | X | std::vector | - |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 355 kb | 72 b | X | std::vector | - |
| [tripleslash rocket st](https://github.com/tripleslash/rocket) | 372 kb | 24 b | X | intrusive list | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 387 kb | 16 b | X | std::vector | - |
| [iscool::signals](https://github.com/IsCoolEntertainment/iscool-core/) | 459 kb | 64 b | - | std::vector | - |
| [copperspice cs_signal](https://github.com/copperspice/cs_signal) | 482 kb | 128 b | - | **RCU List | DIY |
| [Boost Signals2 st](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 1561 kb | 32 b | - | ? | - |
| [neolib event](https://github.com/i42output/neolib) | 2027 kb | 136 b | - | **std::unordered_map | - |

| * | ** | [] |
|:-:|:--:|:--:|
| _Supports thread-safety_ | _Uses additional containers_ | Excluded from scoring |

Benchmark Algorithms
--------------------

_The individual benchmark algorithms are completely generic through the use of templates._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](../../benchmark.hpp#L19) | Make sure each signal implementation is functioning correctly. |
| [construction](../../benchmark.hpp#L48) | Sample the default construction of N * N number of Signal instances. |
| [destruction](../../benchmark.hpp#L66) | Sample the destruction of N * N number of Signal instances. |
| [connection](../../benchmark.hpp#L85) | Sample Signal connections to N number of Foo instances. |
| [disconnect](../../benchmark.hpp#L108) | Sample disconnecting N number of Foo instances from a single Signal. |
| [reconnect](../../benchmark.hpp#L135) | Sample reconnecting N number of Foo instances to a global Signal. |
| [emit](../../benchmark.hpp#L159) | Sample the duration of an N slot emission. |
| [all](../../benchmark.hpp#L184) | Sample all previous benchmarks together in one combined benchmark. |
| [threaded](../../benchmark.hpp#L208) | Same as the previous benchmark but is now threaded. |
<br/>
