# jspoc

A POC to test out a simple Hello World script, primarily focusing on SpiderMonkey ESR 115.

## Prepping SpiderMonkey

The repo includes existing builds of SpiderMonkey (`spidermonkey/include` and `spidermonkey/lib`) against ESR 115.

If you do wish to build it yourself, then follow these steps:

* Do the prep work for [Building Firefox](https://firefox-source-docs.mozilla.org/setup/windows_build.html#building-firefox-on-windows)
  * Don't worry about whether you use HG or Git
  * When bootstrapping, indicate you're looking to do a Spidermonkey build
  * Stop when you get to the build stage
* Follow the instructions for [Building SpiderMonkey](https://firefox-source-docs.mozilla.org/js/build.html)
  * Switch to the esr115 branch via `hg up -C esr115`
  * Look in `spidermonkey/configs` for debug and release configs that were/are used
  * When you're doing a build, use `export MOZBUILD=/wherever/my/config/is`

You should be good to go at this point.

## Building with Visual Studio

To successfully build without altering any of the headers or anything, you will need
to configure your Visual Studio to use the Platform `clang-cl` instead of `v14x`.

This requires the clang-cl Visual Studio installation option to be installed.

Open the solution and build as is - VS 2022 Community was used for setup.

### Issues using v14x toolsets

#### Compile issues

```c++
enum JSProtoKey {
#define PROTOKEY_AND_INITIALIZER(name, clasp) JSProto_##name,
  JS_FOR_EACH_PROTOTYPE(PROTOKEY_AND_INITIALIZER)
#undef PROTOKEY_AND_INITIALIZER
      JSProto_LIMIT
};
```

The `JSProto_LIMIT` requires a `,` before it.

```c++
  ~LinkedList() {
#  ifdef DEBUG
    if (!isEmpty()) {
      MOZ_CRASH_UNSAFE_PRINTF(
          "%s has a buggy user: "
          "it should have removed all this list's elements before "
          "the list's destruction",
          __PRETTY_FUNCTION__);
    }
#  endif
  }
```

The `__PRETTY_FUNCTION__` needs to be replaced (e.g. `__FUNCSIG__`)
