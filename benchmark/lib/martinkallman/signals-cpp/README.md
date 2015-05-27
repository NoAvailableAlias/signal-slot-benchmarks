signals++
=========

A minimalistic implementation of slots and signals for C++11 using varadic templates.

<br>

### Usage

```c++
#include <signal.h>
#include <iostream>


class Mouse {
  Signal<int,int> on_move_;
  
public:
  inline const Signal<int,int>& on_move() {
    return on_move_;
  }
  
  void MoveIt(int x, int y) {
    on_move_(x, y);
  }
};


void PrintMouseCoords(int x, int y) {
  std::cout << "New mouse coordinates: X" << x << ", Y" << y << std::endl;
}


int main() {
  Mouse mouse;
  
  mouse.on_move().Add(PrintMouseCoords);
  mouse.MoveIt(27, 905);
  mouse.on_move().Remove(PrintMouseCoords);
  mouse.MoveIt(32, 891);

  Signal<int,int>::Disposable disposable = mouse.on_move().Add([](int x, int y) {});
  mouse.on_move().Remove(disposable);
  
  return 0;
}
```

<br>

### Compiler support

 - signal.h tested on GCC 4.7, MSC 2012 (Nov CTP) 
