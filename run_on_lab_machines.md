
# Run on the lab machines

```
$ rm CMakeCache.txt
$ rm -r CMakeFiles
$ rm build
$ cmake .
$ vim CMakeCache.txt
$ # for speed CMAKE_BUILD_TYPE:STRING=Release
$ # for multithreading CMAKE_CXX_FLAGS:STRING=-pthread 
$ # wq 
$ make clean
$ make 
```