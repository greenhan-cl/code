## 视频解析

1.VideoExtractor.cpp要用utf8+bom保存

## 探针编译流程

1.先编opensl的sh，需要ssl、cares、curl库

2.在编对应平台的compile(第三方库)

3.将平台工具链放到/dltool/对应平台目录下

4.将对应平台文件打包放大到平台的/backup/dlspeed_rt下

5.执行run查看是否正常
