#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "platform.h"
#include "error_proc.h"

status_t init_daemon(void)
{
	pid_t pid;
	if ((pid = fork()) < 0) {
		return FAILED;
	}

	/* Parent terminates */
	else if (pid) {
		exit(0);
	}

	/* 1th child continues... */

	/* Become session leader */
	if (setsid() < 0) { 
		return FAILED;
	}

	signal(SIGHUP, SIG_IGN);

	if ((pid = fork()) < 0) {
		return FAILED;
	}

	/* 1th child terminates */
	else if (pid) {
		exit(0);
	}

	/* 2th child continues... */

	chdir("/");

	/* Close off file descriptors */
/*
	for (int i = 0; i < MAXFD; i++) {
		close(i);
	}
*/

	/* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

	return OK;
}
