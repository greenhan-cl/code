# 视频点播系统 - 模块详细功能文档

## 📋 目录

- [1. 项目概述](#1-项目概述)
- [2. 系统架构](#2-系统架构)
- [3. 微服务模块](#3-微服务模块)
  - [3.1 网关服务 (svc_gateway)](#31-网关服务-svc_gateway)
  - [3.2 用户服务 (svc_user)](#32-用户服务-svc_user)
  - [3.3 文件服务 (svc_file)](#33-文件服务-svc_file)
  - [3.4 视频服务 (svc_video)](#34-视频服务-svc_video)
  - [3.5 转码服务 (svc_transcode)](#35-转码服务-svc_transcode)
- [4. 数据层模块 (data/)](#4-数据层模块-data)
- [5. 公共模块 (common/)](#5-公共模块-common)
- [6. 协议定义 (proto/)](#6-协议定义-proto)
- [7. 技术栈](#7-技术栈)
- [8. 部署架构](#8-部署架构)

---

## 1. 项目概述

**项目名称**: C++ 微服务视频点播系统  
**技术架构**: 微服务架构 + RPC通信  
**主要功能**: 视频上传、转码、播放、搜索、用户管理、弹幕互动等

### 核心特性

- ✅ **微服务架构**: 服务间通过 RPC 通信，职责清晰，易于扩展
- ✅ **高性能缓存**: Redis 缓存热点数据，提升响应速度
- ✅ **分布式存储**: FastDFS 存储大文件（视频、图片）
- ✅ **视频转码**: FFmpeg 转码为 HLS 格式，支持流式播放
- ✅ **全文搜索**: Elasticsearch 提供中文分词和相关性搜索
- ✅ **异步任务**: RabbitMQ 处理异步任务（转码、文件删除、缓存同步）
- ✅ **服务发现**: etcd 实现服务注册与发现
- ✅ **统一网关**: HTTP 网关统一对外暴露接口

---

## 2. 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                        客户端层                               │
│              (Qt 客户端 / Web 前端)                           │
└──────────────────────┬──────────────────────────────────────┘
                       │ HTTP/RESTful API
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                      网关服务 (svc_gateway)                   │
│  - 统一 HTTP 入口                                            │
│  - 权限验证                                                  │
│  - 请求路由与转发                                            │
│  - HTTP ↔ RPC 转换                                          │
└───┬──────────────┬──────────────┬───────────────────────────┘
    │ RPC          │ RPC          │ RPC
    ▼              ▼              ▼
┌─────────┐  ┌─────────┐  ┌─────────────┐
│用户服务 │  │文件服务 │  │  视频服务   │
│svc_user │  │svc_file │  │  svc_video  │
└────┬────┘  └────┬────┘  └──────┬──────┘
     │            │               │ MQ
     │            │               ▼
     │            │      ┌─────────────────┐
     │            │      │   转码服务      │
     │            │      │ svc_transcode   │
     │            │      └─────────────────┘
     │            │
     ▼            ▼              ▼
┌─────────────────────────────────────────────┐
│              基础设施层                       │
│  MySQL  │  Redis  │  FastDFS  │  ES  │ MQ  │
└─────────────────────────────────────────────┘
```

### 通信流程

1. **客户端 → 网关**: HTTP 请求（JSON 格式）
2. **网关 → 子服务**: RPC 调用（Protobuf 二进制）
3. **子服务间**: 通过 etcd 服务发现，RPC 通信
4. **异步任务**: 通过 RabbitMQ 消息队列处理

---

## 3. 微服务模块

### 3.1 网关服务 (svc_gateway)

**职责**: 统一对外接口，权限验证，请求转发

#### 核心功能

| 功能模块 | 说明 | 对应接口 |
|---------|------|---------|
| **权限验证** | 根据会话ID和URI验证用户权限 | `judgePermission()` |
| **用户相关接口转发** | 转发用户登录、注册、信息管理等请求 | `/HttpService/tempLogin`, `/HttpService/sessionLogin`, `/HttpService/getCode`, `/HttpService/vcodeLogin`, `/HttpService/passwdLogin`, `/HttpService/logout`, `/HttpService/setAvatar`, `/HttpService/setNickname`, `/HttpService/setPassword`, `/HttpService/setStatus`, `/HttpService/getUserInfo`, `/HttpService/newAttention`, `/HttpService/delAttention`, `/HttpService/newAdministrator`, `/HttpService/delAdministrator`, `/HttpService/setAdministrator`, `/HttpService/getAdminByEmail`, `/HttpService/getAdminListByStatus` |
| **文件相关接口转发** | 转发文件上传下载请求 | `/HttpService/uploadPhoto`, `/HttpService/downloadPhoto`, `/HttpService/uploadVideo`, `/HttpService/downloadVideo` |
| **视频相关接口转发** | 转发视频管理、播放、搜索等请求 | `/HttpService/newVideo`, `/HttpService/removeVideo`, `/HttpService/judgeLike`, `/HttpService/setLike`, `/HttpService/setPlay`, `/HttpService/newBarrage`, `/HttpService/getBarrage`, `/HttpService/allVideoList`, `/HttpService/typeVideoList`, `/HttpService/tagVideoList`, `/HttpService/keyVideoList` |

#### 技术实现

- **HTTP Server**: brpc 支持 HTTP 协议
- **RPC Client**: 通过 `SvcChannels` 获取子服务 Channel
- **服务发现**: 通过 `SvcWatcher` 监听 etcd 中的服务变化
- **协议转换**: 自动将 HTTP JSON 转换为 Protobuf 消息

#### 关键组件

- `HttpServiceImpl`: 实现 `HttpService` RPC 接口
- `SvcData`: 权限验证逻辑
- `SvcServer`: 管理 brpc Server 和 etcd 注册

---

### 3.2 用户服务 (svc_user)

**职责**: 用户认证、用户管理、关注关系、管理员管理

#### 核心功能

##### 3.2.1 用户认证

| 方法 | 功能说明 | 实现逻辑 |
|-----|---------|---------|
| `tempLogin()` | **临时用户登录** | 1. 生成会话ID<br>2. 创建临时会话记录到MySQL<br>3. 返回会话ID |
| `sessionLogin()` | **会话用户登录** | 1. 验证会话ID是否存在<br>2. 检查会话是否有效<br>3. 返回是否为临时会话 |
| `getVCode()` | **获取验证码** | 1. 生成6位数字验证码<br>2. 生成验证码ID<br>3. 将验证码存入Redis（5分钟过期）<br>4. 返回验证码ID和验证码内容 |
| `regOrLogin()` | **验证码注册/登录** | 1. 验证验证码是否正确<br>2. 检查邮箱是否已注册<br>3. 未注册：创建用户、创建身份信息、更新会话<br>4. 已注册：更新会话信息 |
| `normalLogin()` | **用户名密码登录** | 1. 通过用户名查询用户<br>2. 验证密码（MD5）<br>3. 更新会话信息 |
| `logout()` | **退出登录** | 1. 获取会话信息<br>2. 更新会话为临时会话<br>3. 返回新的临时会话ID |

##### 3.2.2 用户信息管理

| 方法 | 功能说明 | 权限要求 |
|-----|---------|---------|
| `setNickname()` | **修改昵称** | 登录用户（修改自己的昵称） |
| `setPassword()` | **修改密码** | 登录用户（修改自己的密码） |
| `setAvatar()` | **修改头像** | 登录用户（修改自己的头像）<br>返回旧头像文件ID用于删除 |
| `getUser()` | **获取用户详细信息** | 游客可查看<br>登录用户可查看自己和他人信息<br>返回：基础信息、身份角色、统计数据、是否关注 |

##### 3.2.3 用户状态管理

| 方法 | 功能说明 | 权限要求 |
|-----|---------|---------|
| `setStatus()` | **修改用户状态** | 管理员（启用/禁用用户）<br>普通管理员不能修改超级管理员 |

##### 3.2.4 关注关系管理

| 方法 | 功能说明 | 业务逻辑 |
|-----|---------|---------|
| `newFollow()` | **新增关注** | 1. 验证会话有效性<br>2. 检查是否已关注<br>3. 检查不能关注自己<br>4. 创建关注关系<br>5. 更新用户统计数据缓存 |
| `delFollow()` | **取消关注** | 1. 验证会话有效性<br>2. 删除关注关系<br>3. 更新用户统计数据缓存 |

##### 3.2.5 管理员管理

| 方法 | 功能说明 | 权限要求 | 业务逻辑 |
|-----|---------|---------|---------|
| `newAdmin()` | **新增管理员** | 超级管理员 | 1. 验证权限<br>2. 检查邮箱是否已存在<br>3. 创建用户（如果不存在）<br>4. 添加管理员身份角色 |
| `setAdmin()` | **编辑管理员** | 管理员 | 1. 验证权限等级（不能修改比自己等级高的）<br>2. 不能修改自己的状态<br>3. 更新管理员信息 |
| `delAdmin()` | **删除管理员** | 超级管理员 | 1. 验证权限等级<br>2. 删除管理员身份角色<br>3. 用户仍保留，只是失去管理员身份 |
| `getAdminList()` | **获取管理员列表** | 管理员 | 根据状态筛选，分页返回 |
| `getAdmin()` | **通过邮箱获取管理员** | 管理员 | 根据邮箱查询管理员信息 |

#### 数据存储

- **MySQL**: 用户表、会话表、关注关系表、身份角色表
- **Redis**: 
  - 会话信息缓存（加速会话验证）
  - 验证码缓存（5分钟过期）
  - 用户统计数据缓存（点赞数、播放数、关注数等）

#### 缓存同步机制

- 使用 `CacheSync` 实现缓存与数据库的异步同步
- 采用"双删"策略：立即删除缓存 → 延迟3秒再删除一次（防止并发问题）

---

### 3.3 文件服务 (svc_file)

**职责**: 文件上传、下载、元数据管理

#### 核心功能

##### 3.3.1 文件上传

| 接口 | 功能说明 | 处理流程 |
|-----|---------|---------|
| `uploadPhoto()` | **上传图片** | 1. 接收图片二进制数据<br>2. 上传到 FastDFS<br>3. 创建文件元数据记录到MySQL<br>4. 返回文件ID |
| `uploadVideo()` | **上传视频** | 1. 接收视频二进制数据<br>2. 上传到 FastDFS<br>3. 创建文件元数据记录到MySQL<br>4. 触发转码任务（通过MQ）<br>5. 返回文件ID |

##### 3.3.2 文件下载

| 接口 | 功能说明 | 处理流程 |
|-----|---------|---------|
| `downloadPhoto()` | **下载图片** | 1. 根据文件ID查询元数据<br>2. 从 FastDFS 下载文件<br>3. 返回文件二进制流 |
| `downloadVideo()` | **下载视频** | 1. 根据文件ID查询元数据<br>2. 从 FastDFS 下载文件（支持断点续传）<br>3. 如果是 HLS，返回 m3u8 播放列表<br>4. 返回视频文件或分片 |

##### 3.3.3 文件元数据管理

| 方法 | 功能说明 |
|-----|---------|
| `newFileMeta()` | **创建文件元数据** | 将文件信息（ID、路径、大小、类型等）存入MySQL |
| `getFileMeta()` | **获取文件元数据** | 根据文件ID查询文件信息 |
| `removeFileMeta()` | **删除文件元数据** | 从MySQL删除文件记录 |

##### 3.3.4 异步文件删除

- 通过 RabbitMQ 异步删除文件
- 消息队列：`delete_file_queue`
- 删除流程：
  1. 从 FastDFS 删除文件
  2. 从 MySQL 删除元数据
  3. 支持延迟删除（3秒延迟，防止误删）

#### 数据存储

- **FastDFS**: 实际文件存储（图片、视频文件）
- **MySQL**: 文件元数据（文件ID、FastDFS路径、大小、类型、创建时间等）
- **Redis**: 文件元数据缓存（加速查询）

#### 文件类型支持

- **图片**: JPG, PNG 等（头像、视频封面）
- **视频**: MP4 等（原始视频文件）
- **HLS**: m3u8 播放列表 + ts 分片（转码后的流式视频）

---

### 3.4 视频服务 (svc_video)

**职责**: 视频管理、视频列表、点赞、弹幕、搜索

#### 核心功能

##### 3.4.1 视频生命周期管理

| 方法 | 功能说明 | 状态流转 | 权限要求 |
|-----|---------|---------|---------|
| `newVideo()` | **创建视频** | → `AUDITING` (审核中) | 登录用户 |
| `checkVideo()` | **审核视频** | `AUDITING` → `ONLINE`/`REJECT` | 管理员 |
| `onlineVideo()` | **上架视频** | `OFFLINE` → `ONLINE` | 管理员 |
| `offlineVideo()` | **下架视频** | `ONLINE` → `OFFLINE` | 管理员 |
| `delVideo()` | **删除视频** | 任何状态 → 删除 | 视频作者或管理员 |

**状态说明**:
- `AUDITING` (1): 审核中
- `ONLINE` (2): 审核通过，已上架
- `REJECT` (3): 审核驳回
- `OFFLINE` (4): 已下架
- `TRANSCODING` (5): 转码中
- `TRANSCODE_FAILED` (6): 转码失败

##### 3.4.2 视频创建流程

```cpp
void newVideo(ssid, videoInfo) {
    1. 验证会话有效性
    2. 生成视频ID (UUID 16位)
    3. 创建视频记录到MySQL (状态=AUDITING)
    4. 关联分类和标签
    5. 将视频ID加入用户视频列表缓存
    6. 将视频信息同步到Elasticsearch (状态=false，不可搜索)
    7. 触发转码任务（通过MQ）
    8. 更新视频状态为TRANSCODING
}
```

##### 3.4.3 视频审核流程

```cpp
void checkVideo(ssid, vid, result) {
    1. 验证管理员权限
    2. 查询视频信息
    3. 更新视频状态:
       - result=true → ONLINE (审核通过)
       - result=false → REJECT (审核驳回)
    4. 更新Elasticsearch:
       - ONLINE → 可搜索 (status=true)
       - REJECT → 不可搜索 (status=false)
    5. 同步缓存
}
```

##### 3.4.4 视频播放与互动

| 方法 | 功能说明 | 业务逻辑 |
|-----|---------|---------|
| `playVideo()` | **播放视频** | 1. 记录播放次数<br>2. 更新视频播放统计缓存<br>3. 更新用户播放统计缓存 |
| `judgeLike()` | **判断是否点赞** | 检查用户是否已点赞该视频 |
| `likeVideo()` | **点赞/取消点赞** | 1. 切换点赞状态<br>2. 更新视频点赞统计<br>3. 更新用户点赞统计 |

##### 3.4.5 弹幕功能

| 方法 | 功能说明 | 数据结构 |
|-----|---------|---------|
| `newBarrage()` | **发送弹幕** | 1. 创建弹幕记录到MySQL<br>2. 弹幕信息：用户ID、视频ID、内容、时间戳、位置 |
| `getBarrage()` | **获取弹幕列表** | 1. 查询视频的所有弹幕<br>2. 按时间戳排序<br>3. 返回弹幕列表 |

##### 3.4.6 视频列表查询

| 方法 | 功能说明 | 数据源 | 排序方式 |
|-----|---------|--------|---------|
| `getMainPageList()` | **主页视频列表** | Redis ZSET | 按上传时间倒序 |
| `getUserPageList()` | **用户视频列表** | Redis ZSET + MySQL | 按上传时间倒序 |
| `getCatgoryPageList()` | **分类视频列表** | Redis ZSET + MySQL | 按上传时间倒序 |
| `getTagPageList()` | **标签视频列表** | Redis ZSET + MySQL | 按上传时间倒序 |
| `getSearchPageList()` | **搜索视频列表** | Elasticsearch | 按相关性得分排序 |
| `getStatusPageList()` | **状态视频列表** | MySQL | 按创建时间倒序 |

**缓存策略**:
- 使用 Redis Sorted Set (ZSET) 存储视频ID列表
- Key 格式：`video:idlist:mainpage`, `video:idlist:user:{uid}`, `video:idlist:catgory:{cid}`, `video:idlist:tag:{tid}`
- Score: 上传时间戳或播放次数
- 查询流程：从缓存获取ID列表 → 批量查询视频详情 → 返回

##### 3.4.7 视频搜索

- **搜索引擎**: Elasticsearch
- **分词器**: `ikmax` (中文分词)
- **搜索字段**: 
  - `videoTitle` (视频标题，权重 3.0)
  - `videoDesc` (视频描述，权重 1.0)
- **搜索流程**:
  1. 构建 `multi_match` 查询
  2. 排除 `videoStatus=false` 的视频
  3. 分页返回视频ID列表
  4. 根据ID列表查询视频详情

#### 数据存储

- **MySQL**: 
  - 视频表、视频分类表、视频标签表
  - 视频点赞表、弹幕表
  - 视频统计表（点赞数、播放数等）
- **Redis**: 
  - 视频ID列表缓存（ZSET）
  - 视频统计缓存（点赞数、播放数）
  - 用户点赞关系缓存
- **Elasticsearch**: 
  - 视频搜索索引
  - 字段：videoId, videoTitle, videoDesc, videoStatus

#### 缓存同步机制

- **双删策略**: 立即删除 → 延迟3秒再删除
- **异步同步**: 通过 RabbitMQ 异步同步缓存到数据库
- **同步队列**: `cache_sync_to_db_queue` (延迟30秒)

#### 消息队列

- **转码队列**: `hls_transcode_queue` - 触发视频转码
- **文件删除队列**: `delete_file_queue` - 异步删除文件
- **缓存同步队列**: `cache_sync_to_db_queue` - 异步同步缓存
- **缓存删除队列**: `cache_delete_queue` - 延迟删除缓存

---

### 3.5 转码服务 (svc_transcode)

**职责**: 视频转码为 HLS 格式，支持流式播放

#### 核心功能

##### 3.5.1 转码流程

```cpp
void transcode(video_id) {
    1. 从MySQL获取视频信息
    2. 获取视频文件ID，从FastDFS下载原始视频
    3. 调用FFmpeg转码:
       - 输入: 原始视频文件
       - 输出: HLS格式 (m3u8 + ts分片)
       - 参数: hls_time=10秒, playlist_type=vod
    4. 上传转码后的文件到FastDFS:
       - m3u8播放列表
       - ts视频分片
    5. 更新视频元数据:
       - 设置HLS播放地址
       - 更新视频状态为ONLINE
    6. 清理临时文件
    7. 更新Elasticsearch (视频可搜索)
}
```

##### 3.5.2 HLS 转码配置

- **分片时长**: 10秒（可配置）
- **播放类型**: VOD (Video On Demand)
- **播放地址**: `/HttpService/downloadVideo/?fileId={m3u8_file_id}`

##### 3.5.3 消息队列处理

- **队列**: `hls_transcode_queue`
- **消息内容**: 视频ID
- **处理方式**: 订阅队列，收到消息后执行转码

##### 3.5.4 转码状态管理

| 状态 | 说明 | 触发时机 |
|-----|------|---------|
| `TRANSCODING` | 转码中 | 视频创建后，发送转码任务到MQ |
| `ONLINE` | 转码完成，已上架 | 转码成功后，更新视频状态 |
| `TRANSCODE_FAILED` | 转码失败 | 转码过程中出现错误 |

#### 技术实现

- **FFmpeg**: 视频转码引擎（封装在 `bitehls::HLSTranscoder`）
- **HLS格式**: 
  - `.m3u8`: 播放列表文件（包含所有ts分片信息）
  - `.ts`: 视频分片文件（每个约10秒）
- **临时文件**: 转码过程中在本地临时目录存储

#### 数据存储

- **MySQL**: 视频表（更新HLS地址和状态）
- **FastDFS**: 转码后的m3u8和ts文件

---

## 4. 数据层模块 (data/)

**职责**: 数据库ORM映射、缓存操作、搜索引擎交互

### 4.1 数据库对象映射 (ODB)

| 模块 | 说明 | 数据库表 |
|-----|------|---------|
| `user.h/cc` | **用户表** | `tbl_user_meta` - 用户基础信息 |
| `session.h/cc` | **会话表** | `tbl_session_meta` - 用户会话信息 |
| `user_identify_role.h/cc` | **身份角色表** | `tbl_user_identify_role` - 用户身份和角色 |
| `user_count.h/cc` | **用户统计表** | `tbl_user_count` - 用户统计数据 |
| `follow.h/cc` | **关注关系表** | `tbl_user_follow` - 用户关注关系 |
| `permission_role.h/cc` | **权限角色表** | `tbl_permission_role` - URI权限与角色映射 |
| `verify_code.h/cc` | **验证码表** | `tbl_verify_code` - 验证码记录（仅用于日志） |
| `file.h/cc` | **文件表** | `tbl_file_meta` - 文件元数据 |
| `video.h/cc` | **视频表** | `tbl_video_meta` - 视频基础信息 |
| `video_catgory_tag.h/cc` | **分类标签表** | `tbl_video_catgory_tag` - 视频分类和标签 |
| `video_like.h/cc` | **视频点赞表** | `tbl_video_like` - 视频点赞关系 |
| `video_count.h/cc` | **视频统计表** | `tbl_video_count` - 视频统计数据 |
| `barrage.h/cc` | **弹幕表** | `tbl_video_barrage` - 视频弹幕 |

### 4.2 缓存操作模块

| 模块 | 说明 | Redis数据结构 |
|-----|------|--------------|
| `session.h/cc` | **会话缓存** | STRING: `session:{ssid}` → 用户ID |
| `verify_code.h/cc` | **验证码缓存** | STRING: `verify:code:{cid}` → 验证码内容 (5分钟过期) |
| `user_count.h/cc` | **用户统计缓存** | HASH: `user:count:{uid}` → {likeCount, playCount, ...} |
| `video_idlist.h/cc` | **视频ID列表缓存** | ZSET: `video:idlist:{type}:{id}` → 视频ID列表 (按时间/播放量排序) |
| `video_count.h/cc` | **视频统计缓存** | HASH: `video:count:{vid}` → {likeCount, playCount, ...} |
| `video_like.h/cc` | **视频点赞缓存** | SET: `video:like:{vid}` → 用户ID集合 |

### 4.3 搜索引擎模块

| 模块 | 说明 | Elasticsearch索引 |
|-----|------|------------------|
| `video_search.h/cc` | **视频搜索** | 索引名: `video`<br>字段: videoId(keyword), videoTitle(text+ikmax), videoDesc(text+ikmax), videoStatus(boolean) |

### 4.4 数据访问模式

- **Cache-Aside**: 优先从缓存读取，缓存未命中时查询数据库并回填缓存
- **Write-Through**: 写入时同时更新缓存和数据库
- **双删策略**: 删除时立即删除缓存，延迟3秒再删除一次（防止并发问题）

---

## 5. 公共模块 (common/)

| 模块 | 说明 |
|-----|------|
| `error.h/cc` | **错误定义** | 定义业务错误码和错误消息 |
| `sync.h` | **缓存同步** | 缓存同步策略接口和实现 |

### 5.1 错误处理

```cpp
enum class VpError {
    SUCCESS = 0,
    SESSION_INVALID,      // 会话无效
    PERMISSION_DENIED,    // 权限不足
    USER_NOT_FOUND,       // 用户不存在
    VIDEO_NOT_FOUND,      // 视频不存在
    // ... 更多错误码
};
```

### 5.2 缓存同步策略

- **CacheToDB**: 缓存数据异步同步到数据库
- **CacheDelete**: 延迟删除缓存（双删策略）

---

## 6. 协议定义 (proto/)

### 6.1 消息定义文件

| 文件 | 说明 | 主要内容 |
|-----|------|---------|
| `base.proto` | **基础消息** | UserInfo, AdminInfo, FileInfo, VideoInfo, BarrageInfo 等 |
| `user.proto` | **用户服务消息** | 用户相关的请求和响应消息 |
| `file.proto` | **文件服务消息** | 文件上传下载相关的请求和响应消息 |
| `video.proto` | **视频服务消息** | 视频管理、播放、搜索相关的请求和响应消息 |
| `message.proto` | **通用消息** | 通用的请求响应消息（如会话登录） |
| `http.proto` | **HTTP服务定义** | HttpService RPC接口定义（网关服务实现） |

### 6.2 RPC 服务定义

| 服务 | 说明 | 主要接口 |
|-----|------|---------|
| `UserService` | **用户服务接口** | tempLogin, sessionLogin, getCode, vcodeLogin, passwdLogin, logout, setAvatar, setNickname, setPassword, setStatus, getUserInfo, newAttention, delAttention, newAdministrator, delAdministrator, setAdministrator, getAdminByEmail, getAdminListByStatus |
| `FileService` | **文件服务接口** | uploadPhoto, downloadPhoto, uploadVideo, downloadVideo |
| `VideoService` | **视频服务接口** | newVideo, removeVideo, judgeLike, setLike, setPlay, newBarrage, getBarrage, checkVideo, saleVideo, haltVideo, userVideoList, statusVideoList, allVideoList, typeVideoList, tagVideoList, keyVideoList |
| `HttpService` | **HTTP网关接口** | 包含上述所有服务的接口，统一对外暴露 |

### 6.3 消息序列化

- **格式**: Protocol Buffers (二进制)
- **优点**: 体积小、序列化快、跨语言
- **使用场景**: RPC通信、数据持久化

---

## 7. 技术栈

### 7.1 核心框架

| 技术 | 版本/说明 | 用途 |
|-----|---------|------|
| **C++17** | C++标准 | 开发语言 |
| **brpc** | Baidu RPC | RPC框架，支持HTTP和RPC协议 |
| **ODB** | Object-Relational Mapping | MySQL ORM框架 |
| **Protobuf** | Protocol Buffers | 数据序列化和RPC消息定义 |

### 7.2 存储与缓存

| 技术 | 用途 | 说明 |
|-----|------|------|
| **MySQL** | 关系型数据库 | 存储业务数据（用户、视频、文件元数据等） |
| **Redis** | 缓存数据库 | 缓存热点数据（会话、统计、ID列表等） |
| **FastDFS** | 分布式文件系统 | 存储大文件（视频、图片） |
| **Elasticsearch** | 搜索引擎 | 视频全文搜索，支持中文分词 |

### 7.3 消息队列

| 技术 | 用途 | 队列说明 |
|-----|------|---------|
| **RabbitMQ** | 消息队列 | 异步任务处理：<br>- 视频转码队列<br>- 文件删除队列<br>- 缓存同步队列<br>- 缓存删除队列 |

### 7.4 服务发现与配置

| 技术 | 用途 | 说明 |
|-----|------|------|
| **etcd** | 服务注册与发现 | 微服务注册中心，服务间通过etcd发现对方 |

### 7.5 多媒体处理

| 技术 | 用途 | 说明 |
|-----|------|------|
| **FFmpeg** | 视频转码 | 将视频转码为HLS格式（封装在bitehls库中） |

### 7.6 构建工具

| 技术 | 用途 |
|-----|------|
| **CMake** | 构建系统 |
| **gflags** | 命令行参数解析 |

---

## 8. 部署架构

### 8.1 服务端口

| 服务 | 端口 | 说明 |
|-----|------|------|
| 网关服务 | 9000 | HTTP入口 |
| 用户服务 | 9001 | RPC服务 |
| 文件服务 | 9002 | RPC服务 |
| 视频服务 | 9003 | RPC服务 |
| 转码服务 | 9004 | RPC服务（可选，也可作为后台任务） |

### 8.2 基础设施端口

| 服务 | 端口 | 说明 |
|-----|------|------|
| MySQL | 3306 | 数据库 |
| Redis | 6379 | 缓存 |
| RabbitMQ | 5672 | 消息队列 |
| etcd | 2379 | 服务注册中心 |
| Elasticsearch | 9200 | 搜索引擎 |
| FastDFS | 22122 | 文件存储Tracker |

### 8.3 部署方式

- **Docker Compose**: 支持容器化部署（见 `docker-compose.yml`）
- **独立部署**: 各服务可独立编译和运行
- **配置文件**: 通过 gflags 或 `global.conf` 配置服务参数

### 8.4 扩展性

- **水平扩展**: 各微服务可独立扩容（通过etcd负载均衡）
- **缓存扩展**: Redis集群支持
- **存储扩展**: FastDFS分布式存储支持
- **搜索扩展**: Elasticsearch集群支持

---

## 9. 典型业务流程

### 9.1 用户上传视频流程

```
1. 用户登录 → 获取会话ID
2. 上传视频文件 → 文件服务 → FastDFS存储 → 返回文件ID
3. 创建视频记录 → 视频服务 → MySQL存储 → 状态=AUDITING
4. 触发转码任务 → 发送MQ消息 → 转码服务接收
5. 转码服务处理 → 下载视频 → FFmpeg转码 → 上传HLS文件
6. 更新视频状态 → 状态=TRANSCODING → 完成后=ONLINE
7. 管理员审核 → 审核通过 → 状态=ONLINE → Elasticsearch可搜索
```

### 9.2 用户播放视频流程

```
1. 用户搜索/浏览 → 获取视频列表
2. 选择视频 → 获取视频详情（含HLS地址）
3. 播放视频 → 记录播放次数 → 更新统计缓存
4. 发送弹幕 → 保存弹幕到MySQL
5. 获取弹幕 → 实时显示弹幕
```

### 9.3 缓存同步流程

```
1. 业务操作 → 更新MySQL和Redis
2. 发送同步消息 → 缓存同步队列（延迟30秒）
3. 异步处理 → 将缓存数据同步回数据库
4. 删除操作 → 立即删除缓存 → 延迟3秒再删除（双删策略）
```

---

## 10. 性能优化策略

### 10.1 缓存策略

- **热点数据缓存**: 用户信息、视频统计、视频ID列表
- **缓存过期**: 验证码5分钟，其他数据根据业务需求
- **缓存预热**: 系统启动时可预加载热点数据

### 10.2 数据库优化

- **索引优化**: 对常用查询字段建立索引
- **分页查询**: 所有列表接口支持分页
- **连接池**: MySQL和Redis使用连接池减少连接开销

### 10.3 异步处理

- **转码异步**: 视频转码通过MQ异步处理，不阻塞用户请求
- **文件删除异步**: 大文件删除异步处理，避免阻塞
- **缓存同步异步**: 缓存更新异步同步到数据库

### 10.4 搜索优化

- **分词优化**: 使用ikmax中文分词器
- **字段权重**: 标题权重3.0，描述权重1.0
- **索引优化**: 只索引已上架的视频

---

## 11. 安全性设计

### 11.1 权限控制

- **角色权限**: 基于角色的访问控制（RBAC）
- **URI权限**: 每个URI对应不同角色的访问权限
- **会话验证**: 所有接口需要验证会话有效性

### 11.2 数据安全

- **密码加密**: 用户密码MD5加密存储
- **文件访问控制**: 文件下载需要会话验证
- **管理员权限**: 管理员操作需要额外权限验证

---

## 12. 总结

本项目是一个完整的微服务视频点播系统，采用C++开发，具有以下特点：

✅ **微服务架构**: 职责清晰，易于扩展和维护  
✅ **高性能**: 缓存、异步处理、连接池等优化  
✅ **可扩展性**: 水平扩展、分布式存储  
✅ **功能完整**: 用户管理、视频管理、搜索、转码、弹幕等  
✅ **技术先进**: 使用现代C++、Protobuf、brpc等框架  

适用于中小型视频点播平台，可扩展为大型视频平台。





