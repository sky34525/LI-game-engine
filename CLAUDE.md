# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

LI 是一个 C++ 游戏引擎项目,采用 DLL 架构设计。引擎核心编译为动态链接库(SharedLib),通过 Sandbox 测试应用程序进行开发和测试。

## 构建系统

项目使用 Premake5 作为构建系统生成器:

1. **生成 Visual Studio 项目**:
   ```
   GenerateProjects.bat
   ```
   或手动执行:
   ```
   vendor\bin\premake5\premake5.exe vs2022
   ```

2. **构建配置**:
   - Debug: 调试版本,定义 LI_DEBUG
   - Release: 发布版本,定义 LI_RELEASE
   - Dist: 分发版本,定义 LI_DIST

3. **输出目录结构**:
   - 二进制: `bin/{配置}-windows-x64/{项目名}/`
   - 中间文件: `bin-int/{配置}-windows-x64/{项目名}/`

## 架构设计

### 项目结构

- **LI/**: 引擎核心(SharedLib DLL)
  - 使用预编译头 `pch.h`/`pch.cpp`
  - 导出符号通过 `LI_API` 宏(Core.h:4-8)
  - 包含 spdlog 第三方库用于日志记录

- **Sandbox/**: 测试应用程序(ConsoleApp)
  - 链接到 LI.dll
  - 实现 `CreateApplication()` 工厂函数创建应用实例

### 核心组件

1. **Application 系统** (LI/Application.h:8-15)
   - 抽象基类,提供 `Run()` 方法
   - 客户端通过 `CreateApplication()` 工厂函数实例化
   - 入口点在 `EntryPoint.h` 中定义

2. **事件系统** (LI/Events/Event.h)
   - 基于枚举的事件类型系统(EventType 和 EventCategory)
   - 使用 `EventDispatcher` 类进行类型安全的事件分发
   - 支持位掩码标记事件类别(使用 BIT 宏)
   - 事件类使用宏 `EVENT_CLASS_TYPE` 和 `EVENT_CLASS_CATEGORY` 简化定义

3. **日志系统** (LI/log.h)
   - 基于 spdlog 库
   - 分离核心日志(LI_CORE_*)和客户端日志(LI_*)
   - 提供 ERROR/WARN/INFO/TRACE 四个级别

### 平台抽象

- 当前仅支持 Windows 平台(Core.h:10)
- 使用 `LI_PLATFORM_WINDOWS` 宏进行平台检测
- DLL 导出/导入通过 `__declspec(dllexport/dllimport)` 实现

## 开发注意事项

### 编译选项

- C++17 标准
- `/utf-8` 编译选项(因 spdlog 需要)
- Windows SDK 版本: 10.0.26100.0

### 添加新功能

- 客户端代码应继承 `LI::Application` 类
- 新的事件类型需要在 `EventType` 枚举中添加
- 使用 `LI_API` 宏标记需要导出的类和函数
- 引擎核心修改后会自动复制 DLL 到 Sandbox 输出目录

### 入口点模式

引擎使用"入口点隐藏"模式:
- main() 函数在引擎内部(EntryPoint.h:7-15)
- 客户端只需实现 `CreateApplication()` 返回应用实例
- 日志系统在 main 中自动初始化
