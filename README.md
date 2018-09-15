
# signal-slot-benchmarks

Comprehensive benchmarks for a majority of GitHub c++ signal slot implementations and others.
<br/>
**_Boost, as well as C++17 support, is required to build this project._**

Foreword
--------

These benchmarks do not accurately depict real world usage patterns seen in the wild.
<br/>
However, these benchmarks were designed to provide impartial results regardless of implementation.
<br/>
It is my goal to take what I've learned from these results and apply this knowledge into making nano-signal-slot better.

Tested Environments
-------------------

<div style="text-align:center; margin-bottom:1em;">click to view results</div>
<div style="text-align:center; display:flex;">
	<div style="flex:1;">
    	<a href="https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/results_msvc/README.md">
        	<button type="button" style="padding:2em; cursor:pointer;  font-size:2em; width:90%;">
            	MSVC (Windows)
            </button>
        </a>
	</div>
	<div style="flex:1;">
        <a href="https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/results_gcc/README.md">
            <button type="button" style="padding:2em; cursor:pointer; font-size:2em; width:90%;">
            	GCC (Linux)
            </button>
        </a>
	</div>
</div>

Maintainers
-----------

Generally, implementations that get added to this benchmark project aren't regularly updated by me unless asked to do so.
If you are a maintainer of a referenced library, I encourage you to fork this project and create pull requests to expedite updates.

Benchmark Algorithms
--------------------

_The individual benchmark algorithms are completely generic through templates._

| Algorithm | Description |
| --------- | ----------- |
| [validation_assert](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L27) | Make sure each signal implementation is functioning correctly. |
| [construction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L56) | Time the construction of a Signal to an N number of Foo instances. |
| [destruction](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L77) | Time the destruction of a Signal and associated Connections to N number of Foo instances. |
| [connection](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L105) | Time Signal connections to a randomized N number of Foo instances. |
| [emission](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L130) | Time the duration of an N slot emission. |
| [combined](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L157) | Time construction, destruction, connection, and emission together. |
| [threaded](https://github.com/NoAvailableAlias/signal-slot-benchmarks/blob/master/benchmark.hpp#L182) | Same as the "combined" test except threaded using a shared Signal. |
<br/>
