## **protobuf**

```json
syntax = "proto3";
package namespace;
option cc_generic_services = true;

message request {
    parameter;
};

message response{
     parameter;
};

//http
message httpRequest {
}
message httpRespond {
}

service rpcServer {
    rpc rpcCall(request) returns (respond);
    rpc httpCall(httpRequest ) returns (httpRespond );
};
```

**protobuf编译后会生成request、 response、httpRequest 、httpRespond 、rpcServer 、rpcServer\_Stub**

**注意事项：**

- `option cc_generic_services = true;` 必须设置，否则不会生成服务基类和Stub类，无法使用brpc的RPC功能
- HTTP请求的message通常为空（如httpRequest、httpRespond），因为HTTP请求的详细信息通过Controller获取，不需要在protobuf中定义
- service中定义的rpc方法名会直接影响HTTP路径，格式为 `/ServiceName/MethodName`
- rpcServer由服务器继承并实现虚函数
- 客户端通过rpcServer\_Stub发起请求

## **server基本使用**

### **服务类设计**

```cpp
class rpcServerMy : public namespace::rpcServer {
    // RPC方法实现
    void rpcCall(RpcController* controller,
                 const request* request,
                 response* response,
                 Closure* done) override {
        ClosureGuard done_guard(done);  // 必须使用，管理done回调
        // 处理请求
        response->set_result(处理逻辑);
    }
  
    // HTTP方法实现
    void httpCall(RpcController* controller,
                  const httpRequest* request,
                  httpRespond* response,
                  Closure* done) override {
        ClosureGuard done_guard(done);
        Controller* cntl = (Controller*)controller;
  
        // 获取请求信息
        cntl->http_request().method();
        cntl->request_attachment().to_string();
  
        // 设置响应
        cntl->response_attachment().append(响应内容);
        cntl->http_response().set_status_code(200);
    }
};
```

**注意事项：**

- **必须继承protobuf生成的服务基类**：`namespace::rpcServer` 是protobuf编译后自动生成的基类，包含所有RPC方法的纯虚函数声明
- **ClosureGuard必须使用**：`ClosureGuard done_guard(done)` 是RAII机制，当函数返回时会自动调用 `done->Run()` 完成RPC调用。如果忘记使用，RPC调用永远不会完成，客户端会一直等待
- **Controller类型转换**：在HTTP方法中需要将 `RpcController*` 转换为 `brpc::Controller*`，因为HTTP相关的API都在brpc::Controller中
- **HTTP响应必须设置状态码**：`set_status_code(200)` 必须调用，否则客户端可能收到默认错误状态码
- **request和response的生命周期**：这些对象由brpc框架管理，函数返回后会被销毁，不要保存指针或引用
- **异步调用时：**在实现部分开一个线程将 `ClosureGuard done_guard(done)`放入新线程，进行线程分离就实现了异步

### **服务器启动流程**

```cpp
// 1. 创建服务实例
rpcServerMy service;

// 2. 配置服务器选项
ServerOptions options;
options.idle_timeout_sec = -1;  // -1表示不超时

// 3. 创建服务器并添加服务
Server server;
server.AddService(&service, brpc::SERVER_DOESNT_OWN_SERVICE);

// 4. 启动服务器
server.Start(端口, &options);

// 5. 等待停止
server.RunUntilAskedToQuit();
```

**注意事项：**

- **服务对象生命周期**：`rpcServerMy service` 必须在server运行期间保持有效，如果使用栈对象，要确保在server停止前不析构
- **SERVER_DOESNT_OWN_SERVICE参数**：表示server不拥有service对象的所有权，service的生命周期由调用者管理。如果使用 `SERVER_OWNS_SERVICE`，server会在停止时自动删除service对象
- **idle_timeout_sec = -1**：设置为-1表示连接永不超时，适合长连接场景。如果设置为正数，空闲连接会在指定秒数后自动关闭
- **Start失败检查**：必须检查返回值，如果端口被占用或其他原因导致启动失败，需要处理错误
- **RunUntilAskedToQuit是阻塞调用**：会一直阻塞直到收到停止信号（如SIGINT），适合单线程场景。如果需要非阻塞，可以使用其他方法

## **client基本使用**

### **HTTP客户端设计**

```cpp
// 1. 配置Channel（设置协议为HTTP）
ChannelOptions options;
options.protocol = PROTOCOL_HTTP;

// 2. 初始化Channel
Channel channel;
channel.Init(服务器地址, &options);

// 3. 配置Controller和请求
Controller cntl;
cntl.http_request().set_method(HTTP_METHOD_POST);
cntl.http_request().uri().set_path("/ServiceName/MethodName");
cntl.request_attachment().append(请求体);

// 4. 发起调用
channel.CallMethod(nullptr, &cntl, nullptr, nullptr, nullptr);

// 5. 处理响应
if (!cntl.Failed()) {
    cntl.response_attachment();  // 获取响应
} else {
    cntl.ErrorText();  // 获取错误
}
```

**注意事项：**

- **协议必须匹配**：`options.protocol = PROTOCOL_HTTP` 必须设置，否则Channel不知道使用什么协议进行通信，默认可能使用RPC协议导致调用失败
- **Channel.Init必须在调用前完成**：Init失败会导致后续所有调用失败，应该检查返回值
- **HTTP路径格式严格**：`/ServiceName/MethodName` 格式必须正确，ServiceName是protobuf中service的名称，MethodName是rpc方法名，大小写敏感
- **CallMethod参数说明**：`channel.CallMethod(nullptr, &cntl, nullptr, nullptr, nullptr)` 对于HTTP调用，后三个参数（method_descriptor, request, response, done）都传nullptr，因为HTTP请求信息都在Controller中
- **必须检查Failed()**：调用后必须检查 `cntl.Failed()`，因为网络错误、超时、服务器错误等都会导致调用失败，直接使用响应会导致未定义行为
- **response_attachment是IOBuf类型**：需要使用 `to_string()` 或 `copy_to()` 等方法获取内容，不能直接当字符串使用

### **RPC客户端设计**

```cpp
// 1. 配置Channel（设置协议为标准RPC）
ChannelOptions options;
options.protocol = PROTOCOL_BAIDU_STD;

#if ASYNC
//以便二次封装用同一个回调包装器统一处理所有 RPC
using callback_t = std::function<void()>;
struct Object {
    callback_t callback;
};
#endif
void Callback(const Object obj) {
    obj.callback();
}
// 2. 初始化Channel
Channel channel;
channel.Init(服务器地址, &options);

// 3. 创建Stub对象
rpcServer_Stub stub(&channel);

// 4. 准备请求响应对象
request request;
response response;
req.set_parameter(参数值);

// 5. 发起RPC调用

#if SYNC
   //同步
   Controller cntl;
   stub.rpcCall(&cntl, &request, &response, nullptr);
   //处理响应
   if (!cntl.Failed()) {
       rsp.result();  // 获取结果
   }

#else ASYNC
   //异步
   Object obj;
       obj.callback = [=]() {
           std::unique_ptr<brpc::Controller> cntl_guard(cntl);
           std::unique_ptr<cal::request> req_guard(request);
           std::unique_ptr<cal::response> rsp_guard(response);
           if (cntl_guard->Failed() == true) {
              std::cout << "rpc请求失败: " << cntl_guard->ErrorText() << std::endl;
              return ;
           }
       	   std::cout << rsp_guard->result() << std::endl;
       };
       google::protobuf::Closure* closure = brpc::NewCallback(Callback, obj);
       stub.Add(cntl, request, response, closure);
       getchar();

#endif


```

**注意事项：**

- **协议必须设置为RPC协议**：`PROTOCOL_BAIDU_STD` 是brpc的标准RPC协议，与HTTP协议不同，需要明确指定
- **Stub对象依赖Channel**：`rpcServer_Stub stub(&channel)` 中传入的channel指针必须在stub使用期间保持有效，通常stub和channel的生命周期应该一致
- **请求响应对象生命周期**：`request req` 和 `response rsp` 必须在调用完成前保持有效，因为stub内部会使用这些对象的指针
- **同步调用**：`stub.rpcCall(&cntl, &req, &rsp, nullptr)` 最后一个参数为nullptr表示同步调用，会阻塞直到收到响应。如果需要异步调用，需要传入Closure回调
- **Controller可以复用**：同一个Controller对象可以用于多次调用，但每次调用前应该重置状态（或创建新的Controller）
- **错误处理**：即使调用失败，也应该检查rsp中的内容，某些情况下可能包含部分错误信息

## **brpc::Controller**

`brpc::Controller` 是控制RPC调用的核心对象。

### **服务端使用**

```cpp
Controller* cntl = (Controller*)controller;

// 获取请求
cntl->http_request().method();           // HTTP方法
cntl->request_attachment().to_string();   // 请求体

// 设置响应
cntl->response_attachment().append(内容);
cntl->http_response().set_status_code(状态码);
```

**注意事项：**

- **类型转换是安全的**：在brpc服务端，传入的 `RpcController*` 实际上就是 `brpc::Controller*`，可以安全转换。这是brpc框架的保证
- **request_attachment是IOBuf**：`to_string()` 会复制数据，如果请求体很大，考虑使用流式读取避免内存拷贝
- **response_attachment.append()**：可以多次调用append追加内容，最终会按顺序拼接成完整响应体
- **状态码必须设置**：HTTP响应必须设置状态码，否则客户端可能收到默认值（通常是500），即使数据正确也可能被当作错误处理
- **Controller在函数返回后失效**：不要保存Controller的指针或引用，函数返回后Controller会被框架回收

### **客户端使用**

```cpp
Controller cntl;

// 配置请求
cntl.http_request().set_method(方法);
cntl.http_request().uri().set_path(路径);
cntl.request_attachment().append(请求体);

// 调用后检查
cntl.Failed()        // 是否失败
cntl.response_attachment()  // 响应体
cntl.ErrorText()     // 错误信息
```

**注意事项：**

- **Controller生命周期**：Controller对象必须在调用完成后才能访问结果，在 `CallMethod` 返回前访问 `response_attachment()` 是未定义行为
- **路径设置时机**：所有请求配置（method、path、header、body）都必须在 `CallMethod` 之前完成，调用后修改无效
- **Failed()检查是必须的**：网络超时、连接失败、服务器错误等都会导致 `Failed() == true`，必须先检查再使用响应数据
- **ErrorText()包含详细信息**：失败时 `ErrorText()` 会返回具体的错误原因，如连接超时、DNS解析失败等，有助于调试
- **response_attachment是IOBuf**：需要使用 `to_string()` 等方法获取内容，或者使用 `copy_to()` 复制到其他缓冲区

### **常用接口**

- `http_request()` - HTTP请求对象
- `http_response()` - HTTP响应对象
- `request_attachment()` - 请求体（IOBuf类型）
- `response_attachment()` - 响应体（IOBuf类型）
- `Failed()` - 检查是否失败
- `ErrorText()` - 错误信息
- `ErrorCode()` - 错误码

## **brpc二次封装**


## **注意事项**

1. **ClosureGuard**: 服务端必须使用 `ClosureGuard done_guard(done)` 管理回调
2. **HTTP路径格式**: `/ServiceName/MethodName`，需与服务端对应
3. **协议选择**:

   - `PROTOCOL_HTTP` - HTTP协议
   - `PROTOCOL_BAIDU_STD` - 标准RPC协议
4. **服务器选项**: `idle_timeout_sec = -1` 表示连接不超时
