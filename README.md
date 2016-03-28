# signal-slot-benchmarks

Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+----------------------+-----------+----------+---------+----------+----------+----------+---------+
|       Library        | construct | destruct | connect | emission | combined | threaded |  total  |
|----------------------+-----------+----------+---------+----------+----------+----------+---------|
| jeffomatic jl_signal |  104714   |  15898   |  49989  |  44164   |  10631   |  0       |  225397 |
| nano-signal-slot     |  150003   |  10320   |  8416   |  44264   |  3766    |  0       |  216770 |
| Wink-Signals         |  138683   |  14293   |  8043   |  45537   |  4810    |  0       |  211366 |
| Yassi                |  137535   |  7234    |  4993   |  44384   |  2764    |  0       |  196910 |
| amc522 Signal11      |  118923   |  6778    |  4750   |  41339   |  2674    |  0       |  174464 |
| mwthinker Signal     |  109434   |  7886    |  5501   |  43759   |  3160    |  0       |  169740 |
| pbhogan Signals      |  109651   |  7840    |  4926   |  43650   |  3083    |  0       |  169150 |
| * fr00b0 nod         |  110131   |  9629    |  5665   |  37281   |  3301    |  2273    |  168280 |
| * Kosta signals-cpp  |  127607   |  7935    |  1289   |  21165   |  1001    |  2 ?     |  158998 |
| joanrieu signal11    |  92494    |  10611   |  5332   |  38241   |  3099    |  0       |  149778 |
| EvilTwin Observer    |  105578   |  4427    |  1985   |  30969   |  1340    |  0       |  144299 |
| * lsignal            |  73392    |  4176    |  1893   |  36781   |  1255    |  614     |  118110 |
| supergrover sigslot  |  11385    |  2022    |  2587   |  43171   |  1010    |  0       |  60175  |
| * winglot Signals    |  10878    |  3411    |  3410   |  37126   |  1426    |  660     |  56911  |
| * neosigslot         |  14277    |  4054    |  2955   |  8280    |  1312    |  433     |  31310  |
| Boost Signals        |  8667     |  2482    |  883    |  9638    |  565     |  0       |  22235  |
| * Boost Signals2     |  6967     |  3649    |  1405   |  7057    |  798     |  337     |  20214  |
+----------------------+-----------+----------+---------+----------+----------+----------+---------+
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
