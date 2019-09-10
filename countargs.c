
#include <stdio.h>
#include <string.h>
#include <linux/types.h>

// Counts the arguments in a comma seperated list.
// Used be the tglWidgetRegister #define
int countArgs(char *args) {
	int argc = 1;
	for (int i = 0; args[i]; i++)
		if (args[i] == ',')
			argc++;
	
	return argc;
}
