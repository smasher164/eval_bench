# goto eval benchmark

Compare switch case, computed goto, and regular goto + local switch
in benchmark. Compiled with `gcc` and `-O3` for my machine
(`11th Gen Intel(R) Core(TM) i7-1165G7 @ 2.80GHz`/`4 cores`/`64gb ram`).

```
g++ -o eval_bench -O3 main.cpp
```
Ran `10,000,000` iterations with 5 samples.
```
./eval_bench --iters=100000000 --samples=5
```
Which produced the following output:
```
===============================================================================
    Name (* = baseline)   |   Dim   |  Total ms |  ns/op  |Baseline| Ops/second
===============================================================================
    bench_interp_switch * |10000000 |   152.058 |      15 |      - | 65764191.7
       bench_interp_cgoto |10000000 |    74.976 |       7 |  0.493 |133375889.1
        bench_interp_goto |10000000 |    87.024 |       8 |  0.572 |114910990.3
===============================================================================
```

Not only is the goto version nearly equivalent in dispatch overhead to the
computed goto version, the local switch statements get de-duplicated by
the optimizer! I assume it's triggered by common subexpression elimination.

Perhaps this is an alternative strategy for writing interpreters
in languages without tail-call optimization and computed goto.
