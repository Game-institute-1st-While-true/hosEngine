# hosEngine
![logo](https://user-images.githubusercontent.com/45713360/124558885-08cb9900-de76-11eb-92ae-b538626f056e.png)  
Copyright (c) 2021 Game-institute-1st-While-true  

## Introduction
hosEngine은 유니티와 같은 컴포넌트 기반 게임엔진을 만들어보자는 목적으로 만들어졌습니다.  
C++ 17을 기반으로 FBX 로드, 물리 충돌과 트리거, 직렬화/역직렬화, 길 찾기, 소켓 통신을 사용한 네트워킹 등을 지원합니다.  
게임인재원 1기 While(true)팀의 졸업 프로젝트 입니다.

hosEngine was created with the purpose of creating a component-based game engine like Unity.  
Based on C++17, it supports FBX loading, physics collisions and triggers, serialization/deserialization, pathfinding, networking using socket communication, and more.  
This is the graduation project of the 1st While (true) Team of Game institute.

## How to Build
### requires
>Visual Studio 2019  
>Windows SDK 10.0  
>Platform Toolset v142  
>MFC Toolset v142 

#### 1. clone this repository onto your local drive

#### 2. FBX SDK install
> https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2  
> download FBX SDK 2020.2 vs2019 and install  
> copy "FBX SDK Directory/2020.2/include" to "hosEngine/Extern/Inc"  
> copy "FBX SDK Directory/2020.2/lib/vs2019/x64/debug" to "hosEngine/Extern/Lib"  
> > change name libfbxsdk-md.lib -> libfbxsdk_x64D.lib  
> > change name libxml2-md.lib -> libxml2_x64D.lib  
> > change name zlib-md.lib -> zlib_x64D.lib  
>
> copy "FBX SDK Directory/2020.2/lib/vs2019/x64/release" to "hosEngine/Extern/Lib"  
> > change name libfbxsdk-md.lib -> libfbxsdk_x64R.lib  
> > change name libxml2-md.lib -> libxml2_x64R.lib  
> > change name zlib-md.lib -> zlib_x64R.lib  

#### 3. open Build/hosEngine.sln

#### 4. Build hosEditor and hosClient

## Documentation
