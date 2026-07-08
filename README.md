# NetDisk - 基于 Qt 的网络云盘系统

基于 **Qt 5.15.2 (C++17)** 开发的客户端-服务端架构网络云盘系统，支持用户注册登录、文件上传下载、断点续传、秒传、文件搜索与分享等功能。

## 功能特性

### 用户系统
- 用户注册与登录
- 用户身份验证
- 用户文件隔离管理

### 文件管理
- **文件上传**：支持大文件分块上传，带 MD5 校验
- **秒传**：通过 MD5 检测重复文件，已存在文件无需重复上传
- **断点续传**：上传中断后可从断点位置继续传输
- **文件下载**：支持将服务器文件下载到本地
- **文件删除**：支持删除已上传的文件
- **文件搜索**：按关键词搜索用户文件

### 文件分享
- **生成分享链接**：为文件生成分享提取码
- **提取分享文件**：通过提取码获取他人分享的文件信息

## 项目结构

```
NetDisk/
├── 3-21-server/              # 服务端（命令行应用）
│   ├── Kernel/               # 业务逻辑核心
│   │   ├── kernel.cpp/h      # 处理注册、登录、文件操作等核心逻辑
│   │   └── IKernel.h         # 内核接口
│   ├── netWork/              # TCP 网络层
│   │   ├── tcpnet.cpp/h      # TCP 服务端实现
│   │   └── INet.h            # 网络接口
│   ├── CMySQL/               # MySQL 数据库层
│   │   ├── cmysql.cpp/h      # 数据库操作封装
│   │   └── CMySQL.pri
│   ├── init_database.sql     # 数据库初始化脚本
│   ├── packdef.h             # 通信协议定义
│   ├── main.cpp              # 服务端入口
│   └── 3-21-server.pro       # Qt 项目文件
│
├── 3-29client/               # 客户端（GUI 应用）
│   ├── Kernel/               # 业务逻辑核心
│   │   ├── kernel.cpp/h      # 请求处理与信号转发
│   │   └── IKernel.h
│   ├── TcpNet/               # TCP 网络客户端
│   │   ├── clientclass.cpp/h # TCP 客户端实现
│   │   └── INet.h
│   ├── MD5/                  # MD5 哈希计算
│   │   ├── md5.cpp/h         # 文件 MD5 计算（用于秒传判断）
│   │   └── MD5.pri
│   ├── login.cpp/h/ui        # 登录界面
│   ├── register.cpp/h/ui     # 注册界面
│   ├── mainwindow.cpp/h/ui   # 主界面（文件列表、操作）
│   ├── dialog.cpp/h/ui       # 提取分享文件对话框
│   ├── packdef.h             # 通信协议定义
│   ├── main.cpp              # 客户端入口
│   ├── 3-29client.pro        # Qt 项目文件
│   └── image.qrc             # 图片资源文件
│
├── .gitignore
└── README.md
```

## 通信协议

系统采用自定义二进制协议，基于 TCP 传输。协议定义在 `packdef.h` 中，核心数据结构：

```cpp
// 基础协议头
struct STRU_BASE {
    char m_nType;  // 协议类型标识
};
```

支持的消息类型：

| 类型 | 说明 |
|------|------|
| 注册请求/响应 | 用户注册 |
| 登录请求/响应 | 用户登录认证 |
| 文件列表请求/响应 | 获取用户文件列表 |
| 上传文件信息请求/响应 | 上传前发送文件元信息（含 MD5） |
| 上传文件内容请求 | 分块传输文件内容（每块 4096 字节） |
| 删除文件请求 | 删除指定文件 |
| 搜索文件请求/响应 | 按关键词搜索文件 |
| 分享链接请求/响应 | 生成分享提取码 |
| 提取链接请求/响应 | 通过提取码获取文件 |
| 下载文件请求/响应 | 下载文件内容 |

## 环境要求

- **操作系统**：Windows（当前开发环境）
- **Qt 版本**：Qt 5.15.2 (MinGW 64-bit)
- **编译器**：MinGW 64-bit
- **数据库**：MySQL 5.7+
- **构建工具**：qmake

## 快速开始

### 1. 初始化数据库

运行 `3-21-server/init_database.sql` 创建数据库和表：

```bash
mysql -u root -p < 3-21-server/init_database.sql
```

数据库结构：

| 表名 | 说明 |
|------|------|
| `user` | 用户表（u_id, u_name, u_password, u_tel） |
| `file` | 文件表，按 MD5 去重（f_id, f_name, f_size, f_MD5, f_path, f_count） |
| `user_file` | 用户-文件映射表（u_id, f_id, time） |
| `user_shared` | 分享表（uid, fid, code） |
| `ufile` | 视图：user_file JOIN file，方便查询用户文件列表 |

### 2. 修改数据库连接

在 `3-21-server/Kernel/kernel.cpp` 的 `open()` 函数中修改连接参数：

```cpp
m_pSql->ConnectMySql("127.0.0.1", "root", "your_password", "3-22NetDisk");
```

### 3. 编译运行

**服务端：**

```bash
cd 3-21-server
qmake 3-21-server.pro
mingw32-make
```

**客户端：**

```bash
cd 3-29client
qmake 3-29client.pro
mingw32-make
```

## 技术亮点

- **自定义二进制协议**：高效的数据传输格式，减少网络开销
- **MD5 文件校验**：保证文件完整性，实现秒传功能
- **断点续传**：大文件上传失败后可从中断位置继续
- **文件引用计数**：同一文件被多用户持有时不重复存储，通过 f_count 管理
- **模块化架构**：网络层、数据库层、业务逻辑层分离，易于维护和扩展
- **信号槽通信**：客户端使用 Qt 信号槽机制实现异步网络响应处理

## 协议

MIT License
