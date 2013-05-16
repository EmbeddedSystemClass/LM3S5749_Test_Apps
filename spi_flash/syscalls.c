#include <sys/types.h>
#include <sys/stat.h>
#include "utils/uartstdio.h"
#include "usb_serial.h"


caddr_t _sbrk(int incr)
{
	extern char _end; /* Defined by the linker */
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0)
	{
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
#if 0
	// Check if heap hit the stack
	if (heap_end + incr > stack_ptr)
	{
		abort ();
	}
#endif
	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len)
{
	return USBSerialWrite(ptr, len);
}

int _close(int file)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _open(const char *name, int flags, int mode)
{
	return -1;
}

int _read(int file, char *ptr, int len)
{
	return USBSerialRead(ptr, len);
}

int _stat(char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}
