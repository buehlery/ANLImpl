# ANL-Impl

## System Requirements

* A Linux system (tested with Arch Linux)
* `cpplint` (available from `pip3 install cpplint`)
* `g++` (C++11, tested with g++ 4.9.2)
* `gtest` -- [link](https://github.com/google/googletest)

(Ensure that the command `cpplint` is available in the $PATH)

## Building

*Release version:*

```
~$ make
```

*Debug version:*

```
~$ make debug
```

*Building without cpplint and gtest (**not officially supported**)*

```
~$ make libanlsim.so
```
