# goto eval benchmark

Compare switch case, computed goto, tail calls, and regular goto + local switch
in benchmark. Compiled with `gcc` and `-O3` for my machine
(`11th Gen Intel(R) Core(TM) i7-1165G7 @ 2.80GHz`/`4 cores`/`64gb ram`).

```
g++ -o eval_bench -O3 main.cpp
```
Ran `100` iterations with 5 samples.
```
$ ./eval_bench --iters=100 --samples=5
===============================================================================
    Name (* = baseline)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
    bench_interp_switch * |     100 |    58.306 |  583062 |      - |     1715.1
       bench_interp_cgoto |     100 |    51.521 |  515209 |  0.884 |     1941.0
        bench_interp_goto |     100 |    59.076 |  590755 |  1.013 |     1692.7
    bench_interp_tailcall |     100 |    59.049 |  590487 |  1.013 |     1693.5
===============================================================================

Not sure why the tailcall version isn't faster than baseline.
