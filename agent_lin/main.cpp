#include <stdio.h>
#include <string.h>

int main ()
{
	FILE *f = fopen ("config", "rt");
	
	while (!feof (f))
	{
		char str[200];
		fscanf (f, "%s", str);

		char* pos = strchr (str, '=');
		if (pos == 0)
			continue;
		*pos = 0;

		char key[200], value[200];
		strcpy (key, str);
		strcpy (value, pos + 1);

		printf ("|%s|=|%s|\r\n", str, value);
	}

	fclose (f);

	return 0;
}
