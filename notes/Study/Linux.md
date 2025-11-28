## 命令

**1.共享目录挂载**

sudo mount -t cifs //宿主机ip/共享目录 主机目录/ -o username=用户名,password=密码dir_mode=0777,file_mode=0777

**2.开机自启**

~/.bashrc
