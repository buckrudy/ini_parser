#include "ini.h"

int main(int argc, char **argv)
{
	struct INI_FILE *ff;
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <ini file>\n", argv[0]);
		return 1;
	}
	if (0 != ini_load_file(argv[1], &ff))
		return 1;

	ini_dump(ff);

	ini_del_ini(ff);

	return 0;
}
