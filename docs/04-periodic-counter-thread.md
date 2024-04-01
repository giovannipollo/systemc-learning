This examples is really similar to the previous one. The only difference is the usage of `SC_THREAD`. This macro allows to define a thread. The latter is executed only one, and in fact the usual syntax is the following:

```cpp
void count_thread() {
    while (true) {
        // Do something
        wait(10, SC_NS);
    }
}
```

As we can see, inside the thread, there is a `while(true)`. In fact the thread should run indefinetely, and should be put in wait after the execution. The `wait` statement suspend the thread and *allows simulation time to continue*. This is substantially different compared to the `SC_METHOD` *which does not allow simulation time to continue between the invocation and the return of the function*

