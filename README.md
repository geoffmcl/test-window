test-window project
------------

2017-12-22 - The date is important...

For **many** years I developed `WIN32` apps, then I fell into the many `FOSS` projects, which involved cross porting, to many OS's, and seems I somehow began to **forget** `WIN32`, ie a pure windows app...

This `WIN32` app really does **nothing**, except run a **Win32** app, without `UNICODE`, and demostrates some WIN32 API's...

It exists, so that if I have an idea to add something **new** to such a `WIN32`, then it will probably be in a `branch` ...

2017-12-24 - Added my **old** `testap4` source to here. Again, it is really a **do nothing** app, just testing and exploring some pure WIN32 API's...

That is **both** apps here a really just `educational` in purpose, and provide a way to test and experiment with various WIN32 API's...

**Building**

This uses a [cmake](https://cmake.org/) build script to generae `local` build files...

```
> cd build
cmake .. [options]
cmake --build . --config Release
```

In that `build` folder, there is a `build-me.bat`, for MSVC14, which sets up the WIN64 environment, and builds the projects... that batch file may need some adjustment for your particular environment...

Coding is **FUN**... Enjoy...

Geoff.  
20171222

; eof
