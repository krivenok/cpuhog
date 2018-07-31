#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <sched.h>
#include <sys/syscall.h>

int debug = 0;

void alarm_handler(int signo) {
	if (debug) fprintf(stderr,"Got signal %d, exiting.\n", signo);
	exit(0);
}

void setup_exit_timer_if_needed() {
	char* duration = getenv("CPUHOG_DURATION");
	// Do nothing if env var is not set (i.e. never exit)
	if(duration == NULL) return;

	// Setup exit handler and fire the timer
	signal(SIGALRM, alarm_handler);
	alarm(atoi(duration));
}

struct cpu_hog_thread {
	pthread_t thr;
	int core_index;
};

void *cpu_hog_thread(void *ptr) {
	struct cpu_hog_thread* t = ptr;
	cpu_set_t cpuset;
	int r;
	char name[16];
	char* do_syscall = getenv("CPUHOG_SYSCALL");

	assert(t);
	if (debug) fprintf(stderr,"CPU hog thread %d started\n", t->core_index);

	CPU_ZERO(&cpuset);
	CPU_SET(t->core_index, &cpuset);
	r = pthread_setaffinity_np(t->thr, sizeof(cpu_set_t), &cpuset);
	assert(r == 0);
	if (debug) fprintf(stderr,"Affined CPU hog thread %d to CPU core\n", t->core_index);

	snprintf(name, sizeof(name), "cpuhog%05d", t->core_index);
	r = pthread_setname_np(t->thr, name);
	assert(r == 0);
	if (debug) fprintf(stderr,"Changed CPU hog thread %d name to %s\n", t->core_index, name);

	if (do_syscall == NULL) {
		// Start infinite CPU hogging loop
		while(1);
	} else {
		// Start infinite loop where we do a syscall to trigger user->kernel->user switch
		while(1) syscall(SYS_gettid);
	}

	return NULL;
}

int main() {
	int cores_count = get_nprocs();
	int i;
	struct cpu_hog_thread* hog_threads;

	debug = getenv("CPUHOG_DEBUG") != NULL;
	assert(cores_count > 0);
	if (debug) fprintf(stderr,"Detected %d available CPU cores\n", cores_count);

	setup_exit_timer_if_needed();

	hog_threads = malloc(cores_count * sizeof(struct cpu_hog_thread));
	for(i=0; i<cores_count; ++i) {
		int r;
		hog_threads[i].core_index = i;
		r = pthread_create(&hog_threads[i].thr, NULL, cpu_hog_thread, (void *) &hog_threads[i]);
		assert(r == 0);
	}

	for(i=0; i<cores_count; ++i) {
		int r;
		r = pthread_join(hog_threads[i].thr, NULL);
		assert(r == 0);
	}
	free(hog_threads);

	return 0;
}
