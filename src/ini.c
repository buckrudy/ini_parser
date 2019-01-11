/* Author: leegoogol */
/* Email: buckgugle@gmail.com */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "ini.h"

static char *trimright(char *s);
static char *trimleft(char *s);
static char *trim(char *s);
static int isComment(char *s);
static const char *trimCommentHead(const char *s);


struct INI_FILE *ini_create_INI_FILE(void)
{
	struct INI_FILE *f_p = ini_malloc(sizeof(struct INI_FILE));
	if (f_p) {
		INIT_LIST_HEAD(&f_p->section_head);
	}
	return f_p;
}

void ini_free_INI_FILE(struct INI_FILE *ini_f)
{
	struct INI_SECTION *ini_s, *tmp;

	if (ini_f == NULL) return;

	if (list_empty(&ini_f->section_head))
		goto __FREE;
	
	list_for_each_entry_safe(ini_s, tmp, &ini_f->section_head, node) {
		ini_free_INI_SECTION(ini_s);
	}

__FREE:
	ini_free(ini_f);
}

void ini_free_INI_SECTION(struct INI_SECTION *ini_s)
{
	struct INI_ITEM *ini_i, *tmp;

	if (ini_s == NULL) return;

	if (list_empty(&ini_s->item_head))
		goto __FREE;

	list_for_each_entry_safe(ini_i, tmp, &ini_s->item_head, node) {
		ini_free_INI_ITEM(ini_i);
	}

__FREE:
	if (!list_empty(&ini_s->node)) list_del(&ini_s->node);
	if (ini_s->name) free(ini_s->name);
	if (ini_s->comment) free(ini_s->comment);
	ini_free(ini_s);
}

void ini_free_INI_ITEM(struct INI_ITEM *ini_i)
{
	if (ini_i == NULL) return;

	if (!list_empty(&ini_i->node)) list_del(&ini_i->node);
	if (ini_i->name) free(ini_i->name);
	if (ini_i->value) free(ini_i->value);
	if (ini_i->comment) free(ini_i->comment);
	ini_free(ini_i);
}

struct INI_SECTION *ini_create_INI_SECTION(const char *name, const char *comm)
{
	struct INI_SECTION *s_p;

	if (name == NULL)
		return NULL;

	s_p = ini_malloc(sizeof(struct INI_SECTION));
	if (s_p) {
		memset(s_p, 0, sizeof(struct INI_SECTION));
		INIT_LIST_HEAD(&s_p->item_head);
		INIT_LIST_HEAD(&s_p->node);
		s_p->name = strdup(name);
		if (s_p->name == NULL) {
			ini_free(s_p);
			return NULL;
		}
	}
	return s_p;
}


struct INI_SECTION *ini_get_INI_SECTION(struct INI_FILE *ini_f, const char *name)
{
	struct INI_SECTION *ini_s;

	if (ini_f == NULL || name == NULL)
		return NULL;

	if (!list_empty(&ini_f->section_head)) {
		list_for_each_entry(ini_s, &ini_f->section_head, node) {
			if (strcmp(ini_s->name, name) == 0)
				return ini_s;
		}
	}
	return NULL;
}

int ini_add_INI_SECTION(struct INI_FILE *ini_f, struct INI_SECTION *ini_s)
{
	if (ini_f == NULL || ini_s == NULL)
		return INI_ERROR;

	list_add_tail(&ini_s->node, &ini_f->section_head);
	return INI_SUCESS;
}

struct INI_ITEM *ini_create_INI_ITEM(const char *name, const char *value, const char *comm)
{
	struct INI_ITEM *i_p = ini_malloc(sizeof(struct INI_SECTION));
	if (i_p) {
		memset(i_p, 0, sizeof(struct INI_ITEM));
		INIT_LIST_HEAD(&i_p->node);
#if 1
		i_p->name = (name && name[0]!='\0')?strdup(name):NULL;
		i_p->value = value?strdup(value):NULL;
		i_p->comment = (comm && comm[0]!='\0')?strdup(comm):NULL;
#else
		i_p->name = name?strdup(name):NULL;
		i_p->value = value?strdup(value):NULL;
		i_p->comment = comm?strdup(comm):NULL;
#endif
	}
	return i_p;
}

int ini_add_INI_ITEM(struct INI_SECTION *ini_s, struct INI_ITEM *ini_i)
{
	if (ini_s == NULL || ini_i == NULL)
		return INI_ERROR;

	list_add_tail(&ini_i->node, &ini_s->item_head);
	return INI_SUCESS;
}

struct INI_ITEM *ini_get_INI_ITEM(struct INI_SECTION *ini_s, const char *name)
{
	struct INI_ITEM *ini_i;

	if (ini_s == NULL)
		return NULL;

	if (!list_empty(&ini_s->item_head)) {
		list_for_each_entry(ini_i, &ini_s->item_head, node) {
			if (ini_i->name != NULL) {	//Not comment
				if (strcmp(ini_i->name, name)==0)
					return ini_i;
			}
		}
	}
	return NULL;
}

int ini_parse_key_value(struct INI_SECTION *ini_s, char *s, int line_num)
{
	struct INI_ITEM *ini_i;
	char *comm = NULL;
	char *index = strchr(s, '=');

	if (index == NULL) {
		fprintf(stderr, "invalid line: %d\n", line_num);
		return INI_ERROR;
	}
	*index = '\0';

	s = trim(s);

	comm = strpbrk(index+1, ";#");
	if (comm) {
		int plen;
		*comm = '\0';
		plen = strspn(comm+1, ";# ");
		comm = comm + 1 + plen;
	}

	index = trim(index+1);

	if (strlen(s) == 0/* || strlen(index) == 0*/) {
		fprintf(stderr, "invalid line: %d\n", line_num);
		return INI_ERROR;
	}

	ini_i = ini_get_INI_ITEM(ini_s, s);
	if (ini_i) {
		fprintf(stderr, "item already existed, line: %d\n", line_num);
		return INI_ERROR;
	}

	ini_i = ini_create_INI_ITEM(s, index, comm);
	if (ini_i) {
		ini_add_INI_ITEM(ini_s, ini_i);
		return INI_SUCESS;
	}
	fprintf(stderr, "Not enough memory.\n");
	return INI_ERROR;
}

int ini_add_Comment_ITEM(struct INI_SECTION *ini_s, const char *comm)
{
	const char *c = trimCommentHead(comm);
	struct INI_ITEM *ini_i = ini_create_INI_ITEM(NULL, NULL, c);
	ini_i->isComment = 1;
	list_add_tail(&ini_i->node, &ini_s->item_head);
	return INI_SUCESS;
}


int ini_parse_fp(FILE *fp, struct INI_FILE **ini)
{
	char buf[4096], *p, *index;
	struct INI_FILE *ini_f = ini_create_INI_FILE();
	struct INI_SECTION *ini_s = NULL;
	int line_num = 0;

	if (ini_f == NULL)
		return INI_ERROR;

	while (fgets(buf, sizeof(buf), fp)) {
		line_num ++;	//recode line number
		if (buf[0] == '\n' || buf[0] == '\r')
			continue;

		p = trim(buf);
		if (strlen(p) == 0)
			continue;

		if (p[0] == '[') {
			index = strchr(p, ']');
			if (index == NULL) {	//Error
				fprintf(stderr, "not match ], line: %d\n", line_num);
				goto _ERROR;
			}

			if (p + 1 == index) {
				fprintf(stderr, "No section name, line: %d\n", line_num);
				goto _ERROR;
			}

			*index = '\0';

			p = trim(p+1);
			if (strlen(p) == 0) {
				fprintf(stderr, "No section name, line: %d\n", line_num);
				goto _ERROR;
			}

			ini_s = ini_get_INI_SECTION(ini_f, p);
			if (ini_s) {
				fprintf(stderr, "section already exist, line: %d\n", line_num);
				goto _ERROR;
			}

			ini_s = ini_create_INI_SECTION(p, "");
			if (ini_s)
				ini_add_INI_SECTION(ini_f, ini_s);
			else {
				fprintf(stderr, "Not enough memory.\n");
				goto _ERROR;
			}
		} else if (isComment(p)) {	//process this later
			if (ini_s == NULL) {
				ini_s = ini_create_INI_SECTION("", "");	//default section
				if (ini_s)
					ini_add_INI_SECTION(ini_f, ini_s);
				else {
					fprintf(stderr, "Not enough memory.\n");
					goto _ERROR;
				}
			}
			ini_add_Comment_ITEM(ini_s, p);
		} else {
			if (ini_s == NULL) {
				ini_s = ini_create_INI_SECTION("", "");	//default section
				if (ini_s)
					ini_add_INI_SECTION(ini_f, ini_s);
				else {
					fprintf(stderr, "Not enough memory.\n");
					goto _ERROR;
				}
			}

			if (INI_ERROR == ini_parse_key_value(ini_s, p, line_num))
				goto _ERROR;
		}
	}

	*ini = ini_f;
	return INI_SUCESS;

_ERROR:
	ini_free_INI_FILE(ini_f);
	*ini = NULL;
	return INI_ERROR;
}

int ini_load_file(const char *filename, struct INI_FILE **ini)
{
	FILE *fp;

	if (filename == NULL || ini == NULL)
		return INI_ERROR;

	if (access(filename, F_OK) !=0 )
		return INI_ERROR;
	
	fp = fopen(filename, "rb");
	if (fp == NULL)
		return INI_ERROR;

	if (INI_ERROR == ini_parse_fp(fp, ini)) {
		fclose(fp);
		return INI_ERROR;
	}

	fclose(fp);
	return INI_SUCESS;
}

void ini_del_ini(struct INI_FILE *ini_f)
{
	ini_free_INI_FILE(ini_f);
}

static int __ini_dump(struct INI_FILE *ini_f, FILE *fp, int removeComment)
{
	struct INI_SECTION *ini_s;
	struct INI_ITEM *ini_i;

	const char *terminal_fmt = removeComment==0?"\n":"";
	const char *name_val_fmt = removeComment==0?"%s = %s\t":"%s = %s\n";

	list_for_each_entry(ini_s, &ini_f->section_head, node) {
		if (ini_s->name[0] != '\0')	//is not default section
			fprintf(fp, "[ %s ]\n", ini_s->name);
		
		list_for_each_entry(ini_i, &ini_s->item_head, node) {
			if (ini_i->name != NULL)
				fprintf(fp, name_val_fmt, ini_i->name, ini_i->value);
			if (removeComment == 0) {	//dump comment
				if (ini_i->comment != NULL)
					fprintf(fp, ";%s", ini_i->comment);
			}
			fprintf(fp, terminal_fmt);
		}
		fprintf(fp, "\n");
	}
	return INI_SUCESS;
}

int ini_dump_to_FILE_RemoveComment(struct INI_FILE *ini_f, FILE *fp)
{
	if (ini_f == NULL || fp == NULL)
		return INI_ERROR;

	return __ini_dump(ini_f, fp, 1);
}

int ini_dump_to_FILE(struct INI_FILE *ini_f, FILE *fp)
{
	if (ini_f == NULL || fp == NULL)
		return INI_ERROR;

	return __ini_dump(ini_f, fp, 0);
}

int ini_dump(struct INI_FILE *ini_f)
{
	return ini_dump_to_FILE(ini_f, stdout);
}

char *ini_getString(struct INI_FILE *ini_f, const char *sect, const char *name)
{
	struct INI_SECTION *ini_s;
	struct INI_ITEM *ini_i;

	if (ini_f == NULL || sect == NULL || name == NULL)
		return NULL;

	ini_s = ini_get_INI_SECTION(ini_f, sect);
	if (ini_s) {
		ini_i = ini_get_INI_ITEM(ini_s, name);
		if (ini_i)
			return ini_i->value;
	}
	return NULL;
}

int ini_getInt(struct INI_FILE *ini_f, const char *sect, const char *name, int def)
{
	char *value;
	value = ini_getString(ini_f, sect, name);
	if (value)
		return strtol(value, NULL, 0);
	return def;
}

struct INI_FILE *ini_create_ini_file(void)
{
	return ini_create_INI_FILE();
}


int ini_add_section_to_ini(struct INI_FILE *ini_f, const char *sectionName)
{
	struct INI_SECTION *ini_s;
	
	ini_s = ini_get_INI_SECTION(ini_f, sectionName);
	if (ini_s)
		return INI_SUCESS;

	ini_s = ini_create_INI_SECTION(sectionName, NULL);
	if (ini_s) {
		ini_add_INI_SECTION(ini_f, ini_s);
		return INI_SUCESS;
	}
	return INI_ERROR;
}

/*
 * item不存在则创建，存在则替换
 **/
int ini_add_item_to_ini(struct INI_FILE *ini_f, 
		const char *sectionName, const char *name, 
		const char *value, const char *comm)
{
	struct INI_SECTION *ini_s;
	struct INI_ITEM *ini_i;
	ini_s = ini_get_INI_SECTION(ini_f, sectionName);
	if (ini_s == NULL) {
		ini_s = ini_create_INI_SECTION(sectionName, NULL);
		if (ini_s)
			ini_add_INI_SECTION(ini_f, ini_s);
		else
			return INI_ERROR;
	}

	ini_i = ini_get_INI_ITEM(ini_s, name);
	if (ini_i == NULL) {	//Not found
		ini_i = ini_create_INI_ITEM(name, value, comm);
		if (ini_i) {
			ini_add_INI_ITEM(ini_s, ini_i);
		} else {
			return INI_ERROR;
		}
	} else {	//found;
		if (ini_i->value)
			free(ini_i->value);
		if (ini_i->comment)
			free(ini_i->comment);

		ini_i->value = strdup(value);
		ini_i->comment = comm&&comm[0]!='\0'?strdup(comm):NULL;
	}
	return INI_SUCESS;
}


/*
 * 段不存在时，不会创建段
 */
int ini_add_comment(struct INI_FILE *ini_f, const char *sectionName, const char *comm)
{
	struct INI_SECTION *ini_s;
	if (ini_f == NULL)
		return INI_ERROR;

	ini_s = ini_get_INI_SECTION(ini_f, sectionName);
	if (ini_s == NULL)
		return INI_ERROR;

	return ini_add_Comment_ITEM(ini_s, comm);
}

int ini_del_section(struct INI_FILE *ini_f, const char *sectionName)
{
	struct INI_SECTION *ini_s;
	if (ini_f == NULL || sectionName == NULL)
		return INI_ERROR;

	ini_s = ini_get_INI_SECTION(ini_f, sectionName);
	if (ini_s == NULL)
		return INI_ERROR;

	ini_free_INI_SECTION(ini_s);
	return INI_SUCESS;
}

int ini_del_item(struct INI_FILE *ini_f, const char *sectionName, const char *name)
{
	struct INI_SECTION *ini_s;
	struct INI_ITEM *ini_i;

	if (ini_f == NULL || sectionName == NULL || name == NULL)
		return INI_ERROR;

	ini_s = ini_get_INI_SECTION(ini_f, sectionName);
	if (ini_s == NULL)
		return INI_ERROR;

	ini_i = ini_get_INI_ITEM(ini_s, name);
	if (ini_i == NULL)
		return INI_ERROR;

	ini_free_INI_ITEM(ini_i);
	return INI_SUCESS;
}


int ini_del_comment(struct INI_FILE *ini_f, const char *sectionName, const char *comm)
{
	struct INI_SECTION *ini_s;
	struct INI_ITEM *ini_i;

	if (ini_f == NULL || sectionName == NULL || comm == NULL)
		return INI_ERROR;

	ini_s = ini_get_INI_SECTION(ini_f, sectionName);
	if (ini_s == NULL)
		return INI_ERROR;

	list_for_each_entry(ini_i, &ini_s->item_head, node) {
		if (ini_i->name == NULL && ini_i->value == NULL && ini_i->comment) {
			if (strcmp(ini_i->comment, comm)==0)
				break;
		}
	}
	if (ini_i)
		ini_free_INI_ITEM(ini_i);

	return INI_SUCESS;
}


static char *trimright(char *s)
{
	int i, len = strlen(s);
	for (i=len-1; i>=0; i--) {
		if (isspace(s[i]) || s[i]=='\r' || s[i]=='\n') {
			;
		} else {
			break;
		}
	}
	s[i+1] = '\0';
	return s;
}

static char *trimleft(char *s)
{
	int i, len = strlen(s);
	for (i=0; i<len; i++) {
		if (isspace(s[i]) || s[i]=='\r' || s[i]=='\n') {
			;
		} else {
			break;
		}
	}
	if (i!=0)
		s[i-1] = '\0';

	return &s[i];
}


static char *trim(char *s)
{
	int i, len = strlen(s);
	for (i=len-1; i>=0; i--) {
		if (isspace(s[i]) || s[i]=='\r' || s[i]=='\n') {
			;
		} else {
			break;
		}
	}
	s[i+1] = '\0';

	len = i;

	for (i=0; i<len; i++) {
		if (isspace(s[i]) || s[i]=='\r' || s[i]=='\n') {
			;
		} else {
			break;
		}
	}
	if (i!=0)
		s[i-1] = '\0';

	return &s[i];
}

static int isComment(char *s)
{
#if 0
	int i;
	i = strspn(s, ";#");
	if (i!=0)
		return 1;
	return 0;
#else
	if (s[0] == ';' || s[0] == '#')
		return 1;
	return 0;
#endif
}

static const char *trimCommentHead(const char *s)
{
	int i;
	i = strspn(s, ";# ");
	return s+i;
}
