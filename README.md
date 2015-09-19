# signal-slot-benchmarks

Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.

Performance
-----------

**_Higher score is better._** _N / (sample size / count)._

```
+-----------------------+------------+-----------+----------+-----------+-----------+---------+
| Library               |  construct |  destruct |  connect |  emission |  combined |  total  |
|-----------------------+------------+-----------+----------+-----------+-----------+---------|
| jeffomatic jl_signal  |  100451    |  16579    |  48720   |  42930    |  11061    |  219740 |
| nano-signal-slot (FT) |  148602    |  12153    |  9304    |  43610    |  5026     |  218696 |
| Wink-Signals          |  141163    |  15649    |  8493    |  44580    |  5014     |  214898 |
| Yassi                 |  140486    |  8136     |  5142    |  43891    |  2916     |  200571 |
| amc522 Signal11       |  117509    |  8185     |  5207    |  40930    |  3024     |  174856 |
| pbhogan Signals       |  110944    |  8934     |  5546    |  42738    |  3496     |  171659 |
| mwthinker Signal      |  110457    |  8639     |  5415    |  42651    |  3221     |  170383 |
| * fr00b0 nod          |  109834    |  9610     |  5928    |  36822    |  3370     |  165564 |
| * Kosta signals-cpp   |  125985    |  8363     |  1194    |  20349    |  981      |  156872 |
| joanrieu signal11     |  92931     |  13150    |  6374    |  37465    |  3816     |  153736 |
| EvilTwin Observer     |  93885     |  5033     |  2034    |  29866    |  1360     |  132178 |
| supergrover sigslot   |  13188     |  2248     |  2767    |  43824    |  1089     |  63116  |
| * winglot Signals     |  11886     |  4181     |  4104    |  36554    |  1749     |  58474  |
| * neosigslot          |  15651     |  4772     |  3301    |  8700     |  1463     |  33888  |
| Boost Signals         |  10207     |  2984     |  912     |  9824     |  609      |  24536  |
| * Boost Signals2      |  8307      |  4534     |  1660    |  7154     |  912      |  22566  |
| martin signals-cpp    |  #N/A      |  #N/A     |  #N/A    |  #N/A     |  #N/A     |  #N/A   |<- ASSERT FAIL
| cpp11nullptr lsignal  |  #N/A      |  #N/A     |  #N/A    |  #N/A     |  #N/A     |  #N/A   |<- TODO
+-----------------------+------------+-----------+----------+-----------+-----------+---------+
```
_* Library is designed to be thread safe._

Metrics
-------

_Size results are from Visual Studio 2015 Community using default release build settings._

| Library | Build Size | Header Only | Data Structure | Thread Safe |
| ------- |:----------:|:-----------:| -------------- |:-----------:|
| [nano-signal-slot (FT)](https://github.com/NoAvailableAlias/nano-signal-slot/tree/FT) | 1001 kb | X | singly linked list | - |
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
