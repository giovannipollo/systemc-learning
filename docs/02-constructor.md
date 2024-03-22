The main structure of systemC is the `SC_MODULE`. The syntax is the following:

```cpp
SC_MODULE(module_name){
    SC_CTOR(module_name){
        cout << "Hi, I'm the constructor" << endl;
    }
}
```

The `SC_CTOR` function is the constructor that is executed when the object is instanced. 