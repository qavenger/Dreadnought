# Dreadnought

## Build
依次执行以下步骤：

### 1.拉取引擎代码
```cmd
> git clone https://github.com/qavenger/Dreadnought.git
```
### 2.构建环境：

#### 2.1下载VS2019，并需要安装英语语言包
#### 2.2拉取三方库
```cmd
> git clone https://github.com/microsoft/vcpkg
> .\vcpkg\bootstrap-vcpkg.bat
> .\vcpkg\vcpkg install imgui[dx12-binding]:x64-windows
> .\vcpkg\vcpkg install imgui[win32-binding]:x64-windows
> .\vcpkg\vcpkg install assimp:x64-windows
> .\vcpkg\vcpkg integrate install
```

一切顺利的话就能运行了
