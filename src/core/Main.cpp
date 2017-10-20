#include "IMServer.h"
#include "Config.h"
#include <stdio.h>
#include <Define.h>
#include <Log.h>
#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#include <signal.h>
#include<sys/wait.h>
static char*   im_signal;

static int get_options(int argc, char *const *argv)
{
	u_char     *p;
	int   i;

	for (i = 1; i < argc; i++) {

		p = (u_char *)argv[i];

		if (*p++ != '-') {
			fprintf(stderr,"invalid option: \"%s\"\n", argv[i]);
			return IM_ERR;
		}

		while (*p) {

			switch (*p++) {

			case 's':
				if (*p) {
					im_signal = (char *)p;

				}
				else if (argv[++i]) {
					im_signal = argv[i];

				}
				else {
					fprintf(stderr, "option \"-s\" requires parameter\n");
					return IM_ERR;
				}

				if (strcmp(im_signal, "stop") == 0
					|| strcmp(im_signal, "reload") == 0 || strcmp(im_signal, "restart") == 0)
				{
					
					goto next;
				}

				fprintf(stderr, "invalid option: \"-s %s\"\n", im_signal);
				return IM_ERR;

			default:
				fprintf(stderr, "invalid option: \"%c\"\n", *(p - 1));
				return IM_ERR;
			}
		}

	next:

		continue;
	}

	return IM_OK;
}

static int handler_signal(const char* pid_file) {
	int fd = open(pid_file, O_RDONLY);
	if (fd == -1) {
		perror("open");
		return IM_ERR;
	}
	pid_t  pid =0 ;
	if (read(fd, &pid, sizeof(pid)) != sizeof(int)) {
		perror("read");
		return IM_ERR;
	}
	if (!strcmp(im_signal, "stop")  || !strcmp(im_signal, "restart")) {
		if (kill(pid, SIGKILL) == -1) {
			fprintf(stderr, "close IMServer fail");
			return IM_ERR;
		}
	}
	close(fd);
	return IM_OK;
	
}

int daemon() {
	int  fd;
    pid_t pid=fork();
	switch (pid) {
	case -1:
		fprintf(stderr, "fork() failed\n");
		return IM_ERR;

	case 0:
		break;

	default:
		exit(0);
	}

	if (setsid() == -1) {
		fprintf(stderr , "setsid() failed\n");
		return IM_ERR;
	}

	umask(0);

	fd = open("/dev/null", O_RDWR);
	if (fd == -1) {
		fprintf(stderr,
			"open(\"/dev/null\") failed\n");
		return IM_ERR;
	}

	if (dup2(fd, STDIN_FILENO) == -1) {
		fprintf(stderr,  "dup2(STDIN) failed\n");
		return IM_ERR;
	}

	if (dup2(fd, STDOUT_FILENO) == -1) {
		fprintf(stderr,  "dup2(STDOUT) failed\n");
		return IM_ERR;
	}

	if (fd > STDERR_FILENO) {
		if (close(fd) == -1) {
			fprintf(stderr,  "close() failed\n");
			return IM_ERR;
		}
	}

	return IM_OK;
}
#endif

int main(int argc, char* const *argv)
{
	CConfigFileReader* pConfig = CConfigFileReader::getInstance();
	if (pConfig->initConfigName(CONFIG_PATH) != IM_OK) {
		return 0;
	}
	char* log_config=pConfig->getConfigName("LogConfig");
	if(log_config){
		initLog(log_config);
	}
#ifndef _WIN32
	if (IM_OK != get_options(argc, argv)) {
		return 0;
	}
	char* pid_file=pConfig->getConfigName("PidFile");
	if (pid_file == NULL) {
		fprintf(stderr, "can't find pid file\n");
		return 0;
	}
	if (im_signal) {
		if (IM_OK != handler_signal(pid_file)) {
			return 0;
		}
		if (strcmp(im_signal, "restart")) {
			return 0;
		}
		printf("begin restart\n");
	}

	char* value=pConfig->getConfigName("daemon");
	if (value != NULL &&  !strcmp(value, "on")) {
		if (IM_OK != daemon()) {
			return 0;
		}
	}

#endif

#ifndef WIN32
	signal(SIGPIPE, SIG_IGN);
#endif

	 IMServer* pServer = IMServer::get_instance();
	if(pServer->init()==IM_OK)
	{
		pServer->start();
	}
	return 0;
}
