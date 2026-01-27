This to be inline with naming convention;
s_ prefix for static fields;
m_ prefix for member fields.


材质：shader + uniform
shader中getUniformLocation统一获取提高性能


Application

单例类

管理整个引擎的生命周期

引擎中的Application是基类，客户端继承实现独立的Application，引擎不知道具体的Application，实现解耦



Window

窗口纯虚类，通过接口模式实现平台无关的窗口系统

通过设置纯虚函数，要求子类实现的具体平台窗口类必须实现

static Window\* Create()在子类的cpp文件中实现，配合premake针对不同平台编译不同文件的策略，实现多平台兼容

OnUpdate()

GetWideth(),GetHeight()

SetEventCallback()

在Application的构造函数中调用，将Application中的OnEvent函数设置为Windows的具体子类的EventCallback

SetVSync()

IsVSync()

GetNativeWindow()



WindowsWindow

Window的子类，实现具体的Windows平台窗口

包含GLFWwindow*和WindowData{Title， Width， Height， VSync， EventCallbackFn}

EventCallback由Application类设置，在设置GLFW callbacks中调用



LayerStack

Layer的容器,包含了一个vector<std::unique\_ptr<Layer>>和std::vector<std::unique\_ptr<Layer>>::iterator

可以从前往后PushLayer也可以在vector末尾PushOverlay



Layer

每个Layer只包含了一个名字

引擎中只有Layer的抽象类，具体的实现在APP中

每个Layer中包含进入Stack时需要调用的OnAttach和出Stack时调用的Detach，负责处理渲染的OnUpdate和处理事件的OnEvent



Event

事件抽象类，Events文件夹中其他的类都是他的具体子类，但是只提供了默认实现，在每个Layer中实现针对每个Layer的具体事件



imGui

imGuiLayer的事件实现需要调用glfw实现，通过imGuiBuild.cpp来实现便捷的链接


/***********************************
完整事件系统：
 窗口事件发生 → GLFW触发 → 调用Data.EventCallback → 执行Application::OnEvent → 事件分发到各层(Layers)→ 各层Layers实现互相独立的OnEvent（通过EventDispatcher筛选出Layers感兴趣的事件）



Renderer

采用静态设计，所有成员都是静态的，静态成员SceneData存储场景数据。
静态设计意味着整个程序共享一份渲染器和场景数据，多个Submit共享同一份SceneData

Timestep

有一个单参数构造函数允许float类型隐式转换为Timestep， 同时有类型转换运算符允许将Timestep隐式转换为float， 可以把 Timestep 对象当作 float 直接使用



OpenGLTexture

硬编码了纹理的放大缩小方式
glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
缩小时使用双线性过滤
放大时使用最近邻过滤

Shader
可通过字符串或文件读取的方式构造
每个Shader必须有一个name方便ShaderLibrary管理
需要注意的是Create返回的是智能指针，和其他资源返回原始指针不同，因此不能通过reset转移资源

ShaderLibrary

统一管理shader
通过unordermap存储shader，可根据name索引取出shader

OrthographicCameraController

管理相机类的同时，负责相机的移动缩放，根据windows窗口宽高比调整相机宽高比。
同时提供GetCamera的接口（Submit提交渲染时需要的参数时camera）



TODO：
ImGui拖动bug（可能是ImGui版本原因，可更换到老版本尝试）或者移除循环  //已解决：未清除渲染缓冲区

Renderer类的作用是什么
了解混合，纹理缩放原理



diff：

layout(binding = 0) uniform sampler2D u_Texture;通过OpenGL4.2+语法 layout(binding = 0) 自动设置纹理槽位

shader类中读取的文件必须从两个文件（vertex, fragment）分别读取


优化：

支持上传VertexBuffer时将静态数据和动态数据分为两个VertexBuffer上传：
将静态数据与动态数据拆分到不同的 Vertex Buffer，在数据更新频率差异明显的场景下，通常可以显著降低更新成本、减少同步、提升整体渲染性能。



