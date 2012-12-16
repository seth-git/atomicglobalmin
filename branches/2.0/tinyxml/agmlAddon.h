
#ifndef __AGML_ADD_ON_H_
#define __AGML_ADD_ON_H_

#include <stddef.h>

template <size_t iSize>
void doubleToString(double _value, char (&buffer)[iSize])
{
	int i;
	snprintf(buffer, iSize, "%0.15f", _value);

	// Remove trailing zeros
	i = 0;
	while (buffer[i] != '\0')
		++i;
	--i;
	while (buffer[i] == '0') {
		buffer[i] = '\0';
		--i;
		if (buffer[i] == '.') {
			buffer[i] = '\0';
			break;
		}
	}
}

#endif
