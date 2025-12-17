# 视频上传数据流向详解

## 📋 目录

- [1. 整体架构数据流向](#1-整体架构数据流向)
- [2. 视频上传完整流程](#2-视频上传完整流程)
- [3. 各服务详细处理](#3-各服务详细处理)
- [4. 数据存储位置](#4-数据存储位置)
- [5. 转码服务详细流程](#5-转码服务详细流程)

---

## 1. 整体架构数据流向

```
┌─────────────────────────────────────────────────────────────────┐
│                        客户端 (Qt/Web)                           │
│  上传视频文件（二进制数据）                                        │
└──────────────────┬──────────────────────────────────────────────┘
                   │ HTTP POST
                   │ Content-Type: video/mp4
                   │ Body: 视频二进制数据
                   ▼
┌─────────────────────────────────────────────────────────────────┐
│                      网关服务 (svc_gateway)                      │
│  端口: 9000                                                      │
│  - 权限验证                                                      │
│  - HTTP → RPC 协议转换                                           │
│  - 服务发现 (etcd)                                               │
└──────┬────────────────────────┬─────────────────────────────────┘
       │                        │
       │ HTTP (大数据传输)       │ RPC (小数据)
       │                        │
       ▼                        ▼
┌─────────────────┐    ┌─────────────────┐
│  文件服务        │    │  视频服务        │
│  (svc_file)     │    │  (svc_video)    │
│  端口: 9002     │    │  端口: 9003     │
└─────┬───────────┘    └─────┬───────────┘
      │                      │
      │ FastDFS              │ MySQL + Redis
      │ MQ (转码)            │ ES (搜索)
      ▼                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                      基础设施层                                    │
│  FastDFS (文件存储) │ MySQL (元数据) │ Redis (缓存) │ ES (搜索)  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. 视频上传完整流程

### 流程图

```
客户端                   网关服务                 文件服务             视频服务             转码服务            FastDFS          MySQL
  │                        │                        │                    │                    │                  │                │
  │───── HTTP POST ───────>│                        │                    │                    │                  │                │
  │  上传视频文件            │                        │                    │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │── 权限验证 ──────────>  │                    │                    │                  │                │
  │                        │  会话ID验证             │                    │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │── HTTP POST ──────────>│                    │                    │                  │                │
  │                        │  转发视频数据            │                    │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │── 会话验证 ────────>│                    │                  │                │
  │                        │                        │  获取用户ID         │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │── 上传到FastDFS ───┼──────────────────>│                  │                │
  │                        │                        │  保存视频文件       │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │── 保存文件元数据 ───┼──────────────────────────>│                │
  │                        │                        │  文件ID、路径等     │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │<── 返回文件ID ─────────│                    │                    │                  │                │
  │<── 返回文件ID ─────────│                        │                    │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │───── 创建视频记录 ─────>│                        │                    │                    │                  │                │
  │  上传封面、标题等        │                        │                    │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │── RPC 调用 ────────────┼──────────────────>│                    │                  │                │
  │                        │  创建视频元数据          │                    │                    │                  │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │── 保存视频元数据 ───┼──────────────────>│                │
  │                        │                        │                    │  视频ID、标题等     │                    │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │── 保存分类标签 ────┼──────────────────>│                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │── 发布转码消息 ────┼────────>│          │                │
  │                        │                        │                    │  发送到MQ          │                    │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │                    │── 接收转码消息 ──>│                │
  │                        │                        │                    │                    │  从MQ获取视频ID   │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │                    │── 下载原始视频 ───┼───────────────>│
  │                        │                        │                    │                    │  从FastDFS下载    │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │                    │── FFmpeg转码 ────>│                │
  │                        │                        │                    │                    │  生成HLS文件      │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │                    │── 上传HLS文件 ────┼───────────────>│
  │                        │                        │                    │                    │  m3u8 + ts分片    │                │
  │                        │                        │                    │                    │                  │                │
  │                        │                        │                    │── 更新视频状态 ────┼──────────────────>│                │
  │                        │                        │                    │  转码完成           │                    │                │
  │                        │                        │                    │                    │                  │                │
  │<── 返回视频ID ─────────│<── 返回视频ID ─────────┼───────────────────│                    │                  │                │
```

---

## 3. 各服务详细处理

### 3.1 网关服务 (svc_gateway) - 端口 9000

#### 功能：统一HTTP入口，协议转换

**代码位置**: `svc_gateway/source/svc_rpc.cc` 的 `uploadVideo()` 方法

#### 处理流程：

```cpp
void HttpServiceImpl::uploadVideo(...) {
    // 1. 提取HTTP请求信息
    - 从URI获取 sessionId 和 requestId
    - 从HTTP Headers获取 Content-Type (MIME类型)
    - 从HTTP Body获取视频二进制数据
    
    // 2. 权限验证
    - 调用 _svc_data->judgePermission(ssid, uri)
    - 检查用户是否有上传视频权限
    
    // 3. 构建RPC请求对象
    api_message::UploadVideoReq rpc_req;
    rpc_req.set_sessionid(sessionId);
    rpc_req.set_requestid(requestId);
    rpc_req.mutable_fileinfo()->set_filemime(mime);      // 视频MIME类型
    rpc_req.mutable_fileinfo()->set_filedata(video_data); // 视频二进制数据
    
    // 4. 通过etcd服务发现，获取文件服务地址
    auto node_addr = _channels->getNodeAddr("file");
    
    // 5. 创建HTTP协议Channel（用于大数据传输）
    brpc::Channel channel;
    channel.Init(node_addr, HTTP_PROTOCOL);
    
    // 6. 发起HTTP请求到文件服务
    POST /FileService/uploadVideo
    Content-Type: application/protobuf
    Body: rpc_req.SerializeAsString()
    
    // 7. 接收响应并转换格式
    - 接收文件服务返回的文件ID
    - 将Protobuf响应转换为JSON
    - 返回给客户端
}
```

**关键点**：
- ✅ 使用 **HTTP协议** 而不是普通RPC（因为视频文件较大）
- ✅ **权限验证** 在网关层完成
- ✅ **协议转换**：客户端HTTP → 文件服务HTTP

---

### 3.2 文件服务 (svc_file) - 端口 9002

#### 功能：文件上传下载、FastDFS管理

**代码位置**: `svc_file/source/svc_rpc.cc` 的 `uploadVideo()` 方法

#### 处理流程：

```cpp
void FileServiceImpl::uploadVideo(...) {
    // 1. 解析HTTP请求正文（Protobuf格式）
    api_message::UploadVideoReq req;
    req.ParseFromString(http_cntl->request_attachment());
    
    // 2. 会话验证，获取用户ID
    std::string user_id = _svc_data->getSessionUser(req.sessionid());
    //   - 从Redis缓存查询会话（未命中则查询MySQL）
    //   - 获取会话关联的用户ID
    
    // 3. 上传视频文件到FastDFS
    std::string video_data = req.fileinfo().filedata();  // 视频二进制数据
    auto file_path = bitefdfs::FDFSClient::upload_from_buff(video_data);
    //   - 调用FastDFS客户端API
    //   - 上传文件到FastDFS Storage服务器
    //   - 返回FastDFS文件路径（如：group1/M00/00/00/xxx.mp4）
    
    // 4. 生成文件ID（16位随机字符串）
    std::string file_id = biteutil::Random::code(16);
    
    // 5. 创建文件元数据对象
    vp_data::File file;
    file.set_file_id(file_id);           // 文件ID
    file.set_uploader_uid(user_id);      // 上传用户ID
    file.set_path(*file_path);           // FastDFS存储路径
    file.set_size(video_data.size());    // 文件大小
    file.set_mime(req.fileinfo().filemime()); // MIME类型
    
    // 6. 保存文件元数据到MySQL
    _svc_data->newFileMeta(file);
    //   - 插入到 tbl_file_meta 表
    //   - 字段：file_id, uploader_uid, path, size, mime
    
    // 7. 返回文件ID给网关
    response.set_fileid(file_id);
}
```

**数据流向**：
```
客户端视频数据 
    ↓
网关服务（转发）
    ↓
文件服务（接收）
    ↓
FastDFS（存储原始视频文件）
    ↓
MySQL（存储文件元数据：file_id, path, size, mime）
```

**关键点**：
- ✅ **FastDFS存储**：实际视频文件存储在FastDFS
- ✅ **MySQL存储**：只存储文件元数据（ID、路径、大小等）
- ✅ **文件ID**：生成16位随机字符串作为文件唯一标识

---

### 3.3 视频服务 (svc_video) - 端口 9003

#### 功能：视频元数据管理、分类标签、触发转码

**代码位置**: `svc_video/source/svc_data.cc` 的 `newVideo()` 方法

#### 处理流程：

```cpp
void SvcData::newVideo(ssid, videoInfo) {
    // 1. 开启数据库事务
    odb::transaction mtx(_mysql->begin());
    
    // 2. 会话验证，获取用户ID
    std::string user_id = getSessionUser(ssid, mtx);
    
    // 3. 生成视频ID（16位随机字符串）
    std::string video_id = biteutil::Random::code(16);
    
    // 4. 创建视频元数据对象
    vp_data::Video video;
    video.set_video_id(video_id);                    // 视频ID
    video.set_title(videoInfo.videotitle());         // 视频标题
    video.set_summary(videoInfo.videodesc());        // 视频描述
    video.set_cover_fid(videoInfo.photofileid());    // 封面文件ID
    video.set_video_fid(videoInfo.videofileid());    // 视频文件ID（来自文件服务）
    video.set_uploader_uid(user_id);                 // 上传用户ID
    video.set_duration(videoInfo.duration());        // 视频时长
    video.set_status(TRANSCODING);                   // 初始状态：转码中
    
    // 5. 保存视频元数据到MySQL
    vp_data::VideoData vd(mtx, _redis, _cache_delete_sync);
    vd.insert(video);
    //   - 插入到 tbl_video_meta 表
    //   - 字段：video_id, title, cover_fid, video_fid, uploader_uid, status等
    
    // 6. 保存视频分类标签关系
    vp_data::VideoCatgoryTagData vctd(mtx);
    for (auto tag_id : videoInfo.videotag()) {
        vctd.insert(video_id, videoInfo.videotype(), tag_id);
        //   插入到 tbl_video_catgory_tag_meta 表
        //   字段：video_id, catgory_id, tag_id
    }
    
    // 7. 提交事务
    mtx.commit();
    
    // 8. 发布转码消息到RabbitMQ
    api_message::HLSTranscodeMsg msg;
    msg.add_video_id(video_id);
    _transcode_publisher->publish(msg.SerializeAsString());
    //   - 队列：hls_transcode_queue
    //   - 消息内容：视频ID
}
```

**数据流向**：
```
视频元数据（标题、描述、封面等）
    ↓
视频服务（接收）
    ↓
MySQL（存储视频元数据和分类标签）
    ↓
RabbitMQ（发送转码消息）
    ↓
转码服务（接收消息）
```

**关键点**：
- ✅ **事务保证**：视频元数据和分类标签在同一事务中保存
- ✅ **异步转码**：通过MQ触发转码任务，不阻塞用户请求
- ✅ **初始状态**：视频状态设为 `TRANSCODING`（转码中）

---

### 3.4 转码服务 (svc_transcode) - 端口 9004

#### 功能：视频转码为HLS格式

**代码位置**: `svc_transcode/source/svc_mq.cc` 的 `transcode()` 方法

#### 处理流程：

```cpp
void TranscodeMQ::transcode(video_id) {
    // 1. 定义本地临时文件路径
    std::string local_path = _temp_path + "/" + video_id;      // 原始视频
    std::string m3u8_path = _temp_path + "/" + video_id + ".m3u8"; // m3u8播放列表
    
    // 2. 从MySQL获取视频元信息
    auto video = _svc_data->getVideo(video_id);
    std::string file_id = video->video_fid();  // 视频文件ID
    
    // 3. 从MySQL获取文件元信息
    auto file = _svc_data->getFile(file_id);
    std::string fdfs_path = file->path();  // FastDFS存储路径
    
    // 4. 从FastDFS下载原始视频到本地
    bitefdfs::FDFSClient::download_to_file(fdfs_path, local_path);
    //   - 下载原始MP4文件
    //   - 保存到临时目录
    
    // 5. 调用FFmpeg进行HLS转码
    _transcoder->transcode(local_path, m3u8_path);
    //   - 输入：原始视频文件（MP4）
    //   - 输出：HLS格式文件
    //     * m3u8播放列表文件（包含所有ts分片信息）
    //     * ts视频分片文件（每个约10秒）
    //   - 参数：hls_time=10秒, playlist_type=vod
    
    // 6. 解析m3u8文件，获取所有ts分片信息
    bitehls::M3U8Info m3u8_info(m3u8_path);
    m3u8_info.parse();
    auto &ts_pieces = m3u8_info.pieces();  // ts分片列表
    
    // 7. 上传每个ts分片到FastDFS
    for (auto &piece : ts_pieces) {
        std::string ts_local_path = _temp_path + "/" + piece.second;
        
        // 上传ts文件到FastDFS
        auto ts_remote_path = bitefdfs::FDFSClient::upload_from_file(ts_local_path);
        
        // 为ts文件创建文件元数据
        std::string ts_file_id = biteutil::Random::code();
        vp_data::File ts_file;
        ts_file.set_file_id(ts_file_id);
        ts_file.set_path(*ts_remote_path);
        ts_file.set_size(filesystem::file_size(ts_local_path));
        ts_file.set_mime("video/MP2T");
        ts_file.set_uploader_uid(file->uploader_uid());
        _svc_data->newFile(ts_file);  // 保存到MySQL
        
        // 更新m3u8文件中的ts路径
        piece.second = _access_url + ts_file_id;
        //   例如：/HttpService/downloadVideo/?fileId=ts_file_id_1
    }
    
    // 8. 上传m3u8播放列表到FastDFS
    m3u8_info.write();  // 写入更新后的路径
    auto m3u8_remote_path = bitefdfs::FDFSClient::upload_from_file(m3u8_path);
    
    // 9. 更新原始文件元数据
    file->set_path(*m3u8_remote_path);  // 更新为m3u8路径
    file->set_size(filesystem::file_size(m3u8_path));
    file->set_mime("application/x-mpegURL");  // MIME类型改为m3u8
    _svc_data->setFile(file);  // 更新到MySQL
    
    // 10. 更新视频状态
    video->set_status(AUDITING);  // 转码完成，状态改为审核中
    _svc_data->setVideo(video);
    
    // 11. 删除FastDFS中的原始视频文件（已不需要）
    bitefdfs::FDFSClient::remove(fdfs_path);
    
    // 12. 清理本地临时文件
    filesystem::remove(local_path);
    filesystem::remove(m3u8_path);
    for (auto &ts_path : ts_pieces) {
        filesystem::remove(ts_path);
    }
}
```

**数据流向**：
```
RabbitMQ消息（视频ID）
    ↓
转码服务（接收消息）
    ↓
MySQL（查询视频和文件元数据）
    ↓
FastDFS（下载原始视频文件）
    ↓
本地临时目录（存储原始视频）
    ↓
FFmpeg（转码为HLS格式）
    ↓
本地临时目录（生成m3u8和ts文件）
    ↓
FastDFS（上传ts分片文件）
    ↓
MySQL（保存ts文件元数据）
    ↓
FastDFS（上传m3u8播放列表）
    ↓
MySQL（更新原始文件元数据为m3u8路径）
    ↓
MySQL（更新视频状态为审核中）
```

**关键点**：
- ✅ **异步处理**：通过线程池处理转码任务，避免阻塞MQ消费
- ✅ **HLS格式**：转码为m3u8播放列表 + ts分片（支持流式播放）
- ✅ **文件管理**：每个ts分片都有独立的文件ID和元数据
- ✅ **状态更新**：转码完成后，视频状态从 `TRANSCODING` 变为 `AUDITING`

---

## 4. 数据存储位置

### 4.1 文件存储（FastDFS）

| 文件类型 | 存储位置 | 说明 |
|---------|---------|------|
| **原始视频** | FastDFS | MP4格式，上传后即存储，转码完成后删除 |
| **封面图片** | FastDFS | JPG/PNG格式 |
| **HLS播放列表** | FastDFS | m3u8文件 |
| **HLS视频分片** | FastDFS | ts文件，每个约10秒 |

### 4.2 数据库存储（MySQL）

#### 表：`tbl_file_meta` - 文件元数据

| 字段 | 说明 | 示例 |
|-----|------|------|
| `file_id` | 文件唯一ID（16位随机字符串） | `abc123def4567890` |
| `uploader_uid` | 上传用户ID | `user_id_1` |
| `path` | FastDFS存储路径 | `group1/M00/00/00/xxx.mp4` |
| `size` | 文件大小（字节） | `10485760` |
| `mime` | MIME类型 | `video/mp4`, `application/x-mpegURL` |

#### 表：`tbl_video_meta` - 视频元数据

| 字段 | 说明 | 示例 |
|-----|------|------|
| `video_id` | 视频唯一ID | `video_id_123` |
| `video_fid` | 视频文件ID（关联`tbl_file_meta`） | `abc123def4567890` |
| `cover_fid` | 封面文件ID | `cover_fid_123` |
| `uploader_uid` | 上传用户ID | `user_id_1` |
| `title` | 视频标题 | `美食视频：红烧肉制作` |
| `summary` | 视频描述 | `详细讲解红烧肉的制作方法...` |
| `status` | 视频状态 | `TRANSCODING`, `AUDITING`, `ONLINE` |
| `upload_time` | 上传时间 | `2024-01-01 12:00:00` |

#### 表：`tbl_video_catgory_tag_meta` - 视频分类标签

| 字段 | 说明 | 示例 |
|-----|------|------|
| `video_id` | 视频ID | `video_id_123` |
| `catgory_id` | 分类ID（整数） | `1`（美食分类） |
| `tag_id` | 标签ID（整数） | `1`, `2`, `3` |

### 4.3 缓存存储（Redis）

| Key | 类型 | 说明 |
|-----|------|------|
| `session:{ssid}` | STRING | 会话信息，值为用户ID |
| `video:idlist:mainpage` | ZSET | 主页视频ID列表（按时间排序） |
| `video:idlist:catgory:{cid}` | ZSET | 分类视频ID列表 |
| `video:count:{vid}` | HASH | 视频统计数据（点赞数、播放数） |

### 4.4 搜索引擎（Elasticsearch）

| 索引 | 字段 | 说明 |
|-----|------|------|
| `video` | `videoId` | 视频ID（keyword类型） |
| | `videoTitle` | 视频标题（text类型，ikmax分词，权重3.0） |
| | `videoDesc` | 视频描述（text类型，ikmax分词，权重1.0） |
| | `videoStatus` | 视频状态（boolean类型，false=不可搜索） |

---

## 5. 转码服务详细流程

### 5.1 消息队列处理

**代码位置**: `svc_transcode/source/svc_mq.cc` 的 `callback()` 方法

```cpp
void TranscodeMQ::callback(body, len) {
    // 1. 反序列化消息
    api_message::HLSTranscodeMsg msg;
    msg.ParseFromArray(body, len);
    
    // 2. 提取视频ID列表
    for (int i = 0; i < msg.video_id_size(); i++) {
        std::string video_id = msg.video_id(i);
        
        // 3. 将转码任务添加到线程池
        auto task = std::bind(&TranscodeMQ::transcode, this, video_id);
        _worker->addTask(std::move(task));
    }
}
```

**线程池处理**（`svc_transcode/source/svc_worker.cc`）：
- 多线程处理转码任务（默认多个线程）
- 任务队列：FIFO队列
- 并发控制：使用条件变量和互斥锁

### 5.2 FFmpeg转码参数

**配置位置**: `svc_transcode/source/main.cc`

```cpp
DEFINE_int32(hls_time, 10, "HLS分片时长");              // 每个ts分片10秒
DEFINE_string(hls_play_type, "vod", "HLS播放类型");     // VOD（点播）类型
DEFINE_string(hls_base_url, "/HttpService/downloadVideo/?fileId=", "HLS基础URL");
```

**生成的m3u8文件示例**：
```m3u8
#EXTM3U
#EXT-X-VERSION:3
#EXT-X-TARGETDURATION:10
#EXTINF:10.0,
/HttpService/downloadVideo/?fileId=ts_file_id_1
#EXTINF:10.0,
/HttpService/downloadVideo/?fileId=ts_file_id_2
#EXTINF:5.0,
/HttpService/downloadVideo/?fileId=ts_file_id_3
#EXT-X-ENDLIST
```

### 5.3 转码后的文件结构

```
FastDFS存储:
├── group1/M00/00/00/m3u8_file.m3u8          (m3u8播放列表)
├── group1/M00/00/00/ts_file_1.ts            (ts分片1)
├── group1/M00/00/00/ts_file_2.ts            (ts分片2)
├── group1/M00/00/00/ts_file_3.ts            (ts分片3)
└── ...

MySQL数据库:
├── tbl_file_meta
│   ├── file_id: m3u8_file_id (mime: application/x-mpegURL)
│   ├── file_id: ts_file_id_1 (mime: video/MP2T)
│   ├── file_id: ts_file_id_2 (mime: video/MP2T)
│   └── file_id: ts_file_id_3 (mime: video/MP2T)
└── tbl_video_meta
    └── video_fid: m3u8_file_id (指向m3u8文件)
```

---

## 6. 完整时序图

```
客户端          网关           文件服务        视频服务        转码服务        FastDFS      MySQL      RabbitMQ
  │              │               │               │               │              │            │           │
  │──上传视频────>│               │               │               │              │            │           │
  │              │──权限验证────>│               │               │              │            │           │
  │              │               │               │               │              │            │           │
  │              │──转发视频────>│               │               │               │              │            │           │
  │              │  HTTP         │               │               │              │            │           │
  │              │               │──上传FastDFS──┼──────────────>│              │            │           │
  │              │               │               │               │              │            │           │
  │              │               │──保存元数据────┼─────────────────────────────>│            │           │
  │              │               │               │               │              │            │           │
  │              │<──返回文件ID──│               │               │              │            │           │
  │<──返回文件ID──│               │               │               │              │            │           │
  │              │               │               │               │              │            │           │
  │──创建视频───>│               │               │               │              │            │           │
  │              │──RPC调用──────┼──────────────>│               │              │            │           │
  │              │               │               │──保存视频──────┼──────────────>│            │           │
  │              │               │               │──保存分类──────┼──────────────>│            │           │
  │              │               │               │──发布转码──────┼───────────────────────────>│           │
  │              │               │               │               │              │            │           │
  │              │               │               │               │<──接收消息────┼───────────────────────│
  │              │               │               │               │              │            │           │
  │              │               │               │               │──查询视频──────┼──────────>│            │           │
  │              │               │               │               │──查询文件──────┼──────────>│            │           │
  │              │               │               │               │──下载视频──────┼──────────>│            │           │
  │              │               │               │               │              │            │           │
  │              │               │               │               │──FFmpeg转码───>│            │           │
  │              │               │               │               │              │            │           │
  │              │               │               │               │──上传ts分片────┼──────────>│            │           │
  │              │               │               │               │──保存ts元数据──┼──────────>│            │           │
  │              │               │               │               │──上传m3u8──────┼──────────>│            │           │
  │              │               │               │               │──更新文件──────┼──────────>│            │           │
  │              │               │               │               │──更新视频──────┼──────────>│            │           │
  │              │               │               │               │              │            │           │
  │<──返回视频ID──│<──返回视频ID──┼───────────────│               │              │            │           │
```

---

## 7. 关键数据流向总结

### 上传阶段
1. **客户端** → **网关**：HTTP POST，视频二进制数据
2. **网关** → **文件服务**：HTTP POST（转发），Protobuf消息
3. **文件服务** → **FastDFS**：上传原始视频文件
4. **文件服务** → **MySQL**：保存文件元数据

### 创建视频阶段
1. **客户端** → **网关**：RPC请求，视频元数据（标题、描述等）
2. **网关** → **视频服务**：RPC调用
3. **视频服务** → **MySQL**：保存视频元数据和分类标签
4. **视频服务** → **RabbitMQ**：发布转码消息

### 转码阶段
1. **RabbitMQ** → **转码服务**：接收转码消息
2. **转码服务** → **MySQL**：查询视频和文件元数据
3. **转码服务** → **FastDFS**：下载原始视频
4. **转码服务** → **FFmpeg**：本地转码（生成m3u8和ts）
5. **转码服务** → **FastDFS**：上传ts分片和m3u8
6. **转码服务** → **MySQL**：保存ts元数据，更新文件元数据，更新视频状态

---

## 8. 注意事项

1. **文件上传使用HTTP协议**：因为视频文件较大，使用HTTP而不是普通RPC
2. **异步转码**：转码通过MQ异步处理，不阻塞用户请求
3. **临时文件清理**：转码完成后会自动清理本地临时文件
4. **原始视频删除**：转码完成后会删除FastDFS中的原始视频，节省存储空间
5. **事务保证**：视频元数据和分类标签在同一事务中保存，保证一致性
6. **状态管理**：视频状态流转：`TRANSCODING` → `AUDITING` → `ONLINE`

---

这份文档详细说明了视频上传的完整数据流向和处理流程。如需进一步了解某个环节，请告诉我！


