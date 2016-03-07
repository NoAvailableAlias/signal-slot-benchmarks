# signal-slot-benchmarks

Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+----------------------+------------+-----------+----------+-----------+-----------+---------+
| Library              |  construct |  destruct |  connect |  emission |  combined |  total  |
|----------------------+------------+-----------+----------+-----------+-----------+---------|
| nano-signal-slot     |  157574    |  12679    |  9427    |  45458    |  5143     |  230282 |
| jeffomatic jl_signal |  103475    |  17176    |  43870   |  45252    |  11145    |  220919 |
| Wink-Signals         |  144269    |  16570    |  8967    |  45545    |  5392     |  220742 |
| Yassi                |  144797    |  8408     |  4856    |  45055    |  2865     |  205981 |
| amc522 Signal11      |  122476    |  7795     |  5160    |  41120    |  2946     |  179496 |
| mwthinker Signal     |  110855    |  9081     |  5641    |  45117    |  3342     |  174036 |
| pbhogan Signals      |  111994    |  9204     |  5610    |  41539    |  3513     |  171861 |
| * fr00b0 nod         |  111480    |  9851     |  5762    |  37409    |  3352     |  167854 |
| * Kosta signals-cpp  |  131455    |  8611     |  1228    |  20815    |  977      |  163086 |
| joanrieu signal11    |  93606     |  14324    |  6921    |  39887    |  4067     |  158805 |
| EvilTwin Observer    |  102480    |  5205     |  2103    |  30831    |  1431     |  142050 |
| * lsignal            |  75819     |  4912     |  2089    |  36747    |  1390     |  120958 |
| supergrover sigslot  |  13631     |  2271     |  2795    |  45287    |  1112     |  65097  |
| * winglot Signals    |  11720     |  3973     |  4281    |  37743    |  1698     |  59415  |
| * neosigslot         |  15140     |  4739     |  3577    |  9006     |  1517     |  33979  |
| Boost Signals        |  8746      |  3035     |  937     |  9726     |  613      |  23058  |
| * Boost Signals2     |  7822      |  4705     |  1697    |  7205     |  927      |  22356  |
| martin signals-cpp   |  #N/A      |  #N/A     |  #N/A    |  #N/A     |  #N/A     |  #N/A   |<- FAILS ASSERT
+----------------------+------------+-----------+----------+-----------+-----------+---------+
```
_* Library is designed to be thread safe._

Metrics
-------

_Size results are from Visual Studio 2015 Community using default release build settings._

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [nano-signal-slot](https://github.com/NoAvailableAlias/nano-signal-slot) | 1001 kb | X | singly linked list | - |
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 1055 kb | - | doubly linked list | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 1108 kb | X | std::vector | - |
| [winglot Signals](https://github.com/winglot/Signals) | 1156 kb | - | *std::list | X |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 1164 kb | X | std::set | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 1193 kb | - | std::list | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 1202 kb | - | std::list | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 1212 kb | X | std::vector | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 1221 kb | X | std::vector | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 1243 kb | X | std::vector | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 1250 kb | X | std::list | - |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 1262 kb | X | std::vector | X |
| [cpp11nullptr lsignal](https://github.com/cpp11nullptr/lsignal) | 1498 kb | X | *std::list | X |
| [Boost Signals](http://www.boost.org/doc/libs/1_56_0/doc/html/signals.html) | 1618 kb | - | ? | - |
| ~~[martinkallman signals-cpp](https://github.com/martinkallman/signals-cpp)~~ | ~~1621 kb~~ | ~~X~~ | ~~std::unordered_map~~ | - |
| [Kosta-Github signals-cpp](https://github.com/Kosta-Github/signals-cpp) | 2127 kb | X | std::vector | X |
| [neosigslot](http://www.i42.co.uk/stuff/neosigslot.htm) | 2325 kb | - | *std::map | X |
| [Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 2766 kb | ? | ? | X |

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

Miscellaneous
-------------

Currently working on a new benchmark algorithm method for testing thread safe implementations.
Additionally new benchmark classes will be added for thread safe libs that also support a single thread policy.

