ini文件解析器
=============
简单自用型

编译
------------
`make`


应用API
-----------
头文件：`ini.h`

```c
//加载ini文件
int ini_load_file(const char *filename, struct INI_FILE **ini);

//输出ini文件，移除了注释
int ini_dump_to_FILE_RemoveComment(struct INI_FILE *ini_f, FILE *fp);

//输出ini文件
int ini_dump_to_FILE(struct INI_FILE *ini_f, FILE *fp);

//输出ini文件到 stdout
int ini_dump(struct INI_FILE *ini_f);

//获取
char *ini_getString(struct INI_FILE *ini_f, const char *sect, const char *name);

// 1989 十进制，0124 八进制，0x23 十六进制
int ini_getInt(struct INI_FILE *ini_f, const char *sect, const char *name, int def);

//卸载ini
void ini_del_ini(struct INI_FILE *ini_f);

//创建空ini
struct INI_FILE *ini_create_ini_file(void);

//添加一个段
int ini_add_section_to_ini(struct INI_FILE *ini_f, const char *sectionName);

//添加要给名值对，段不存在会自动创建。若name已存在则替换其值
int ini_add_item_to_ini(struct INI_FILE *ini_f, 
		const char *sectionName, const char *name, 
		const char *value, const char *comm);

//添加一注释，段不存在不会自动创建
int ini_add_comment(struct INI_FILE *ini_f, const char *sectionName, const char *comm);

//删除段
int ini_del_section(struct INI_FILE *ini_f, const char *sectionName);

//删除名值对
int ini_del_item(struct INI_FILE *ini_f, const char *sectionName, const char *name);

//删除注释
int ini_del_comment(struct INI_FILE *ini_f, const char *sectionName, const char *comm);
```

测试
-----------
`test` 目录有测试demo

