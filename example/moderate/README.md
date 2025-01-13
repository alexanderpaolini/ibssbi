<!-- markdownlint-disable -->
# Fibonacci(n)

This example code calculates fib(N) iteratively.

**Algorithm:**

1. Push N to stack
2. Check if N = 0
    - if it does, print 0 and halt
3. Check if N ≤ 2
    - if it does, print 1 and halt
4. Subtract 2 from N
5. Push 1, 1 to stack

> Resulting stack is `n, f(1), f(2)`

6. Duplicate `f(2)`
7. Rotate stack, putting `f(1)` at the top

> Resulting stack is `n, f(2), f(2), f(1)`

8. Add `f(2)` and `f(1)`

> Resulting stack is `n, f(2), f(3)`

9. Rotate stack, putting `n` at the top
10. Decrement N
11. Check if N = 0 now
    - if it does, print f(3) and halt
12. Rotate stack twice, putting it back in order

> Resulting stack is `n-1, f(2), f(3)`

13. Jump to step 6

> The stack `n-1, f(2), f(3)` is no different from `n, f(1), f(2)`. We can keep jumping back to step 6 until N = 0, which is when we have computed fib(N).

Disassembled program can be found [here](./code.sasm).  
Assembled program can be found [here](./bin).
