#ifndef XBYPASS_H
#define XBYPASS_H

#include <stdarg.h>

void xf86Msg(int type, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

int xf86SetIntOption(pointer opts, const char *name, int deflt)
{
	return deflt;
}

int xf86SetBoolOption(pointer opts, const char *name, int deflt)
{
	return deflt;
}

double xf86SetRealOption(pointer opts, const char *name, double deflt)
{
	return deflt;
}

#endif
