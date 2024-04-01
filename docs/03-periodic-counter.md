This code runs a counter 10 times, since the `SC_METHOD` is executed every every 10ns.
It is important to remember that a method expect to have a `next_trigger` function at the end, otherwise it will be executed only once. This is clearly visible from the output of the program

```
Counter initialized
Counter value: 0
Counter no next trigger value: 0
Counter value: 1
Counter value: 2
Counter value: 3
Counter value: 4
Counter value: 5
Counter value: 6
Counter value: 7
Counter value: 8
Counter value: 9
```
The reason behind stands in the fact that the `SC_METHOD` can be seen as a programming function. This means that is executed only one when called. To schedule another execution, the `next_trigger` should be specified

It is also important to remember that the method should be initialized in the `SC_CTOR`. This is needed because SystemC must register it in the simulation kernel.

```cpp
    SC_CTOR(counter){
        ...
        SC_METHOD(count);
        SC_METHOD(count_no_next_trigger);
    }
```
