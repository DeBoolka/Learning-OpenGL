#define _CRT_SECURE_NO_WARNINGS

#include "tools.h"

char	*LoadShader(char *file_name)
{
	FILE *file = fopen(file_name, "rt");

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);

		long length = ftell(file);

		char *shader_program = (char *)malloc(sizeof(char) * (length));
		memset(shader_program, 0, sizeof(char) * length);

		fseek(file, 0, SEEK_SET);

		fread(shader_program, sizeof(char), sizeof(char) * length, file);

		return (char *)shader_program;

	}

	return NULL;
}

void	FreeShader(void *ptr)
{
	free(ptr);
}
