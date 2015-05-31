# signal-slot-benchmarks

Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+---------------------------------------------------------------------------------------------+
| Library               |  construct |  destruct |  connect |  emission |  combined |  total  |
+---------------------------------------------------------------------------------------------+
| jeffomatic jl_signal  |  104934    |  17479    |  50983   |  45051    |  11702    |  230150 |
| nano-signal-slot (FT) |  152362    |  12718    |  9576    |  45219    |  5235     |  225110 |
| Wink-Signals          |  146193    |  16181    |  8835    |  45903    |  5186     |  222298 |
| Yassi                 |  144775    |  8428     |  5295    |  44441    |  2945     |  205884 |
| amc522 Signal11       |  120039    |  8358     |  5383    |  41713    |  3100     |  178592 |
| mwthinker Signal      |  115446    |  9093     |  5682    |  44037    |  3338     |  177595 |
| pbhogan Signals       |  113131    |  9275     |  5776    |  43828    |  3624     |  175634 |
| * fr00b0 nod          |  112814    |  9919     |  6194    |  38097    |  3490     |  170514 |
| joanrieu signal11     |  97472     |  13991    |  7014    |  39942    |  4077     |  162494 |
| EvilTwin Observer     |  106043    |  5157     |  2101    |  31531    |  1412     |  146246 |
| supergrover sigslot   |  13798     |  2342     |  2887    |  43736    |  1146     |  63910  |
| * winglot Signals     |  12355     |  4321     |  4322    |  37698    |  1814     |  60509  |
| * neosigslot          |  16288     |  4927     |  3395    |  9019     |  1517     |  35146  |
| Boost Signals         |  10388     |  3087     |  948     |  10016    |  627      |  25067  |
| * Boost Signals2      |  8579      |  4630     |  1695    |  7354     |  941      |  23199  |
| martin signls-cpp     |  #N/A      |  #N/A     |  #N/A    |  #N/A     |  #N/A     |  #N/A   |<- ASSERT FAIL
| * Kosta signls-cpp    |  #N/A      |  #N/A     |  #N/A    |  #N/A     |  #N/A     |  #N/A   |<- FAILS TO COMPILE
+---------------------------------------------------------------------------------------------+
```
_* Library is designed to be thread safe._

Metrics
-------

_Size results are from Visual Studio 2015 RC using default release build settings._

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [nano-signal-slot (FT)](https://github.com/NoAvailableAlias/nano-signal-slot/tree/FT) | 1043 kb | X | singly linked list | - |
| [jeffomatic jl_signal](https://github.com/jeffomatic/jl_signal) | 1099 kb | - | doubly linked list | - |
| [Wink-Signals](https://github.com/miguelmartin75/Wink-Signals) | 1153 kb | X | std::vector | - |
| [winglot Signals](https://github.com/winglot/Signals) | 1195 kb | - | *std::list | X |
| [pbhogan Signals](https://github.com/pbhogan/Signals) | 1209 kb | X | std::set | - |
| [mwthinker Signal](https://github.com/mwthinker/Signal) | 1241 kb | - | std::list | - |
| [supergrover sigslot](https://github.com/supergrover/sigslot) | 1250 kb | - | std::list | - |
| [amc522 Signal11](https://github.com/amc522/Signal11) | 1262 kb | X | std::vector | - |
| [EvilTwin Observer](http://eviltwingames.com/blog/the-observer-pattern-revisited/) | 1271 kb | X | std::vector | - |
| [Yassi](http://www.codeproject.com/Articles/867044/Yassi-Yet-Another-Signal-Slot-Implementation) | 1293 kb | X | std::vector | - |
| [joanrieu signal11](https://github.com/joanrieu/signal11) | 1300 kb | X | std::list | - |
| [fr00b0 nod](https://github.com/fr00b0/nod) | 1307 kb | X | std::vector | X |
| ~~[martinkallman signals-cpp](https://github.com/martinkallman/signals-cpp)~~ | ~~1676 kb~~ | ~~X~~ | ~~std::unordered_map~~ | - |
| [Boost Signals](http://www.boost.org/doc/libs/1_56_0/doc/html/signals.html) | 1677 kb | - | ? | - |
| [neosigslot](http://www.i42.co.uk/stuff/neosigslot.htm) | 2397 kb | - | *std::map | X |
| [Boost Signals2](http://www.boost.org/doc/libs/1_58_0/doc/html/signals2.html) | 2824 kb | ? | ? | X |
| ~~[Kosta-Github signals-cpp](https://github.com/Kosta-Github/signals-cpp)~~ | ~~?~~ | ~~X~~ | ~~std::vector~~ | ~~X~~ |

_* Uses additional data structures._

Benchmark Algorithms
--------------------

_The individual benchmark algorithms are completely generic through templates._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L21) | Assert correct implementation operation. |
| [construction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L50) | Time the construction of a Signal to an N number of Foo instances. |
| [destruction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L71) | Time the destruction of a Signal and associated Connections to N number of Foo instances. |
| [connection](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L101) | Time Signal connections to a randomized N number of Foo instances. |
| [emission](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L129) | Time the duration of an N slot emission. |
| [combined](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L159) | Time construction, destruction, connection, and emission together. |
