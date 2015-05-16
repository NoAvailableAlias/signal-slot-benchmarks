# signal-slot-benchmarks
Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+ -------------------------------------------------------------------------------- +
| Library              |  construct |  destruct |  connect |  emission |  combined |
+ -------------------------------------------------------------------------------- +
| jeffomatic jl_signal |  129043    |  10085    |  47138   |  41156    |  6421     |
| Yassi                |  139720    |  2185     |  1593    |  38778    |  825      |
| mwthinker Signal     |  120239    |  5234     |  1835    |  39795    |  1305     |
| amc522 Signal11      |  119326    |  6033     |  3690    |  33264    |  2084     |
| pbhogan Signals      |  104589    |  5099     |  4595    |  33036    |  2254     |
| EvilTwin Fork        |  110521    |  3946     |  2095    |  19522    |  1255     |
| * fr00b0 nod         |  95052     |  3741     |  2756    |  31873    |  1475     |
| EvilTwin Observer    |  106882    |  2562     |  1223    |  21237    |  811      |
| joanrieu signal11    |  90430     |  6061     |  4104    |  7883     |  1841     |
| supergrover sigslot  |  10690     |  1369     |  2266    |  40484    |  751      |
| Nano-signal-slot     |  12449     |  4060     |  3761    |  29266    |  1655     |
| * winglot Signals    |  5476      |  2055     |  2499    |  29961    |  908      |
| * neosigslot         |  13059     |  2502     |  2312    |  6349     |  936      |
| Boost Signals        |  8060      |  1699     |  542     |  4352     |  357      |
| * Boost Signals2     |  6355      |  1914     |  914     |  3411     |  462      |
+ -------------------------------------------------------------------------------- +

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
