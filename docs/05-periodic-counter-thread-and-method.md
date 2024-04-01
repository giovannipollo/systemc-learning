This examples shows the usage of both method and thread. 

```
Counter initialized
TIME@ 0 s METHOD: Counter value: 0
Incrementing counter inside METHOD
TIME @0 s THREAD: Counter value: 1
Incrementing counter inside THREAD
TIME @10 ns THREAD: Counter value: 2
Incrementing counter inside THREAD
TIME@ 20 ns METHOD: Counter value: 3
Incrementing counter inside METHOD
TIME @20 ns THREAD: Counter value: 4
Incrementing counter inside THREAD
TIME @30 ns THREAD: Counter value: 5
Incrementing counter inside THREAD
TIME@ 40 ns METHOD: Counter value: 6
Incrementing counter inside METHOD
TIME @40 ns THREAD: Counter value: 7
Incrementing counter inside THREAD
Simulation ending @50 ns
```

As we can see from the output the `THREAD` is executed every `10ns`, while the method every `20ns`. The difference is that the first one is using a `wait()`, while the second one a `next_trigger()` statement. 

If we try to put a `wait()` statement inside the `METHOD`, like the following code:
```cpp
void count_method() {
    cout << "TIME@ " << sc_time_stamp() << " METHOD: Counter value: " << cnt << endl;
    cnt++;
    cout << "Incrementing counter inside METHOD" << endl;
    next_trigger(sc_time(20, SC_NS));
    wait(10, SC_NS);
}
```

 we get the following error:

```
Error: (E519) wait() is only allowed in SC_THREADs and SC_CTHREADs: 
    in SC_METHODs use next_trigger() instead
```

This is reasonable, since the `wait()` statement is not allowed inside the `SC_METHOD` function. In fact, given the timing behaviour of the `SC_METHOD`, the `wait` statement would block the execution of the rest of the simulation. 