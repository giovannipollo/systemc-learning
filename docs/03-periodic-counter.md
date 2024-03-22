This code runs a counter 10 times, since the `SC_METHOD` is executed every every 10ns. The `SC_METHOD` should be defined inside the `SC_CTOR` function. 
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

