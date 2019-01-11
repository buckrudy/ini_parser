#ifndef __INI_H__
#define __INI_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "list.h"

#ifndef INI_ERROR
#define INI_ERROR -1
#endif

#ifndef INI_SUCESS
#define INI_SUCESS 0
#endif

#define ini_malloc malloc
#define ini_free   free

struct INI_ITEM {
	struct list_head node;
	char *name;
	char *value;
	char *comment;
	unsigned char isComment;
};

struct INI_SECTION {
	struct list_head item_head;
	struct list_head node;
	char *name;
	char *comment;
};

struct INI_FILE {
	struct list_head section_head;
};

/* internal API, application shouldn't used */
struct INI_FILE *ini_create_INI_FILE(void);
struct INI_SECTION *ini_create_INI_SECTION(const char *name, const char *comm);
struct INI_SECTION *ini_get_INI_SECTION(struct INI_FILE *ini_f, const char *name);
int ini_add_INI_SECTION(struct INI_FILE *ini_f, struct INI_SECTION *ini_s);
struct INI_ITEM *ini_create_INI_ITEM(const char *name, const char *value, const char *comm);
int ini_add_INI_ITEM(struct INI_SECTION *ini_s, struct INI_ITEM *ini_i);
int ini_add_Comment_ITEM(struct INI_SECTION *ini_s, const char *comm);
struct INI_ITEM *ini_get_INI_ITEM(struct INI_SECTION *ini_s, const char *name);
void ini_free_INI_FILE(struct INI_FILE *ini_f);
void ini_free_INI_SECTION(struct INI_SECTION *ini_s);
void ini_free_INI_ITEM(struct INI_ITEM *ini_i);


/* application API */
struct INI_FILE *ini_create_ini_file(void);
int ini_dump_to_FILE_RemoveComment(struct INI_FILE *ini_f, FILE *fp);
int ini_dump_to_FILE(struct INI_FILE *ini_f, FILE *fp);
int ini_dump(struct INI_FILE *ini_f);
char *ini_getString(struct INI_FILE *ini_f, const char *sect, const char *name);
int ini_getInt(struct INI_FILE *ini_f, const char *sect, const char *name, int def);
int ini_load_file(const char *filename, struct INI_FILE **ini);
void ini_del_ini(struct INI_FILE *ini_f);
int ini_add_section_to_ini(struct INI_FILE *ini_f, const char *sectionName);
int ini_add_item_to_ini(struct INI_FILE *ini_f, 
		const char *sectionName, const char *name, 
		const char *value, const char *comm);
int ini_add_comment(struct INI_FILE *ini_f, const char *sectionName, const char *comm);
int ini_del_section(struct INI_FILE *ini_f, const char *sectionName);
int ini_del_item(struct INI_FILE *ini_f, const char *sectionName, const char *name);
int ini_del_comment(struct INI_FILE *ini_f, const char *sectionName, const char *comm);

#ifdef __cplusplus
}
#endif
#endif
