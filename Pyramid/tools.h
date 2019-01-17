#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

char	*LoadShader(char *file_name);
void	FreeShader(void *ptr);

#endif