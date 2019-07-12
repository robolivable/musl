#define _GNU_SOURCE
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>

#include "pthread_impl.h"

int pthread_getname_np(pthread_t thread, char *name, size_t len)
{
	int fd, cs, status = 0;
	char f[sizeof "/proc/self/task//comm" + 3*sizeof(int)];

	if (len > 15) return ERANGE;

	if (thread == pthread_self())
		return prctl(PR_GET_NAME, name) ? errno : 0;

	snprintf(f, sizeof f, "/proc/self/task/%d/comm", thread->tid);
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	if ((fd = open(f, O_RDONLY)) < 0 || read(fd, name, len) < 0) status = errno;
	if (fd >= 0) close(fd);
	pthread_setcancelstate(cs, 0);
	return status;
}
