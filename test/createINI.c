#include "ini.h"

int main(void)
{
	struct INI_FILE *ini_f;


	ini_f = ini_create_ini_file();
	ini_add_section_to_ini(ini_f, "user");
	ini_add_item_to_ini(ini_f, "user", "name", "googol", NULL);
	ini_add_item_to_ini(ini_f, "user", "password", "xxx", "secret key");
	ini_add_comment(ini_f, "user", "this is a comment line");
	ini_dump(ini_f);
	ini_del_ini(ini_f);

	return 0;
}
