# signal-slot-benchmarks
Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+---------------------------------------------------------------------------------------------+
| Library               |  construct |  destruct |  connect |  emission |  combined |  total  |
+---------------------------------------------------------------------------------------------+
| jeffomatic jl_signal  |  98065     |  16566    |  47176   |  42883    |  11087    |  215777 |
| nano-signal-slot (FT) |  146463    |  11773    |  8735    |  42603    |  4772     |  214345 |
| Yassi                 |  138330    |  7890     |  5046    |  41760    |  2747     |  195773 |
| amc522 Signal11       |  114413    |  7891     |  5047    |  38332    |  2897     |  168581 |
| pbhogan Signals       |  107755    |  8652     |  5581    |  41188    |  3491     |  166666 |
| * fr00b0 nod          |  106506    |  9292     |  5730    |  35765    |  3267     |  160559 |
| mwthinker Signal      |  91975     |  7989     |  4848    |  38685    |  2966     |  146462 |
| joanrieu signal11     |  79948     |  12125    |  6256    |  33634    |  3519     |  135483 |
| EvilTwin Fork         |  90853     |  7162     |  3323    |  27778    |  2141     |  131259 |
| EvilTwin Observer     |  93953     |  4538     |  1886    |  28133    |  1347     |  129858 |
| supergrover sigslot   |  12817     |  2174     |  2636    |  41534    |  1039     |  60201  |
| * winglot Signals     |  11641     |  3903     |  3784    |  34914    |  1661     |  55903  |
| * neosigslot          |  13764     |  4152     |  2976    |  8595     |  1459     |  30946  |
| Boost Signals         |  8960      |  2719     |  862     |  8759     |  583      |  21884  |
| * Boost Signals2      |  7730      |  4271     |  1565    |  6601     |  855      |  21021  |
+---------------------------------------------------------------------------------------------+
```
_* Library is designed to be thread safe._

Size Metrics
------------

_Results are from Visual Studio 2015 RC using default release build settings._

| Library | Object File Size ^ | Header Only |
| ------- | ------------------ |:-----------:|
| [Nano-signal-slot (FT)](https://github.com/NoAvailableAlias/nano-signal-slot/tree/FT) | 1043 kb | X |
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 1099 kb | - |
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
