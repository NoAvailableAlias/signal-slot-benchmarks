# neolib (subset)

The minimal subset of [neolib](https://github.com/i42output/neolib) needed to build `neolib::event`
(`include/neolib/task/event.hpp`). Files are copied unmodified from neolib, except for
`include/neolib/neolib_export.hpp`, which neolib's own build generates.

Requires C++20 and Boost 1.81 or later (header-only parts of Boost.Unordered, Boost.Lockfree,
Boost.Thread and Boost.Fiber).
