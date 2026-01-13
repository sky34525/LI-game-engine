# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

LI 是一个 C++ 游戏引擎项目,采用静态库架构设计。引擎核心编译为静态库,通过 Sandbox 测试应用程序进行开发和测试。引擎使用平台抽象层设计,当前支持 Windows 平台和 OpenGL 渲染 API。

## 构建系统

### 生成和构建项目

1. **生成 Visual Studio 项目**:
   ```bash
   GenerateProjects.bat
   ```
   或手动执行:
   ```bash
   vendor\bin\premake5\premake5.exe vs2022
   ```

2. **在 Visual Studio 中构建**:
   - 打开生成的 `LI.sln`
   - 选择配置: Debug/Release/Dist
   - 构建解决方案 (Ctrl+Shift+B)
   - 启动项目默认为 Sandbox

3. **构建配置**:
   - **Debug**: 调试版本,定义 `LI_DEBUG`,启用断言和调试符号
   - **Release**: 发布版本,定义 `LI_RELEASE`,启用优化和调试符号
   - **Dist**: 分发版本,定义 `LI_DIST`,完全优化

4. **输出目录结构**:
   - 二进制: `bin/{配置}-windows-x64/{项目名}/`
   - 中间文件: `bin-int/{配置}-windows-x64/{项目名}/`

5. **架构变更** (提交 462e8e4):
   - 引擎核心已从 DLL 改为**静态库** (`kind "StaticLib"`)
   - 不再需要 DLL 复制步骤,LI 直接链接到 Sandbox

## 核心架构

### 静态库架构模式

引擎使用"入口点隐藏"模式实现引擎和客户端代码的解耦:

- **引擎侧**: `main()` 函数在 `EntryPoint.h` 中实现,负责初始化日志系统、调用 `CreateApplication()` 并运行应用
- **客户端侧**: 只需实现 `CreateApplication()` 工厂函数返回自定义 `Application` 子类实例
- **符号标记**: `LI_API` 宏仍用于标记引擎 API,但当前构建为静态库而非 DLL

### Layer 系统

Layer 系统是引擎的核心组织方式,提供模块化的逻辑和渲染管理:

**Layer 基类** (LI/Layer.h):
- `OnAttach()`: 层添加到栈时调用,用于初始化
- `OnDetach()`: 层移除时调用,用于清理
- `OnUpdate()`: 每帧更新逻辑
- `OnEvent(Event& e)`: 处理事件
- `OnImGuiRender()`: ImGui 渲染回调

**LayerStack 容器** (LI/LayerStack.h):
- 使用 `std::vector<std::unique_ptr<Layer>>` 管理生命周期
- 分为两个区域: 常规层(前半部分) 和 覆盖层(后半部分,如 ImGui)
- `PushLayer()`: 添加常规层到前半部分
- `PushOverlay()`: 添加覆盖层到末尾
- `PopLayer()/PopOverlay()`: 移除层并调用 `OnDetach()`
- **重要**: OnAttach/OnDetach 在 LayerStack 中自动调用 (最新提交 fb710a1)

**事件传播顺序**:
事件从栈顶到栈底反向传播,覆盖层优先处理。任何层都可以设置 `e.Handled = true` 阻止事件继续传播。

### 事件系统

完整的事件流:
```
窗口事件发生 → GLFW 回调触发 → Lambda 回调创建 Event 对象
→ 调用 Data.EventCallback → Application::OnEvent
→ EventDispatcher 分发到 Application 处理器
→ 反向迭代 LayerStack,每层调用 Layer::OnEvent
→ 各层使用 EventDispatcher 筛选感兴趣的事件
→ 层标记 e.Handled = true 阻止继续传播
```

**EventDispatcher 使用模式** (LI/Events/Event.h):
```cpp
void OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(LI_BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<KeyPressedEvent>(LI_BIND_EVENT_FN(OnKeyPressed));
}
```

**事件类型**:
- **窗口事件**: WindowResize, WindowClose
- **键盘事件**: KeyPressed, KeyReleased, KeyTyped
- **鼠标事件**: MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased

**添加新事件类型**:
1. 在 `EventType` 枚举中添加类型
2. 创建事件类,继承 `Event`
3. 使用 `EVENT_CLASS_TYPE` 和 `EVENT_CLASS_CATEGORY` 宏

### 平台抽象

**Window 系统** (LI/Window.h):
- 抽象接口定义平台无关的窗口操作
- `Window::Create()` 静态工厂方法在平台特定 cpp 中实现
- **WindowsWindow** 实现基于 GLFW,在 `Platform/Windows/WindowsWindow.cpp` 中
- 所有 GLFW 回调在 `Init()` 中设置,映射到对应的 Event 对象

**Input 系统** (LI/Input.h):
- 单例模式 + 静态接口
- `IsKeyPressed(keycode)`, `IsMouseButtonPressed(button)`, `GetMousePosition()` 等
- **WindowsInput** 实现使用 GLFW 的输入查询函数
- 键码定义在 `KeyCodes.h`,鼠标按钮码在 `MouseButtonCodes.h`

### ImGui 集成

**ImGuiLayer** (LI/imGui/imGuiLayer.h):
- 作为 Overlay 在 Application 构造时自动添加
- 在主循环中调用 `Begin()/End()` 包裹所有层的 `OnImGuiRender()`
- 启用了 Docking 和 Multi-Viewport 功能

**后端配置**:
- 平台后端: `imgui_impl_glfw` (GLFW3)
- 渲染后端: `imgui_impl_opengl3` (OpenGL 4.1)
- 后端实现通过 `ImGuiBuild.cpp` 直接包含源文件编译

**最近修复** (提交 ec3c8db):
修复了 ImGui 拖动残影问题,在 `Begin()` 中添加了渲染缓冲区清除。

### 渲染系统

**GraphicsContext 抽象** (LI/Renderer/GraphicsContext.h):
- 平台无关的图形上下文接口,定义 `Init()` 和 `SwapBuffers()`
- **OpenGLContext** 实现在 `Platform/OpenGL/OpenGLContext.cpp` 中
- 从 WindowsWindow 中分离,初始化 GLAD 并管理 OpenGL 上下文

**Renderer API 抽象** (LI/Renderer/Renderer.h):
- 定义 `RendererAPI` 枚举(None, OpenGL)
- 提供 `GetAPI()` 静态方法查询当前渲染 API

**Buffer 抽象** (LI/Renderer/Buffer.h):
- **VertexBuffer/IndexBuffer**: 平台无关的缓冲区接口
- **BufferLayout**: 描述顶点属性布局(位置、颜色等)
  - `BufferElement` 定义单个属性(类型、名称、偏移量、归一化)
  - `ShaderDataType` 枚举支持 Float/Float2/Float3/Float4/Mat3/Mat4/Int/Bool
  - 自动计算 Stride 和 Offset
- **OpenGLBuffer**: OpenGL 实现在 `Platform/OpenGL/OpenGLBuffer.cpp`
- 使用静态工厂方法 `Create()` 根据当前 API 创建具体实例

**Shader 系统** (LI/Renderer/Shader.h):
- 着色器编译、链接和使用的抽象
- `Bind()/Unbind()` 管理着色器程序状态

**架构设计原则**:
- 平台抽象层通过纯虚接口定义 API (Renderer/ 目录)
- 平台实现在 `Platform/{API}/` 目录 (如 Platform/OpenGL/)
- 使用静态工厂模式根据 `Renderer::GetAPI()` 创建具体实例

### 日志系统

基于 spdlog 库,分离核心日志和客户端日志:

- **核心日志**: `LI_CORE_ERROR()`, `LI_CORE_WARN()`, `LI_CORE_INFO()`, `LI_CORE_TRACE()`
- **客户端日志**: `LI_ERROR()`, `LI_WARN()`, `LI_INFO()`, `LI_TRACE()`
- 日志在 `EntryPoint.h` 的 `main()` 中自动初始化

## 第三方库

项目依赖管理通过 `vendor/` 目录 + git submodule:

- **GLFW**: 跨平台窗口和输入库
- **GLAD**: OpenGL 函数加载器
- **ImGui**: 即时模式 GUI 库 (支持 Docking 和 Multi-Viewport)
- **spdlog**: 高性能日志库 (仅头文件)
- **glm**: OpenGL 数学库 (仅头文件)

所有第三方库通过 Premake 编译为静态库(除了仅头文件库)。

## 开发指南

### 添加新 Layer

1. 创建类继承 `LI::Layer`:
   ```cpp
   class MyLayer : public LI::Layer {
   public:
       MyLayer() : Layer("MyLayer") {}

       void OnAttach() override { /* 初始化 */ }
       void OnDetach() override { /* 清理 */ }
       void OnUpdate() override { /* 每帧逻辑 */ }
       void OnEvent(LI::Event& e) override { /* 事件处理 */ }
       void OnImGuiRender() override { /* ImGui UI */ }
   };
   ```

2. 在 Application 子类中添加:
   ```cpp
   PushLayer(std::make_unique<MyLayer>());
   ```

### 断言系统

Debug 配置下启用断言 (Core.h:13-23):
- `LI_CORE_ASSERT(condition, message)`: 引擎核心断言
- `LI_ASSERT(condition, message)`: 客户端断言
- 断言失败会记录错误并触发 `__debugbreak()`

### 编译选项

- C++17 标准
- `/utf-8` 编译选项 (spdlog 需要)
- Windows SDK 版本: latest
- 预编译头: `pch.h`/`pch.cpp` (包含常用标准库和引擎头)

### 代码规范

- 引擎核心代码使用 `LI_API` 宏标记导出的类和函数
- 使用智能指针管理生命周期 (`std::unique_ptr` 用于 Layer、Buffer、Shader 等)
- 事件处理使用 `LI_BIND_EVENT_FN` 宏绑定成员函数
- 位掩码操作使用 `BIT(x)` 宏
- 渲染资源使用裸指针返回(由调用者管理生命周期),未来可能改为智能指针

### 添加新的平台实现

1. 在 `LI/Renderer/` 中定义平台无关接口(纯虚类)
2. 在 `Platform/{API}/` 中实现具体类(如 `OpenGLBuffer`)
3. 在基类的 `Create()` 静态工厂方法中根据 `Renderer::GetAPI()` 返回实例:
   ```cpp
   VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
   {
       switch (Renderer::GetAPI())
       {
           case RendererAPI::OpenGL:
               return new OpenGLVertexBuffer(vertices, size);
       }
       return nullptr;
   }
   ```
