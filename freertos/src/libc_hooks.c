#define ARG_UNUSED(x) (void)(x)

#include <stdio.h>
#include <sys/stat.h>

void _exit(int status)
{
	while (1) {
		;
	}
}

void *_sbrk_r(struct _reent *r, int count)
{
	ARG_UNUSED(r);
    ARG_UNUSED(count);

	return NULL;
}

int _kill_r(struct _reent *r, int i, int j)
{
	ARG_UNUSED(r);
    ARG_UNUSED(i);
    ARG_UNUSED(j);

	return 0;
}

int _getpid_r(struct _reent *r)
{
	ARG_UNUSED(r);

	return 0;
}

_ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t nbytes)
{
	ARG_UNUSED(r);
    ARG_UNUSED(fd);
    ARG_UNUSED(buf);
    ARG_UNUSED(nbytes);

	return 0;
}

int _close_r(struct _reent *r, int file)
{
	ARG_UNUSED(r);
    ARG_UNUSED(file);

	return 0;
}

int _fstat_r(struct _reent *r, int file, struct stat *st)
{
	ARG_UNUSED(r);
    ARG_UNUSED(file);
    ARG_UNUSED(st);

	return 0;
}

int _isatty_r(struct _reent *r, int file)
{
	ARG_UNUSED(r);
    ARG_UNUSED(file);

	return 0;
}

_off_t _lseek_r(struct _reent *r, int file, _off_t ptr, int dir)
{
	ARG_UNUSED(r);
	ARG_UNUSED(file);
	ARG_UNUSED(ptr);
	ARG_UNUSED(dir);

	return 0;
}

_ssize_t _read_r(struct _reent *r, int fd, void *buf, size_t nbytes)
{
	ARG_UNUSED(r);
    ARG_UNUSED(fd);
    ARG_UNUSED(buf);
    ARG_UNUSED(nbytes);

	return 0;
}