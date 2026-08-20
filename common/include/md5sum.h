#ifndef MD5_SUM_H__
#define MD5_SUM_H__

#ifdef  __cplusplus
extern "C" {
#endif

//called by FileUtil::getFileMd5(filename);
int md5sum_file(const char* filename, char* buff_md5);

//called by StrUtil::getStrMd5(str);
int md5sum_str(const char* str, int str_len, char* buff_md5);

#ifdef  __cplusplus
}
#endif

#endif
