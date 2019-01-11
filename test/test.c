#include "ini.h"

int main(int argc, char **argv)
{
	struct INI_FILE *ff;

	if (0 != ini_load_file("xxx.ini", &ff))
		return 1;

	ini_dump_to_FILE_RemoveComment(ff, stdout);

	printf("============= contain comment ============\n");

	ini_dump(ff);

	{
		char *value;
		value = ini_getString(ff, "hello", "ip");
		printf("ip = %s\n", value);
		printf("port = %d\n", ini_getInt(ff, "hello", "port", 80));

		value = ini_getString(ff, "", "xxx");
		printf("xxx = %s\n", value);
	}

	ini_del_ini(ff);

	return 0;
}
