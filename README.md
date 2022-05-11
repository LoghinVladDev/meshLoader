## Mesh Loading Library

Project originated in [LoghinVladDev/c-eng](https://github.com/LoghinVladDev/c-eng) as an integrated engine cpp mesh loader.
It was split, to be added as a submodule, and written in C, to reduce multithreading/object overhead

API inspired from vulkan API

### Project Integration

Adding the library via clone
```shell
mkdir project/library/folder
cd project/library/folder
git clone https://github.com/LoghinVladDev/meshLoader
```

Adding the library via submodules
```shell
git submodule add https://github.com/LoghinVladDev/meshLoader project/library/folder
```

Integration via CMake
```cmake
add_subdirectory(project/library/folder)
include_directories(project/library/folder/include)
link_libraries(meshloader)
```

### Install TBA

### Usage
For usage guidelines, the [Specification](https://github.com/LoghinVladDev/meshLoader/wiki) is available as a GitHub wiki page
