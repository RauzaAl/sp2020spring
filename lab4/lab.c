#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <sys/resource.h>

#define THEPID 2973

// Nice value is a user-space and priority PR is the process's actual
// priority that use by Linux Kernel. In linux system priorities are
// 0 to 139 in which 0 to 99 for a real time and 100 to 139 for users. 
// Nice value range is -20 to +19 where -20 is highest, 0 be default, +19 is lowest.
// Relation between nice value and priority is: PR = 20 + NI
// PRIO_PROCESS is 0
void change_nice_value(pid_t pid, int niceval) {
   setpriority(PRIO_PROCESS, pid, niceval); 
}

// The sched_setscheduler() system call sets both the scheduling policy and parameters
// for the thread whose ID is specified in pid. If pid equals zero,
// the scheduling policy and parameters of the calling thread will be set. 
void alter_scheduling(pid_t pid) {
	// Data structure to describe a process' schedulability
    struct sched_param sched;
    sched.sched_priority = 10;
    if ( sched_setscheduler(pid, SCHED_RR, &sched) < 0 ) {
		// Errno - the error code set by various library functions
        fprintf(stderr, "SETSCHEDULER failed - err = %s\n", strerror(errno));
    } else {
        printf("Priority set to \"%d\"\n", sched.sched_priority);
    }
}

// With challenge
void print_info(pid_t pid) {
	int policy;
	int possible_min_priority;
	int possible_max_priority;
	size_t cpusetsize = 0;
	// Data structure to describe CPU mask
	// Used for get and set affinity
	cpu_set_t cpumask;
	
	struct sched_param params;
	struct timespec timespec;
	
	// If we can set schedulle for a process, then we can easily get it.
	policy = sched_getscheduler(pid);
	// Used to get a process's real-time priority
	sched_getparam(pid, &params);
	
	// Get max and min real-time priority
	possible_min_priority = sched_get_priority_min(policy);
	possible_max_priority = sched_get_priority_max(policy);

	// Get the SCHED_RR interval for the named process.
	sched_rr_get_interval(pid, &timespec);
	// Get a process's processor affinity 
	sched_getaffinity(pid, cpusetsize, &cpumask);

	switch (policy){
		case SCHED_OTHER:
			printf("SCHED_OTHER\n");
			break;
		case SCHED_FIFO:
			printf("SCHED_FIFO\n");
			break;
		case SCHED_RR:
			printf("SCHED_RR\n");
			break;
		default:
			printf(" unknown policy (%d)!\n", policy);
		break;
	}
	
	printf("Priority: %d\n", params.sched_priority);
	printf("Possible min priority: %d\n", possible_min_priority);
	printf("Possible max priority: %d\n", possible_max_priority);
	printf("Timeslice info: tv_nsec - %lld\n", timespec.tv_nsec);
	printf("Challenge:\n");
	int ret;
	int i = 0;
	int numOfprocess = 31;
	pid_t pidArray[] = {
		1, 2, 3, 4, 5, 6, 7, 8, 138, 140, 141, 143,
		156, 169, 170, 252, 255, 309, 317, 325, 399,
		404, 409, 412, 415, 418, 460, 469, 487, 500,
		505 };

	pid_t pidArrayByPolicyOther[] = {1, 2, 3, 4, 5, 6, 7, 8, 138, 140, 141, 143,
		156, 169, 170, 252, 255, 309, 317, 325, 399,
		404, 409, 412, 415, 418, 460, 469, 487, 500,
		505};
	int counterO = 0;
	pid_t pidArrayByPolicyFifo[] = {1, 2, 3, 4, 5, 6, 7, 8, 138, 140, 141, 143,
		156, 169, 170, 252, 255, 309, 317, 325, 399,
		404, 409, 412, 415, 418, 460, 469, 487, 500,
		505};
	int counterF = 0;
	pid_t pidArrayByPolicyRr[] = {1, 2, 3, 4, 5, 6, 7, 8, 138, 140, 141, 143,
		156, 169, 170, 252, 255, 309, 317, 325, 399,
		404, 409, 412, 415, 418, 460, 469, 487, 500,
		505};
	int counterR = 0;
	pid_t pidArrayByPolicyError[] = {1, 2, 3, 4, 5, 6, 7, 8, 138, 140, 141, 143,
		156, 169, 170, 252, 255, 309, 317, 325, 399,
		404, 409, 412, 415, 418, 460, 469, 487, 500,
		505};
	int counterE = 0;
	pid_t pidArrayByPolicyNoMatch[] = {1, 2, 3, 4, 5, 6, 7, 8, 138, 140, 141, 143,
		156, 169, 170, 252, 255, 309, 317, 325, 399,
		404, 409, 412, 415, 418, 460, 469, 487, 500,
		505};
	int counterN = 0;
	
	struct sched_param stSched = { 0 };

	while( i < numOfprocess) {
		int j = i;
		ret = sched_getscheduler(pidArray[j]); // get the policy
		if(ret < 0) {
			pidArrayByPolicyError[counterE] = pidArray[j];
			counterE++;
			printf("Error: sched_getscheduler for process with P_ID: %d\n", pidArray[j]);
		}
		printf("Process with P_ID: %d Policy: %d", pidArray[j], ret);

		switch (ret)
		{
		case SCHED_OTHER: // the default policy, use SCHED_NORMAL instead of SCHED_OTHER sometimes
			pidArrayByPolicyOther[counterO] = pidArray[j];
			counterO++;
			printf("Process with P_ID: %d Policy SCHED_OTHER: %d", pidArray[j], ret);
			break;
		case SCHED_FIFO:
			pidArrayByPolicyFifo[counterF] = pidArray[j];
			counterF++;
			printf("Process with P_ID: %d Policy SCHED_FIFO: %d", pidArray[j], ret);
			break;
		case SCHED_RR:
			pidArrayByPolicyRr[counterR] = pidArray[j];
			counterR++;
			printf("Process with P_ID: %d Policy SCHED_RR: %d", pidArray[j], ret);
			break;
		// case SCHED_BATCH: this Linux does not supports it
		// 	break;
		// case SCHED_IDLE: also does not this
		// 	break;
		default:
			pidArrayByPolicyNoMatch[counterN] = pidArray[j];
			counterN++;
			printf("No mathcing schedule policy!\n");
			break;
		};
	
		ret = sched_getparam(pidArray[j], &stSched); // get the priority
		if(ret != 0) {
			printf("Error: sched_getparam\n");
		}
		// printf("Priority: %d\n", stSched.sched_priority);
		memset(&stSched, 0, sizeof(struct sched_param));
		i++;
	}
	printf("Processes with POLICY: OTHER.\n");
	for(int k = 0; k < counterO; k++){
		printf("Process pid: %d.\n", pidArrayByPolicyOther[k]);
	}
	printf("Processes with POLICY: FIFO.\n");
	for(int l = 0; l < counterF; l++){
		printf("Process pid: %d.\n", pidArrayByPolicyFifo[l]);
	}
	printf("Processes with POLICY: RR.\n");
	for(int m = 0; m < counterR; m++){
		printf("Process pid: %d,\n", pidArrayByPolicyRr[m]);
	}
	printf("Processes with POLICY: ERROR.\n");
	for(int o = 0; o < counterE; o++){
		printf("Process pid: %d.\n", pidArrayByPolicyError[o]);
	}
	printf("Processes with no POLICY matched.\n");
	for(int p = 0; p < counterN; p++){
		printf("Process pid: %d.\n", pidArrayByPolicyNoMatch[p]);
	}
}


int main() {
	// Temporarily yields the processor
	// It causes the calling thread to relinquish the CPU.
	// The thread is moved to the end of the queue for its static priority
	// and a new thread gets to run.
	sched_yield();
	pid_t pid = THEPID;
	alter_scheduling(pid);
	change_nice_value(pid, -20);
	print_info(pid);

	return 0;
}