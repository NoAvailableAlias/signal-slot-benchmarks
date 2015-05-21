# signal-slot-benchmarks
Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+---------------------------------------------------------------------------------------------+
| Library               |  construct |  destruct |  connect |  emission |  combined |  total  |
|---------------------------------------------------------------------------------------------|
| jeffomatic jl_signal  |  104011    |  17236    |  50995   |  44984    |  11545    |  228771 |
| Yassi                 |  142993    |  8440     |  5317    |  43289    |  2864     |  202902 |
| * Nano-signal-slot FT |  140575    |  10238    |  3256    |  34277    |  2332     |  190678 |<- ASSERT FAIL
| mwthinker Signal      |  114802    |  13554    |  5681    |  43549    |  3620     |  181207 |
| amc522 Signal11       |  121417    |  8367     |  5405    |  39628    |  3097     |  177913 |
| pbhogan Signals       |  113669    |  9260     |  5824    |  42227    |  3663     |  174643 |
| * fr00b0 nod          |  113194    |  10027    |  6201    |  37862    |  3494     |  170779 |
| joanrieu signal11     |  97149     |  13903    |  7062    |  37037    |  4026     |  159178 |
| EvilTwin Fork         |  105586    |  7801     |  3611    |  30427    |  2299     |  149724 |
| EvilTwin Observer     |  104506    |  5186     |  2085    |  29716    |  1416     |  142909 |
| supergrover sigslot   |  13543     |  2330     |  2840    |  42501    |  1123     |  62337  |
| * winglot Signals     |  12417     |  4254     |  4089    |  35920    |  1757     |  58438  |
| * neosigslot          |  16614     |  4883     |  3368    |  9287     |  1530     |  35682  |
| Boost Signals         |  10564     |  3082     |  948     |  10000    |  628      |  25223  |
| * Boost Signals2      |  8573      |  4669     |  1696    |  7263     |  934      |  23135  |
+---------------------------------------------------------------------------------------------+
```
_* Library is designed to be thread safe._

Size Metrics
------------

_Results are from Visual Studio 2015 RC using default release build settings._

| Library | Object File Size ^ | Header Only |
| ------- | ------------------ |:-----------:|
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 1099 kb | - |
| [Nano-signal-slot (master)](https://github.com/NoAvailableAlias/nano-signal-slot/tree/master) | 1176 kb | X |
| [winglot Signals](https://github.com/winglot/Signals) | 1195 kb | - |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 1209 kb | X |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 1241 kb | - |
| [EvilTwin Fork](https://github.com/NoAvailableAlias/signal-slot-benchmark/blob/master/benchmark/lib/eviltwin/observer_fork.hpp) | 1248 kb | X |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 1250 kb | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 1262 kb | X |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 1271 kb | X |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 1293 kb | X |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 1300 kb | X |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 1307 kb | X |
| [Boost Signals](http://www.boost.org/doc/libs/1_56_0/doc/html/signals.html) | 1676 kb | - |
| [Nano-signal-slot (FT)](https://github.com/NoAvailableAlias/nano-signal-slot/tree/FT) | 1975 kb | X |
| [neosigslot](http://www.i42.co.uk/stuff/neosigslot.htm) | 2397 kb | - |
| [Boost Signals2](http://www.boost.org/doc/libs/1_56_0/doc/html/signals2.html) | 2824 kb | ? |

Benchmark Algorithms
--------------------

_The test size N is doubled for every iteration._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L21) | Assert correct implementation operation. |
| [construction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L46) | Time the construction of a Signal and N number of Foo instances. |
| [destruction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L67) | Time the destruction of a Signal and associated Connections to N number of Foo instances. |
| [connection](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L97) | Time Signal connections to a randomized N number of Foo instances. |
| [emission](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L125) | Time the duration of an N slot emission. |
| [combined](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L155) | Time construction, destruction, connection, and emission together. |
