#pragma once
#include <string>
#include <cstddef> // std::size_t
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/nullable.hxx>  // ODB 可空类型支持
#include <odb/core.hxx>      // ODB 核心功能
#include <optional>

namespace vp_data{

// 用户身份类型枚举
enum class IdentifyType {
    UNKNOWN = 0,  // 未知身份
    NORMAL = 1,   // 普通用户
    ADMIN = 2     // 管理员
};

// 用户角色类型枚举：超级管理员，普通管理员，登录用户，游客
enum class RoleType {
    UNKNOWN = 0,
    ADMIN_SUPER = 1,   // 超级管理员
    ADMIN_NORMAL = 2,  // 普通管理员
    USER_NORNAL = 3,   // 登录用户
    USER_GUEST = 4     // 游客（未登录用户）
};

// 用户状态枚举
enum class UserStatus {
    UNKNOWN,      // 未知状态
    ENABLE = 1,   // 启用
    DISABLE = 2   // 禁用
};

// 视频状态枚举：未知，审核中，审核通过上架中，审核驳回，下架中，转码中，转码失败
enum class VideoStatus {
    UNKNOWN = 0,          // 未知
    AUDITING = 1,         // 审核中
    ONLINE = 2,           // 审核通过上架中
    REJECT = 3,           // 审核驳回
    OFFLINE = 4,          // 下架中
    TRANSCODING = 5,      // 转码中
    TRANSCODE_FAILED = 6  // 转码失败
};

// ====================== 会话（Session）表映射类 ======================
// ODB 指令说明：
// - #pragma db object table("表名"): 标记这个类映射到数据库表 tbl_session_meta
// - friend class odb::access: 允许 ODB 访问私有成员
// - #pragma db id auto: 标记 _id 为主键且自增
// - #pragma db unique: 标记字段为唯一约束（不允许重复）
// - #pragma db index: 创建索引（加速查询）
// - odb::nullable<T>: 表示字段可以为 NULL
#pragma db object table("tbl_session_meta")
class Session {
    public:
        using ptr = std::shared_ptr<Session>;
        Session();
        Session(const std::string &session_id);
        Session(const std::string &session_id, const std::string &user_id);
        size_t id() const;
        void set_id(size_t id);
        const std::string &session_id() const;
        void set_session_id(const std::string &session_id);
        odb::nullable<std::string> user_id() const;
        void set_user_id(const odb::nullable<std::string> &user_id);
    private:
        friend class odb::access;  // 让 ODB 能访问私有成员
        #pragma db id auto         // 主键，自增
        size_t _id;
        #pragma db unique type("VARCHAR(64)")  // 唯一索引，会话ID不能重复
        std::string _session_id;
        #pragma db index type("VARCHAR(64)")   // 普通索引，用于快速查询用户的会话
        odb::nullable<std::string> _user_id;   // 可为 NULL，未登录时为空
};

// ====================== Session 视图结构 ======================
// ODB View 说明：
// - #pragma db view: 标记为视图（用于查询结果映射）
// - object(Session): 基于 Session 表
// - query((??)): 支持动态查询条件，?? 会被替换为实际查询条件
#pragma db view object(Session) \
                query((?))
struct SessionPtr {
    using ptr = std::shared_ptr<SessionPtr>;
    std::shared_ptr<Session> session;  // 查询结果存储在这里
};

// ====================== 文件（File）表映射类 ======================
// 存储文件元数据信息：文件ID、上传者、存储路径、大小、MIME类型
#pragma db object table("tbl_file_meta")
class File {
    public:
        using ptr = std::shared_ptr<File>;
        File();
        File(const std::string &file_id, const std::string &uploader_uid, const std::string &path, size_t size, const std::string &mime);
        size_t id() const;
        void set_id(size_t id);
        const std::string &file_id() const;
        void set_file_id(const std::string &file_id);
        const std::string &uploader_uid() const;
        void set_uploader_uid(const std::string &uploader_uid);
        const std::string &path() const;
        void set_path(const std::string &path);
        size_t size() const;
        void set_size(size_t size);
        const std::string &mime() const;
        void set_mime(const std::string &mime);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                // 主键，自增
        #pragma db unique type("VARCHAR(64)")
        std::string _file_id;      // 文件ID（唯一）
        #pragma db index type("VARCHAR(64)")
        std::string _uploader_uid; // 上传用户ID（索引，用于查询某用户的所有文件）
        #pragma db type("VARCHAR(64)")
        std::string _path;         // 存储路径（FastDFS 路径）
        size_t _size;              // 文件大小（字节）
        #pragma db type("VARCHAR(32)")
        std::string _mime;         // 文件MIME类型（如 image/jpeg, video/mp4）
};


// ====================== 身份（Identify）表映射类 ======================
// 存储身份类型信息（普通用户、管理员等）
#pragma db object table("tbl_identify_meta")
class Identify {
    public:
        using ptr = std::shared_ptr<Identify>;
        Identify();
        Identify(const std::string &identify_id, IdentifyType type, const std::string &name);
        size_t id() const;
        const std::string &identify_id() const;
        IdentifyType type() const; 
        const std::string &name() const;

        void set_id(size_t id);
        void set_identify_id(const std::string &identify_id);
        void set_type(IdentifyType type);
        void set_name(const std::string &name);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db unique type("VARCHAR(64)")
        std::string _identify_id;     // 身份ID（唯一）
        #pragma db unique type("TINYINT")
        IdentifyType _type;           // 身份类型（唯一，NORMAL/ADMIN）
        #pragma db type("VARCHAR(64)")
        std::string _name;            // 身份名称（如"普通用户"、"管理员"）
};


// ====================== 角色（Role）表映射类 ======================
// 存储角色信息（超级管理员、普通管理员、登录用户、游客）
#pragma db object table("tbl_role_meta")
class Role {
    public:
        using ptr = std::shared_ptr<Role>;
        Role();
        Role(const std::string &role_id, RoleType type, const std::string &name);
        size_t id() const;
        const std::string &role_id() const;
        RoleType type() const;
        const std::string &name() const;

        void set_id(size_t id);
        void set_role_id(const std::string &role_id);
        void set_type(RoleType type);
        void set_name(const std::string &name);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db unique type("VARCHAR(64)")
        std::string _role_id;         // 角色ID（唯一）
        #pragma db unique type("TINYINT")
        RoleType _type;               // 角色类型（唯一）
        #pragma db type("VARCHAR(64)")
        std::string _name;            // 角色名称（如"超级管理员"）
};


// ====================== 操作（Operator）表映射类 ======================
// 存储系统操作信息（API 接口），用于权限控制
#pragma db object table("tbl_operator_meta")
class Operator {
    public:
        using ptr = std::shared_ptr<Operator>;
        Operator();
        Operator(const std::string &operator_id, const std::string &op_url, const std::string &name);
        size_t id() const;
        const std::string &operator_id() const;
        const std::string &op_url() const;
        const std::string &name() const;

        void set_id(size_t id);
        void set_operator_id(const std::string &operator_id);
        void set_op_url(const std::string &op_url);
        void set_name(const std::string &name);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db unique type("VARCHAR(64)")
        std::string _operator_id;     // 操作ID（唯一）
        #pragma db unique type("VARCHAR(64)")
        std::string _op_url;          // 操作URL（唯一，如 /api/user/delete）
        #pragma db type("VARCHAR(64)")
        std::string _name;            // 操作名称（如"删除用户"）
};


// ====================== 用户（User）表映射类 ======================
// 存储用户核心信息：ID、邮箱、昵称、密码、手机号、状态、头像、备注等
#pragma db object table("tbl_user_meta")
class User {
    public:
        using ptr = std::shared_ptr<User>;
        User();
        User(const std::string &user_id, const std::string &email);
        size_t id() const;
        const std::string &user_id() const;
        const std::string &email() const;
        const std::string &nickname() const;
        UserStatus status() const;
        const odb::nullable<std::string> &bakname() const;
        const odb::nullable<std::string> &phone() const;
        const odb::nullable<std::string> &password() const;
        const odb::nullable<std::string> &avatar() const;
        const odb::nullable<std::string> &remark() const;
        const boost::posix_time::ptime &reg_time() const;

        void set_id(size_t id);
        void set_user_id(const std::string &user_id);
        void set_email(const std::string &email);
        void set_nickname(const std::string &nickname);
        void set_status(UserStatus status);
        void set_bakname(const odb::nullable<std::string> &bakname);
        void set_phone(const odb::nullable<std::string> &phone);
        void set_password(const odb::nullable<std::string> &password);
        void set_avatar(const odb::nullable<std::string> &avatar);
        void set_remark(const odb::nullable<std::string> &remark);
        void set_reg_time(const boost::posix_time::ptime &reg_time);
    private:    
        friend class odb::access;
        #pragma db id auto
        size_t _id;                // 主键，自增
        // 用户ID，手机号，邮箱，密码，昵称，备用昵称，状态，头像文件ID，备注信息，注册时间
        #pragma db unique type("VARCHAR(64)")
        std::string _user_id;      // 用户ID（唯一）
        #pragma db unique type("VARCHAR(64)")
        std::string _email;        // 邮箱（唯一，用于登录）
        #pragma db unique type("VARCHAR(64)")
        std::string _nickname;     // 昵称（唯一）
        #pragma db unique type("VARCHAR(64)")
        odb::nullable<std::string> _bakname;  // 备用昵称（唯一，可为空）
        #pragma db unique type("VARCHAR(64)")
        odb::nullable<std::string> _phone;    // 手机号（唯一，可为空）
        #pragma db type("VARCHAR(64)") 
        odb::nullable<std::string> _password; // 密码（加密后，可为空-第三方登录）
        #pragma db index type("TINYINT")
        UserStatus _status;        // 用户状态（启用/禁用，有索引便于查询）
        #pragma db type("VARCHAR(64)")
        odb::nullable<std::string> _avatar;   // 头像文件ID（可为空）
        #pragma db type("VARCHAR(64)")
        odb::nullable<std::string> _remark;   // 备注信息（可为空）
        #pragma db type("DATETIME")
        boost::posix_time::ptime _reg_time;   // 注册时间
};





// ====================== 用户身份角色关系表映射类 ======================
// 多对多关系：一个用户可以有多个身份角色组合
// 用于权限控制，例如：用户A 是 普通身份+普通用户角色，用户B 是 管理员身份+超级管理员角色
#pragma db object table("tbl_user_identify_role_meta")
class UserIdentifyRole {
    public:
        using ptr = std::shared_ptr<UserIdentifyRole>;
        UserIdentifyRole();
        UserIdentifyRole(const std::string &user_id, IdentifyType identify_type, RoleType role_type);
        size_t id() const;
        const std::string &user_id() const;
        IdentifyType identify_type() const;
        RoleType role_type() const;
        void set_id(size_t id);
        void set_user_id(const std::string &user_id);
        void set_identify_type(IdentifyType identify_type);
        void set_role_type(RoleType role_type);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        // 用户ID， 身份类型，角色类型
        #pragma db index type("VARCHAR(64)")
        std::string _user_id;         // 用户ID（索引，用于查询用户的所有角色）
        #pragma db index type("TINYINT")
        IdentifyType _identify_type;  // 身份类型（索引）
        #pragma db index type("TINYINT")
        RoleType _role_type;          // 角色类型（索引）
};


// ====================== 操作角色关系表映射类 ======================
// 多对多关系：定义哪些角色可以执行哪些操作（权限控制）
// 例如：超级管理员可以执行"删除用户"操作，普通用户不能
#pragma db object table("tbl_operator_role_meta")
class OperatorRole {
    public:
        using ptr = std::shared_ptr<OperatorRole>;
        OperatorRole();
        OperatorRole(const std::string &operator_url, RoleType role_type);
        size_t id() const;
        const std::string &operator_url() const;
        RoleType role_type() const;
        void set_id(size_t id);
        void set_operator_url(const std::string &operator_url);
        void set_role_type(RoleType role_type);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db index type("VARCHAR(64)")
        std::string _operator_url;    // 操作URL（索引，用于快速查询该操作需要什么角色）
        #pragma db index type("TINYINT")
        RoleType _role_type;          // 角色类型（索引）
};


// ====================== 用户关注关系表映射类 ======================
// 多对多关系：记录用户之间的关注关系（类似微博的关注功能）
// follower 关注了 followed
#pragma db object table("tbl_follow_meta")
class Follow {
    public:
        using ptr = std::shared_ptr<Follow>;
        Follow();
        Follow(const std::string &follower_uid, const std::string &followed_uid);
        size_t id() const;
        const std::string &follower_uid() const;
        const std::string &followed_uid() const;

        void set_id(size_t id);
        void set_follower_uid(const std::string &follower_uid);
        void set_followed_uid(const std::string &followed_uid);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db index type("VARCHAR(64)")
        std::string _follower_uid;    // 关注者ID（索引，查询某人关注了谁）
        #pragma db index type("VARCHAR(64)")
        std::string _followed_uid;    // 被关注者ID（索引，查询某人被谁关注）
};





// ====================== 视频（Video）表映射类 ======================
// 存储视频核心信息：ID、标题、简介、封面、视频文件、上传者、审核者、播放量、时长、大小、状态、上传时间
#pragma db object table("tbl_video_meta")
class Video {
    public:
        using ptr = std::shared_ptr<Video>;
        Video();
        Video(const std::string &video_id, const std::string &title, 
            const std::string &summary, 
            const std::string &cover_fid, 
            const std::string &video_fid, 
            const std::string &uploader_uid, 
            size_t duration, size_t size);
            size_t id() const;
            const std::string &video_id() const;
            const std::string &title() const;
            const odb::nullable<std::string> &summary() const;
            const std::string &cover_fid() const;
            const std::string &video_fid() const;
            const std::string &uploader_uid() const;
            const odb::nullable<std::string> &auditor_uid() const;
            size_t play_count() const;
            size_t duration() const;
            size_t size() const;
            VideoStatus status() const;
            const boost::posix_time::ptime &upload_time() const;

            void set_id(size_t id);
            void set_video_id(const std::string &video_id);
            void set_title(const std::string &title);
            void set_summary(const odb::nullable<std::string> &summary);
            void set_cover_fid(const std::string &cover_fid);
            void set_video_fid(const std::string &video_fid);
            void set_uploader_uid(const std::string &uploader_uid);
            void set_auditor_uid(const odb::nullable<std::string> &auditor_uid);
            void set_play_count(size_t play_count);
            void set_duration(size_t duration);
            void set_size(size_t size);
            void set_status(VideoStatus status);
            void set_upload_time(const boost::posix_time::ptime &upload_time);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                // 主键，自增
        //视频ID，视频标题，视频简介，视频封面文件ID，视频数据文件ID，视频上传用户ID， 审核员用户ID，播放量，视频时长，视频大小，视频状态，上传时间
        #pragma db unique type("VARCHAR(64)")
        std::string _video_id;     // 视频ID（唯一）
        #pragma db type("VARCHAR(64)")
        std::string _cover_fid;    // 封面文件ID（关联到 File 表）
        #pragma db type("VARCHAR(64)")
        std::string _video_fid;    // 视频文件ID（关联到 File 表）
        #pragma db index type("VARCHAR(64)")
        std::string _uploader_uid; // 上传用户ID（索引，查询某用户的所有视频）
        #pragma db type("TEXT")
        std::string _title;        // 视频标题（TEXT 类型支持长文本）
        #pragma db type("TEXT")
        odb::nullable<std::string> _summary;      // 视频简介（可为空）
        #pragma db type("VARCHAR(64)")
        odb::nullable<std::string> _auditor_uid;  // 审核员用户ID（可为空）
        #pragma db type("INT")
        size_t _play_count;        // 播放量
        #pragma db type("INT")
        size_t _duration;          // 视频时长（秒）
        #pragma db type("INT")
        size_t _size;              // 视频大小（字节）
        #pragma db index type("TINYINT")
        VideoStatus _status;       // 视频状态（索引，用于查询待审核、已上架等状态的视频）
        #pragma db type("DATETIME")
        boost::posix_time::ptime _upload_time;   // 上传时间
};




// ====================== 视频点赞关系表映射类 ======================
// 多对多关系：记录用户对视频的点赞关系
#pragma db object table("tbl_video_like_meta")
class VideoLike {
    public:
        using ptr = std::shared_ptr<VideoLike>;
        VideoLike();
        VideoLike(const std::string &user_id, const std::string &video_id);
        size_t id() const;
        const std::string &user_id() const;
        const std::string &video_id() const;
        
        void set_id(size_t id);
        void set_user_id(const std::string &user_id);
        void set_video_id(const std::string &video_id);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db index type("VARCHAR(64)")
        std::string _user_id;         // 用户ID（索引，查询某用户点赞了哪些视频）
        #pragma db index type("VARCHAR(64)")
        std::string _video_id;        // 视频ID（索引，查询某视频被哪些用户点赞）
};

// ====================== 关注数统计视图 ======================
// 查询用户关注了多少人（我关注的人数）
// 使用示例: query = Follow::follower_uid == 'user123'
// 生成SQL: SELECT count(*) FROM tbl_follow_meta WHERE follower_uid='user123'
#pragma db view object(Follow) \
                query((?))
struct FollowedCount {
    using ptr = std::shared_ptr<FollowedCount>;
    #pragma db column("count(*)")    // SQL 聚合函数：count(*)
    size_t count;                     // 查询结果：关注数量
};

// ====================== 粉丝数统计视图 ======================
// 查询有多少人关注了该用户（我的粉丝数）
// 使用示例: query = Follow::followed_uid == 'user123'
// 生成SQL: SELECT count(*) FROM tbl_follow_meta WHERE followed_uid='user123'
#pragma db view object(Follow) \
                query((?))
struct FollowerCount {
    using ptr = std::shared_ptr<FollowerCount>;
    #pragma db column("count(*)")    // SQL 聚合函数：count(*)
    size_t count;                     // 查询结果：粉丝数量
};

// ====================== 用户视频播放总量统计视图 ======================
// 统计某用户上传的所有视频的总播放量
// 使用示例: query = Video::uploader_uid == 'user123'
// 生成SQL: SELECT sum(play_count) FROM tbl_video_meta WHERE uploader_uid='user123'
#pragma db view object(Video) \
                query((?))
struct UserVPlayCount {
    using ptr = std::shared_ptr<UserVPlayCount>;
    #pragma db column("sum(" + Video::_play_count + ")")  // SQL 聚合函数：sum(play_count)
    size_t count;                                          // 查询结果：总播放量
};

// ====================== 用户视频点赞总量统计视图 ======================
// 统计某用户上传的所有视频获得的总点赞数（需要联表查询）
// 使用示例: query = Video::uploader_uid == 'user123'
// 生成SQL: SELECT count(*) FROM tbl_video_like_meta 
//          JOIN tbl_video_meta ON tbl_video_like_meta.video_id = tbl_video_meta.video_id 
//          WHERE tbl_video_meta.uploader_uid='user123'
#pragma db view object(VideoLike) \
                object(Video : VideoLike::_video_id == Video::_video_id) \
                query((?))
struct UserVLikeCount {
    using ptr = std::shared_ptr<UserVLikeCount>;
    #pragma db column("count(*)")    // SQL 聚合函数：count(*)
    size_t count;                     // 查询结果：总点赞数
};

// ====================== 用户统计信息聚合结构 ======================
// 聚合用户的各种统计信息（非 ODB 映射，纯 C++ 结构体）
struct UserCount {
    using ptr = std::shared_ptr<UserCount>;
    size_t followed_count = 0;  // 关注数（我关注了多少人）
    size_t follower_count = 0;  // 粉丝数（多少人关注我）
    size_t vplay_count = 0;     // 视频播放总量
    size_t vlike_count = 0;     // 视频点赞总量
};



// ====================== User 查询视图 ======================
// 简单的 User 对象查询视图，支持动态查询条件
#pragma db view object(User) \
                query((?))
struct UserPtr {
    using ptr = std::shared_ptr<UserPtr>;
    std::shared_ptr<User> user;  // 查询结果
};

// ====================== 用户详细信息聚合结构 ======================
// 聚合用户的完整信息（非 ODB 映射，纯 C++ 结构体）
struct DetailUser {
    using ptr = std::shared_ptr<DetailUser>;
    User::ptr user;                          // 用户基本信息
    UserCount::ptr count;                    // 用户统计信息（关注数、粉丝数等）
    std::vector<UserIdentifyRole> uirs;      // 用户的身份角色列表
    std::optional<bool> isFollowing;         // 当前用户是否关注了该用户（可选）
};

// ====================== 邮箱+身份查询用户视图 ======================
// 根据邮箱和身份类型查询用户（用于登录验证）
// 使用示例: query = User::email == "xxx@example.com" && UserIdentifyRole::identify_type == IdentifyType::NORMAL
// 生成SQL: SELECT * FROM tbl_user_meta 
//          JOIN tbl_user_identify_role_meta ON tbl_user_meta.user_id = tbl_user_identify_role_meta.user_id
//          WHERE tbl_user_meta.email='xxx@example.com' AND tbl_user_identify_role_meta.identify_type=1
#pragma db view object(User) \
                object(UserIdentifyRole : User::_user_id == UserIdentifyRole::_user_id) \
                query((?))
struct EmailIdendifyUser {
    using ptr = std::shared_ptr<EmailIdendifyUser>;
    std::shared_ptr<User> user;  // 查询结果
};


// ====================== 分页宏定义 ======================
// ODB 不直接支持分页，需要手动拼接 SQL
// 升序分页：ORDER BY order_field ASC LIMIT size OFFSET page*size
#define ASC_LIMIT_PAGE(page, size) \
    "ORDER BY order_field ASC LIMIT " + std::to_string(size) + " OFFSET " + std::to_string((page) * size)
// 降序分页：ORDER BY order_field DESC LIMIT size OFFSET page*size
#define DESC_LIMIT_PAGE(page, size) \
    "ORDER BY order_field DESC LIMIT " + std::to_string(size) + " OFFSET " + std::to_string((page) * size)

// ====================== 状态+身份查询用户视图 ======================
// 根据用户状态和身份类型查询用户列表（用于管理后台）
// 使用示例: query = User::status == UserStatus::ENABLE && UserIdentifyRole::identify_type == IdentifyType::NORMAL
// 生成SQL: SELECT * FROM tbl_user_meta 
//          JOIN tbl_user_identify_role_meta ON tbl_user_meta.user_id = tbl_user_identify_role_meta.user_id
//          WHERE tbl_user_meta.status=1 AND tbl_user_identify_role_meta.identify_type=1
#pragma db view object(User) \
                object(UserIdentifyRole : User::_user_id == UserIdentifyRole::_user_id) \
                query((?))
struct StatusIdentifyUser {
    using ptr = std::shared_ptr<StatusIdentifyUser>;
    std::shared_ptr<User> user;  // 用户信息
    #pragma db column(UserIdentifyRole::_role_type)
    RoleType role_type;           // 角色类型（额外提取）
    #pragma db column(User::_id + " AS order_field")
    size_t order_field;           // 排序字段（用于分页排序）
};

// ====================== 状态+身份用户数量统计视图 ======================
// 统计符合条件的用户数量（用于分页计算总页数）
#pragma db view object(User) \
                object(UserIdentifyRole : User::_user_id == UserIdentifyRole::_user_id) \
                query((?))
struct StatusIdentifyUserCount {
    using ptr = std::shared_ptr<StatusIdentifyUserCount>;
    #pragma db column("count(*)")
    size_t count;  // 符合条件的用户总数
};

// ====================== 状态+身份用户列表聚合结构 ======================
// 分页查询结果的聚合结构（非 ODB 映射）
struct StatusIdentifyUserList {
    using ptr = std::shared_ptr<StatusIdentifyUserList>;
    std::vector<StatusIdentifyUser> users;  // 当前页的用户列表
    size_t total = 0;                        // 总用户数
};


// ====================== 验证码结构 ======================
// 验证码信息（非 ODB 映射，存储在 Redis 中）
struct VerifyCode {
    using ptr = std::shared_ptr<VerifyCode>;
    std::string session_id;  // 会话ID
    std::string code_id;     // 验证码ID
    std::string code;        // 验证码内容
    std::string email;       // 接收验证码的邮箱
};


// ====================== 视频分类标签关系表映射类 ======================
// 多对多关系：一个视频可以有多个分类和标签
// 例如：视频A 属于 "娱乐"分类，有"搞笑"、"音乐"两个标签
#pragma db object table("tbl_video_catgory_tag_meta")
class VideoCatgoryTag {
    public:
        using ptr = std::shared_ptr<VideoCatgoryTag>;
        VideoCatgoryTag();
        VideoCatgoryTag(const std::string &video_id, unsigned int catgory_id, unsigned int tag_id);
        size_t id() const;
        const std::string &video_id() const;
        unsigned int catgory_id() const;
        unsigned int tag_id() const;
        void set_id(size_t id);
        void set_video_id(const std::string &video_id);
        void set_catgory_id(unsigned int catgory_id);
        void set_tag_id(unsigned int tag_id);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db index type("VARCHAR(64)")
        std::string _video_id;        // 视频ID（索引，查询某视频的分类标签）
        #pragma db index type("INT")
        unsigned int _catgory_id;     // 分类ID（索引，查询某分类下的视频）
        #pragma db index type("INT")
        unsigned int _tag_id;         // 标签ID（索引，查询某标签下的视频）
};

// ====================== 弹幕信息表映射类 ======================
// 存储视频弹幕信息：弹幕ID、视频ID、用户ID、弹幕内容、弹幕时间点（秒）
#pragma db object table("tbl_barrage_meta")
class Barrage {
    public:
        using ptr = std::shared_ptr<Barrage>;
        Barrage();
        Barrage(const std::string &barrage_id, const std::string &video_id, const std::string &user_id, const std::string &content, size_t time);
        size_t id() const;
        const std::string &barrage_id() const;
        const std::string &video_id() const;
        const std::string &user_id() const;
        const std::string &content() const;
        size_t time() const;
        void set_id(size_t id);
        void set_barrage_id(const std::string &barrage_id);
        void set_video_id(const std::string &video_id);
        void set_user_id(const std::string &user_id);
        void set_content(const std::string &content);
        void set_time(size_t time);
    private:
        friend class odb::access;
        #pragma db id auto
        size_t _id;                   // 主键，自增
        #pragma db unique type("VARCHAR(64)")
        std::string _barrage_id;      // 弹幕ID（唯一）
        #pragma db index type("VARCHAR(64)")
        std::string _video_id;        // 视频ID（索引，查询某视频的所有弹幕）
        #pragma db type("VARCHAR(64)")
        std::string _user_id;         // 用户ID（发送弹幕的用户）
        #pragma db type("VARCHAR(64)")
        std::string _content;         // 弹幕内容
        #pragma db type("INT")
        size_t _time;                 // 弹幕时间点（视频的第几秒，用于同步显示）
};

// ====================== Video 查询视图 ======================
// 简单的 Video 对象查询视图，支持动态查询条件
#pragma db view object(Video) \
                query((?))
struct VideoPtr {
    using ptr = std::shared_ptr<VideoPtr>;
    std::shared_ptr<Video> video;  // 查询结果
};

// ====================== Barrage 查询视图（按时间排序） ======================
// 查询弹幕并按时间排序（用于播放视频时按时间顺序显示弹幕）
// 查询条件后会自动添加 ORDER BY time
#pragma db view object(Barrage) \
                query((?) + "ORDER BY " + Barrage::_time)
struct BarragePtr {
    using ptr = std::shared_ptr<BarragePtr>;
    std::shared_ptr<Barrage> barrage;  // 查询结果
};

// ====================== 视频点赞总量统计视图 ======================
// 统计某个视频的点赞数
// 使用示例: query = VideoLike::video_id == 'video123'
// 生成SQL: SELECT count(*) FROM tbl_video_like_meta WHERE video_id='video123'
#pragma db view object(VideoLike) \
                query((?))
struct VideoLikeCount {
    using ptr = std::shared_ptr<VideoLikeCount>;
    #pragma db column("count(*)")
    size_t count;  // 点赞数
};

// ====================== 视频播放量统计视图 ======================
// 获取某个视频的播放量
// 使用示例: query = Video::video_id == 'video123'
// 生成SQL: SELECT play_count FROM tbl_video_meta WHERE video_id='video123'
#pragma db view object(Video) \
                query((?))
struct VideoPlayCount {
    using ptr = std::shared_ptr<VideoPlayCount>;
    #pragma db column(Video::_play_count)
    size_t count;  // 播放量
};

// ====================== 视频统计信息聚合结构 ======================
// 聚合视频的统计信息（非 ODB 映射）
struct VideoCount {
    using ptr = std::shared_ptr<VideoCount>;
    size_t like_count = 0;  // 点赞数
    size_t play_count = 0;  // 播放量
};

// ====================== 主页视频ID列表视图（按时间排序） ======================
// 获取主页视频列表，按上传时间降序排序，分页获取（用于首页视频推荐）
// 使用示例: query = Video::status == VideoStatus::ONLINE + DESC_LIMIT_PAGE(0, 20)
// 生成SQL: SELECT video_id, upload_time AS order_field FROM tbl_video_meta 
//          WHERE status=2 ORDER BY order_field DESC LIMIT 20 OFFSET 0
#pragma db view object(Video) \
                query((?))
struct VideoMainIdList {
    using ptr = std::shared_ptr<VideoMainIdList>;
    #pragma db column(Video::_video_id)
    std::string video_id;                        // 视频ID
    #pragma db column(Video::_upload_time + " AS order_field")
    boost::posix_time::ptime order_field;        // 排序字段（上传时间）
};

// ====================== 用户视频ID列表视图（按播放量排序） ======================
// 获取某用户的视频列表，按播放量降序排序（用于个人主页）
// 使用示例: query = Video::uploader_uid == 'user123' + DESC_LIMIT_PAGE(0, 20)
// 生成SQL: SELECT video_id, play_count AS order_field FROM tbl_video_meta 
//          WHERE uploader_uid='user123' ORDER BY order_field DESC LIMIT 20 OFFSET 0
#pragma db view object(Video) \
                query((?))
struct VideoUserPlayIdList {
    using ptr = std::shared_ptr<VideoUserPlayIdList>;
    #pragma db column(Video::_video_id)
    std::string video_id;                        // 视频ID
    #pragma db column(Video::_play_count + " AS order_field")
    size_t order_field;                          // 排序字段（播放量）
};

// ====================== 用户指定状态视频数量统计视图 ======================
// 统计某用户指定状态的视频数量（用于分页计算）
// 使用示例: query = Video::uploader_uid == 'user123' && Video::status == VideoStatus::ONLINE
#pragma db view object(Video) \
                query((?))
struct VideoUserPlayIdListCount {
    using ptr = std::shared_ptr<VideoUserPlayIdListCount>;
    #pragma db column("count(*)")
    size_t count;  // 视频数量
};

// ====================== 用户视频列表聚合结构 ======================
// 用户视频列表的聚合结构（非 ODB 映射）
struct VideoUserStatusIdList {
    using ptr = std::shared_ptr<VideoUserStatusIdList>;
    std::vector<std::string> videos;  // 视频ID列表
    size_t total = 0;                  // 总视频数
};


// ====================== 分类视频ID列表视图（按播放量排序） ======================
// 获取某分类下的视频列表，按播放量降序排序（需要联表查询）
// 使用示例: query = VideoCatgoryTag::catgory_id == 1 + DESC_LIMIT_PAGE(0, 20)
// 生成SQL: SELECT DISTINCT tbl_video_meta.video_id, play_count AS order_field 
//          FROM tbl_video_catgory_tag_meta 
//          JOIN tbl_video_meta ON tbl_video_meta.video_id = tbl_video_catgory_tag_meta.video_id
//          WHERE catgory_id=1 ORDER BY order_field DESC LIMIT 20 OFFSET 0
#pragma db view object(VideoCatgoryTag) \
                object(Video : Video::_video_id == VideoCatgoryTag::_video_id) \
                query((?))
struct VideoCatgoryPlayIdList {
    using ptr = std::shared_ptr<VideoCatgoryPlayIdList>;
    #pragma db column("DISTINCT " + Video::_video_id)
    std::string video_id;                        // 视频ID（DISTINCT 去重）
    #pragma db column(Video::_play_count + " AS order_field")
    size_t order_field;                          // 排序字段（播放量）
};

// ====================== 标签视频ID列表视图（按播放量排序） ======================
// 获取某标签下的视频列表，按播放量降序排序（需要联表查询）
// 使用示例: query = VideoCatgoryTag::tag_id == 1 + DESC_LIMIT_PAGE(0, 20)
// 生成SQL: SELECT DISTINCT tbl_video_meta.video_id, play_count AS order_field 
//          FROM tbl_video_catgory_tag_meta 
//          JOIN tbl_video_meta ON tbl_video_meta.video_id = tbl_video_catgory_tag_meta.video_id
//          WHERE tag_id=1 ORDER BY order_field DESC LIMIT 20 OFFSET 0
#pragma db view object(VideoCatgoryTag) \
                object(Video : Video::_video_id == VideoCatgoryTag::_video_id) \
                query((?))
struct VideoTagPlayIdList {
    using ptr = std::shared_ptr<VideoTagPlayIdList>;
    #pragma db column("DISTINCT " + Video::_video_id)
    std::string video_id;                        // 视频ID（DISTINCT 去重）
    #pragma db column(Video::_play_count + " AS order_field")
    size_t order_field;                          // 排序字段（播放量）
};

// ====================== 指定状态视频列表视图（按上传时间排序） ======================
// 获取指定状态的视频列表，按上传时间排序（用于管理后台审核）
// 使用示例: query = Video::status == VideoStatus::AUDITING + DESC_LIMIT_PAGE(0, 20)
// 生成SQL: SELECT * FROM tbl_video_meta 
//          WHERE status=1 ORDER BY order_field DESC LIMIT 20 OFFSET 0
#pragma db view object(Video) \
                query((?))
struct VideoStatusIdList {
    using ptr = std::shared_ptr<VideoStatusIdList>;
    std::shared_ptr<Video> video;                // 视频完整信息
    #pragma db column(Video::_upload_time + " AS order_field")
    boost::posix_time::ptime order_field;        // 排序字段（上传时间）
};

// ====================== 指定状态视频数量统计视图 ======================
// 统计指定状态的视频数量（用于分页计算）
// 使用示例: query = Video::status == VideoStatus::AUDITING
// 生成SQL: SELECT count(*) FROM tbl_video_meta WHERE status=1
#pragma db view object(Video) \
                query((?))
struct VideoStatusCount {
    using ptr = std::shared_ptr<VideoStatusCount>;
    #pragma db column("count(*)")
    size_t count = 0;  // 视频数量
};

// ====================== 指定状态视频列表聚合结构 ======================
// 指定状态视频列表的聚合结构（非 ODB 映射）
struct StatusVideoList {
    using ptr = std::shared_ptr<StatusVideoList>;
    std::vector<Video::ptr> videos;  // 视频列表
    size_t total = 0;                 // 总视频数
};


}  // namespace vp_data