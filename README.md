# curlinho
A minimalist wrapper around curl, with compability with recent features like http2.0 protocol
This project is based on the design of
[C++ Requests]( http://whoshuu.github.io/cpr/) (Curl for People) project.

This Project is compatible with Mac osx and Windows (MSVC).

The Pre-compiled libraries and binaries of Curl and its dependencies, necessary for the curlinho to work, are available in the dependencies folder.

## Features

Curlinho currently supports:

* Custom headers
* Url encoded parameters
* Basic authentication
* Timeout specification
* Asynchronous requests
* Callback interface
* Custom Retry Policy
* Fully Concurrent POST requests
* OpenSSL
* HTTP/2 Protocol
* Hmac authetication

## Integration

If you need to integrate Curlinho with your project, the primary way is to use git submodules. Add this repository as a submodule of your root repository:

```shell
git submodule add git@github.com:jlsilva1994/curlinho.git
OR 
git submodule add https://github.com/jlsilva1994/curlinho.git

git submodule update --init --recursive
```

Next, add this subdirectory to your CMakeLists.txt before declaring any targets that might use it:

```cmake
add_subdirectory(curlinho)
```

This will produce two important CMake variables, `CURLINHO_INCLUDE_DIRS` and `CURLINHO_LIBRARIES`, which you'll use in the typical way:

```cmake
include_directories(${CURLINHO_INCLUDE_DIRS})
target_link_libraries(your_target_name ${CURLINHO_LIBRARIES})
```
