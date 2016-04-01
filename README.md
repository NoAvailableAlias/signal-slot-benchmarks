# signal-slot-benchmarks

Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+----------------------+-----------+----------+---------+----------+----------+----------+--------+
| Library              | construct | destruct | connect | emission | combined | threaded | total  |
|----------------------+-----------+----------+---------+----------+----------+----------+--------|
| jeffomatic jl_signal | 91534     | 15857    | 46827   | 42811    | 10378    | 0        | 207408 |
| nano-signal-slot     | 129634    | 10684    | 8466    | 41656    | 3780     | 0        | 194220 |
| Wink-Signals         | 112965    | 13856    | 8113    | 43101    | 4873     | 0        | 182908 |
| Yassi                | 120136    | 8222     | 5199    | 42372    | 2934     | 0        | 178864 |
| amc522 Signal11      | 105469    | 6892     | 5022    | 38340    | 2766     | 0        | 158489 |
| pbhogan Signals      | 96434     | 7930     | 5516    | 41878    | 3187     | 0        | 154945 |
| mwthinker Signal     | 93651     | 7715     | 5330    | 41322    | 3016     | 0        | 151036 |
| * fr00b0 nod         | 93510     | 9096     | 5592    | 33594    | 3168     | 2004     | 146964 |
| * Kosta signals-cpp  | 109182    | 7632     | 1494    | 19763    | 1089     | 2        | 139161 |
| joanrieu signal11    | 80858     | 10721    | 5886    | 36803    | 3124     | 0        | 137392 |
| EvilTwin Observer    | 90964     | 4467     | 2034    | 28567    | 1333     | 0        | 127366 |
| * lsignal            | 64299     | 4218     | 1998    | 33401    | 1203     | 614      | 105733 |
| supergrover sigslot  | 12332     | 2142     | 2734    | 40634    | 1032     | 0        | 58874  |
| * winglot Signals    | 10962     | 3527     | 3822    | 34071    | 1430     | 679      | 54492  |
| * neosigslot         | 12880     | 4132     | 3319    | 7852     | 1366     | 503      | 30051  |
| Boost Signals        | 6963      | 2299     | 888     | 9096     | 537      | 0        | 19784  |
| * Boost Signals2     | 6490      | 3521     | 1474    | 6732     | 762      | 337      | 19316  |
+----------------------+-----------+----------+---------+----------+----------+----------+--------+
```
_* Library is designed to be thread safe._

Metrics
-------

_Size results are from Visual Studio 2015 Community using default release build settings._

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [nano-signal-slot](https://github.com/NoAvailableAlias/nano-signal-slot) | 3654 kb | X | singly linked list | - |
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 3710 kb | - | doubly linked list | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 3758 kb | X | std::vector | - |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 3813 kb | X | std::set | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 3834 kb | - | std::list | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 3840 kb | - | std::list | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 3851 kb | X | std::vector | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 3863 kb | X | std::vector | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 3883 kb | X | std::vector | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 3888 kb | X | std::list | - |
| [winglot Signals](https://github.com/winglot/Signals) | 3978 kb | - | *std::list | X |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 4079 kb | X | std::vector | X |
| [Kosta-Github signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 4091 kb | X | std::vector | X |
| ~~[martinkallman signals-cpp](https://github.com/martinkallman/signals-cpp)~~ | ~~4245 kb~~ | ~~X~~ | ~~std::unordered_map~~ | - |
| [Boost Signals](http://www.boost.org/doc/libs/1_56_0/doc/html/signals.html) | 4252 kb | - | ? | - |
| [cpp11nullptr lsignal](https://github.com/cpp11nullptr/lsignal) | 4306 kb | X | *std::list | X |
| [neosigslot](http://www.i42.co.uk/stuff/neosigslot.htm) | 5167 kb | - | *std::map | X |
| [Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 5606 kb | ? | ? | X |

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
| [threaded](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L186) | Time the runtime of parallel slot creation, connection, emission, and destruction. |

Notes
-----

Metrics entries that are crossed out are implementations that have failed the pre-benchmark validation assert.
