# **ODB //C++ORM**

### **表的使用**

--javascripttypescriptshellbashsqljsonhtmlcssccppjavarubypythongorustmarkdown

```
#pragma db object
class Student{
    public:
        Student() {}
        Student(unsigned long sn, const std::string &name, unsigned short age, unsigned long cid):
            _sn(sn), _name(name), _age(age), _classes_id(cid){}
          
       //Get Or Set funtion   //设置获取函数
    private:
        friend class odb::access;
        #pragma db id auto            // 自增主键
        unsigned long _id;
        #pragma db unique             // 唯一学号
        unsigned long _sn;
        std::string _name;
        odb::nullable<unsigned short> _age;
        #pragma db index              // 班级外键上建索引
        unsigned long _classes_id;
};

#pragma db object
class Classes {
    public:
        Classes() {}
        Classes(const std::string &name) : _name(name){}
        size_t id() { return _id; }
        void name(const std::string &name) { _name = name; }
        std::string name() { return _name; }
    private:
        friend class odb::access;
        #pragma db id auto        // 自增主键
        unsigned long _id;
        std::string _name;
};
```

**odb编译器会生成Student表**

#### **注意**

* •
  #pragma db object 声明表类型
* •
  #pragma db unique 唯一键
* •
  #pragma db id auto 自增主键
* •
  #pragma db index 外键
* •
  odb::nullable<数据类型> 可以为空

### **视图**

--javascripttypescriptshellbashsqljsonhtmlcssccppjavarubypythongorustmarkdown

```
#pragma db view object(Student)\
                object(Classes = classes : Student::_classes_id == classes::_id)\
                query((?))
struct classes_student {
    #pragma db column(Student::_id)
    unsigned long id;
    #pragma db column(Student::_sn)
    unsigned long sn;
    #pragma db column(Student::_name)
    std::string name;
    #pragma db column(Student::_age)
    odb::nullable<unsigned short>age;
    #pragma db column(classes::_name)
    std::string classes_name;
};
```

#### **注意**

* •
  #pragma db view 声明视图类型
* •
  object(Student) 第一张表
* •
  object(Classes=classes : Student::classes*id==classes::*id) 给Classes取别名为classes，**：**后为join条件（classes_id相同），query((?))占位符，后续可加where查询条件
* •
  #pragma db column 映射表字段

### **查询**

--javascripttypescriptshellbashsqljsonhtmlcssccppjavarubypythongorustmarkdown

```
#pragma db view query("select name from Student" + (?))
struct all_name {
    std::string name;
```

#### **注意**

* •
  这个视图只查询了一列 name，SQL 里的列名与成员变量名 name 完全一致，ODB 会按名字自动匹配，不需要额外指定。
* •
  只有当列名与成员名不一致，或者多表查询需要消歧（如 Student._name、Classes._name）时，才需要 #pragma db column(...) 显式映射。

  ### **MySql客户端使用**

  --javascripttypescriptshellbashsqljsonhtmlcssccppjavarubypythongorustmarkdown


  ```
  class MysqlClient
  {
  public:
      // 创建带连接池的数据库句柄
      static std::shared_ptr<odb::database> create(
          const std::string& user,
          const std::string& passwd,
          const std::string& db_name,
          const std::string& host,
          int port,
          size_t conn_pool_count) {
  		// 创建连接池工厂，指定池大小 conn_pool_count
          std::unique_ptr<odb::mysql::connection_factory> pool(
              new odb::mysql::connection_pool_factory(conn_pool_count));
  		// 构造 MySQL 后端的数据库对象
          std::shared_ptr<odb::database> db(new odb::mysql::database(
              user.c_str(), passwd.c_str(), db_name.c_str(), host.c_str(), port, 0, "utf8", 0, std::move(pool)));
          return db;
      }
      // 开启事务
      static std::shared_ptr<odb::transaction> transaction(const std::shared_ptr<odb::database> &db) {
          return std::make_shared<odb::transaction>(db->begin());
      }
      // 提交事务
      static void commit(const std::shared_ptr<odb::transaction> &t) {
          return t->commit();
      }
      // 回滚事务
      static void rollback(const std::shared_ptr<odb::transaction> &t) {
          return t->rollback();
      }
  };
  ```

#### **注意**

* •
  odb::database是数据库句柄
* •
  odb::mysql::connectionpoolfactory*(*sizet（池大小）)创建连接池工厂，返回值为odb::mysql::connectionpool_factory类型指针
* •
  odb::mysql::database(char*, *char* ,*char.char,int,int,char*,int,odb::mysql::connectionpool_factory）参数依次为用户名、密码、数据库名、ip、端口、socket文件路径、客户端字符集、客户端标记位、连接池工厂，返回值为odb::mysql::database类型指针
* •
  odb::database->begin()在数据库连接上开一个新的事务，返回值为odb::transaction
* •
  odb::transaction->commit()提交事务，无返回值
* •
  odb::transaction->rollback()回滚事务，返回值
  ### **整表封装**

--javascripttypescriptshellbashsqljsonhtmlcssccppjavarubypythongorustmarkdown

```
// 学生表 DAO：封装增删改查
class StudentDao {
public:
    StudentDao(const std::string& user,
    const std::string& passwd,
    const std::string& db_name,
    const std::string& host,
    int port,
    size_t conn_pool_count):_db(MysqlClient::create(
        user, passwd, db_name, host, port, conn_pool_count))
    {}
    // 插入学生，返回自增 id
    size_t append(Student& stu) {
        try{
            auto t = MysqlClient::transaction(_db);
            auto &db = t->database();
            db.persist(stu);
            MysqlClient::commit(t);
        } catch(std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        return stu.id();
    }
    // 更新学生
    void update(const Student& stu) {
        try{
            auto t = MysqlClient::transaction(_db);
            auto &db = t->database();
            db.update(stu);
            MysqlClient::commit(t);
        } catch(std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
    // 按姓名删除学生
    void remove(const std::string &name) {
        try{
            auto t = MysqlClient::transaction(_db);
            auto &db = t->database();
            typedef odb::query<Student> query;
            db.erase_query<Student>(query::name == name);
            MysqlClient::commit(t);
        } catch(std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
    // 查询前 n 个年龄最大的学生姓名（用视图 all_name）
    std::vector<std::string> select(int n) {
        std::vector<std::string> res;
        try{
            auto t = MysqlClient::transaction(_db);
            auto &db = t->database();
            typedef odb::query<all_name> query;
            typedef odb::result<all_name> result;
            std::string cond = "order by age desc limit ";
            cond += std::to_string(n);
            result r(db.query<all_name>(cond));
            for (auto i(r.begin()); i != r.end(); ++i) {
                res.push_back(i->name);
            }
            MysqlClient::commit(t);
        } catch(std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        return res;
    }
private:
    std::shared_ptr<odb::database> _db;
};
```

#### **注意**

* •
  odb::database->persist(对象)，向数据库插入一条数据<=>insert
* •
  odb::database->update(对象)，更新一条数据
* •
  odb::query`<Student>`，查询构造器
* •
  odb::erase_query`<Student>`(odb::query`<Student>`::条件)，删除一条数据
* •
  进行curd时必须先获取事务，进行操作，提交事务
* •
  odb::result<all_name>,结果集类型
* •
  result r(odb::database->query<all_name>(构造的sql语句)),构造的sql语言填补之前预留的占位符

### **简单封装**

#### **odb.h**

--javascripttypescriptshellbashsqljsonhtmlcssccppjavarubypythongorustmarkdown

```
#pragma once
#include <odb/database.hxx>
#include <odb/mysql/transaction.hxx>
#include <odb/mysql/database.hxx>

namespace myodb {
    struct mysql_settings {
        std::string host;
        std::string user = "root";
        std::string passwd;
        std::string db;
        std::string cset = "utf8";
        unsigned int port = 3306;
        unsigned int connection_pool_size = 3;
    };

    class DBFactory {
        public:
            static std::shared_ptr<odb::database> mysql(const mysql_settings &settings);
    };
}
```

#### **odb.cc**

--javascripttypescriptshellbashsqljsonhtmlcssccppjavarubypythongorustmarkdown

```

#include "odb.h"

namespace myodb {
    std::shared_ptr<odb::database> DBFactory::mysql(const mysql_settings &settings) {
        // 1. 创建 MySQL 连接池工厂
        //    connection_pool_size 指定连接池中的最大连接数
        //    连接池会复用数据库连接，避免频繁创建和销毁连接的开销
        std::unique_ptr<odb::mysql::connection_factory> pool(
            new odb::mysql::connection_pool_factory(settings.connection_pool_size));
      
        // 2. 创建 MySQL 数据库操作句柄
        //    参数说明：
        //      - user: MySQL 用户名
        //      - passwd: MySQL 密码
        //      - db: 数据库名称
        //      - host: MySQL 服务器地址
        //      - port: MySQL 服务器端口
        //      - nullptr: Unix socket 路径（不使用则为 nullptr）
        //      - cset: 字符集（如 "utf8", "utf8mb4"）
        //      - 0: 客户端标志位（0 表示使用默认值）
        //      - std::move(pool): 移动连接池到数据库对象中
        auto handler = std::make_shared<odb::mysql::database>(
            settings.user,      // MySQL 用户名
            settings.passwd,    // MySQL 密码
            settings.db,        // 数据库名称
            settings.host,      // MySQL 服务器地址
            settings.port,      // MySQL 服务器端口
            nullptr,            // Unix socket（不使用）
            settings.cset,      // 字符集编码
            0,                  // 客户端标志（默认）
            std::move(pool)     // 连接池（移动语义，转移所有权）
        );
      
        // 3. 返回数据库操作句柄
        //    返回的句柄可用于创建事务、执行查询、持久化对象等操作
        return handler;
    }
}
```

* •
  使用时通过参数和工厂模式获取mysql的连接池句柄
* •
