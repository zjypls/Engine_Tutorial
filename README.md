# Engine_Tutorial
Learning GameEngine with The Cherno in Youtobe  
## clone 
```
git clone --recursive https://github.com/zjypls/Engine_Tutorial.git
```
## or clone and init submodule
```
git clone https://github.com/zjypls/Engine_Tutorial.git
```
```
cd Engine_Tutorial
```
```
git submodule update --init --recursive
```
## build
need vulkanSDK  
you can download from [there](https://vulkan.lunarg.com) or install by package manager on Linux 
```
cmake -B build
```
```
cmake --build build
```
Learning:  
>[The Cherno's GameEngine Series-YouTube](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT)
>
>[Games 104 On Bilibili](https://www.bilibili.com/video/BV1oU4y1R7Km)
>
>[Games 104 On Github](https://github.com/BoomingTech/Piccolo)
>
  
## Screen Shot:
Editor:
![EditorMain](./Assets/ReadMe/Gif/Editor.gif)

notices:can build with msvc or clang(must off ReBuildScriptsCore flags in CMakeLists.txt)
