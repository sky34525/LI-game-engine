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
   - **Debug**: 定义 `LI_DEBUG`,启用断言和调试符号
   - **Release**: 定义 `LI_RELEASE`,启用优化和调试符号
   - **Dist**: 定义 `LI_DIST`,完全优化

4. **输出目录**:
   - 二进制: `bin/{配置}-windows-x64/{项目名}/`
   - 中间文件: `bin-int/{配置}-windows-x64/{项目名}/`

5. 引擎核心为**静态库**,LI 直接链接到 Sandbox,无需 DLL 复制。

## 核心架构

### 静态库架构模式

- **引擎侧**: `main()` 在 `EntryPoint.h` 中实现,初始化日志、调用 `CreateApplication()` 并运行应用
- **客户端侧**: 只需实现 `CreateApplication()` 返回自定义 `Application` 子类实例
- **智能指针别名** (Core.h):
  - `Ref<T>` = `std::shared_ptr<T>`,配套 `CreateRef<T>(args...)`
  - `Scope<T>` = `std::unique_ptr<T>`,配套 `CreateScope<T>(args...)`
- 所有平台接口的 `Create()` 工厂方法统一返回 `Ref<T>`,实现使用 `CreateRef<ConcreteType>(args...)`

### Layer 系统

**Layer 基类** (`LI/Core/Layer.h`):
- `OnAttach()` / `OnDetach()`: 层加入/移出栈时调用(LayerStack 自动触发)
- `OnUpdate(Timestep ts)`: 每帧逻辑
- `OnEvent(Event& e)`: 事件处理
- `OnImGuiRender()`: ImGui 渲染回调

**LayerStack**:
- 分为常规层(前半部分)和覆盖层(后半部分,如 ImGui)
- 事件从栈顶到栈底反向传播,任何层可设 `e.Handled = true` 阻止继续传播

### 事件系统

```
窗口事件 → GLFW 回调 → 创建 Event 对象 → Application::OnEvent
→ EventDispatcher 分发到 Application 处理器
→ 反向迭代 LayerStack,每层 Layer::OnEvent
→ 层用 EventDispatcher 筛选感兴趣的事件
```

**使用模式**:
```cpp
void OnEvent(Event& e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(LI_BIND_EVENT_FN(OnWindowClose));
}
```

**添加新事件类型**: 在 `EventType` 枚举中添加 → 继承 `Event` → 使用 `EVENT_CLASS_TYPE` 和 `EVENT_CLASS_CATEGORY` 宏。

### 渲染系统架构

渲染系统分两层:

**低层 (通用)**:
- `RenderCommand`: 静态封装,`SetClearColor()` / `Clear()` / `DrawIndexed()`
- `RendererAPI` 纯虚接口 → `OpenGLRendererAPI` 实现
- `VertexBuffer` / `IndexBuffer` / `VertexArray` / `Shader` / `Texture2D`: 均为抽象接口 + OpenGL 实现
- 工厂方法根据 `Renderer::GetAPI()` 返回对应平台实例

**高层 2D (推荐)**:
- `Renderer2D`: 静态工具类,内部持有预建 quad 网格,对外提供高层 `DrawQuad` API

### Renderer2D 系统

**初始化** (在 Application 或第一个 Layer 的 `OnAttach` 前调用一次):
```cpp
LI::Renderer2D::Init();   // 创建内部 quad VAO 和 shader
LI::Renderer2D::Shutdown(); // 清理
```

**每帧渲染流程**:
```cpp
// Layer::OnUpdate()
RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
RenderCommand::Clear();

Renderer2D::BeginScene(m_CameraController.GetCamera());

// 绘制纯色 quad (支持 vec2/vec3 位置)
Renderer2D::DrawQuad({ 0.0f, 0.0f },          { 1.0f, 1.0f }, m_Color);
Renderer2D::DrawQuad({ 0.5f, -0.5f, 1.0f },   { 0.5f, 0.75f }, color);

// 绘制纹理 quad
Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f },   { 10.0f, 10.0f }, m_Texture);

Renderer2D::EndScene();
```

**内部实现**:
- 所有 quad 复用同一 VAO,每次调用只改变 uniform(`u_ViewProjection`、`u_Transform`、`u_Color`/纹理槽)
- 平色 quad 使用 `assets/shaders/QuadVs.glsl` + `QuadFs.glsl`
- 纹理 quad 使用 `assets/shaders/TextureVs.glsl` + `TextureFs.glsl`,顶点含 Float3 位置 + Float2 纹理坐标

### Texture 系统

```cpp
// 加载纹理
Ref<Texture2D> m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");

// 手动绑定(Renderer2D::DrawQuad 会自动处理)
m_Texture->Bind(0); // 绑定到纹理单元 0
```

- `OpenGLTexture2D` 使用 **stb_image** 加载 PNG/JPG,自动垂直翻转
- 支持 RGB8(3 通道)和 RGBA8(4 通道)
- 使用 DSA API: `glCreateTextures` / `glTextureStorage2D` / `glTextureSubImage2D`

### Shader 与 ShaderLibrary

```cpp
// 直接创建
Ref<Shader> shader = Shader::Create("vertex.glsl", "fragment.glsl");

// 通过 ShaderLibrary 管理(避免重复编译)
ShaderLibrary m_ShaderLib;
m_ShaderLib.Load("MyShader", "vertex.glsl", "fragment.glsl");
Ref<Shader> s = m_ShaderLib.Get("MyShader");
```

### 低层渲染资源管理

**设置顺序(必须严格遵守)**:
```cpp
// OnAttach() 中
auto vb = VertexBuffer::Create(vertices, sizeof(vertices));
vb->SetLayout({{ ShaderDataType::Float3, "a_Position" },
               { ShaderDataType::Float2, "a_TexCoord" }});
m_VA = VertexArray::Create();
m_VA->AddVertexBuffer(vb);   // 必须先 SetLayout 再 AddVertexBuffer
m_VA->SetIndexBuffer(IndexBuffer::Create(indices, count));
```

**常见陷阱**: 忘记 `AddVertexBuffer()` / `SetIndexBuffer()` 导致 `GetIndexBuffer()` 返回 nullptr,引发崩溃。

### 平台抽象

- **Window**: `Window::Create()` 工厂 → `WindowsWindow`(基于 GLFW)
- **Input**: 静态单例接口 → `WindowsInput`(基于 GLFW 查询)
- **GraphicsContext**: `Init()` / `SwapBuffers()` → `OpenGLContext`(GLAD 初始化)
- 键码在 `KeyCodes.h`,鼠标按钮码在 `MouseButtonCodes.h`

### ImGui 集成

- `ImGuiLayer` 作为 Overlay 在 Application 构造时自动添加
- 主循环调用 `Begin()/End()` 包裹所有层的 `OnImGuiRender()`
- 后端: `imgui_impl_glfw` + `imgui_impl_opengl3`(通过 `ImGuiBuild.cpp` 包含源文件编译)
- 启用了 Docking 和 Multi-Viewport

### 日志系统

- **核心**: `LI_CORE_ERROR/WARN/INFO/TRACE()`
- **客户端**: `LI_ERROR/WARN/INFO/TRACE()`
- 在 `EntryPoint.h` 的 `main()` 中自动初始化

## 第三方库

`vendor/` 目录管理,均通过 Premake 编译为静态库(仅头文件库除外):

- **GLFW**: 窗口和输入
- **GLAD**: OpenGL 函数加载器
- **ImGui**: 即时模式 GUI (Docking + Multi-Viewport)
- **spdlog**: 日志 (仅头文件)
- **glm**: 数学库 (仅头文件)
- **stb_image**: 图像加载 (单文件)

## 开发指南

### 添加新 Layer

```cpp
class MyLayer : public LI::Layer {
public:
    MyLayer() : Layer("MyLayer") {}
    void OnAttach() override { /* 初始化渲染资源、加载纹理 */ }
    void OnDetach() override { /* 清理 */ }
    void OnUpdate(LI::Timestep ts) override { /* 每帧逻辑 + 渲染 */ }
    void OnEvent(LI::Event& e) override { /* 事件处理 */ }
    void OnImGuiRender() override { /* ImGui UI */ }
};

// SandboxApp.cpp 中:
PushLayer(std::make_unique<MyLayer>());
```

### 添加新平台实现

1. 在 `LI/Renderer/` 中定义纯虚接口
2. 在 `Platform/{API}/` 中实现具体类
3. 在基类 `Create()` 中用 `CreateRef<ConcreteType>(args...)` 返回实例:
   ```cpp
   Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
       switch (Renderer::GetAPI()) {
       case RendererAPI::API::OpenGL:
           return CreateRef<OpenGLVertexBuffer>(vertices, size);
       }
       return nullptr;
   }
   ```

### 断言系统

Debug 配置下启用 (`Core.h`):
- `LI_CORE_ASSERT(condition, message)`: 引擎核心
- `LI_ASSERT(condition, message)`: 客户端
- 失败时记录错误并触发 `__debugbreak()`

### 编译选项

- C++17,`/utf-8`,Windows SDK latest
- 预编译头: `pch.h` / `pch.cpp`
- `LI_API` 宏标记引擎 API(当前静态库下为空宏)
- `BIT(x)` 宏用于位掩码操作
