#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <deque>
#include <tuple>

#include "paraskiplist.h"

#define MAX_THREAD 8

using namespace std;

// aggregate variables
long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

skiplist<int, int> list(0,1000000);

// pthread variables
pthread_t thread[MAX_THREAD];
pthread_mutex_t mtx_sum;
pthread_barrier_t barrier;

// function prototypes
int loadwork(deque<tuple<char, long>>& wq, char* fn) {
    FILE* fin = fopen(fn, "r");
    char action;
    long num;
    while(fscanf(fin, "%c %ld\n", &action, &num) > 0) {
        wq.push_back(make_tuple(action, num));
	}
    fclose(fin);
    return EXIT_SUCCESS;
}

void *dowork(void* tpl) {
	tuple<char, long> typle = *(tuple<char, long> *)tpl;
	char action = get<0>(typle);
	char num = get<1>(typle);
	switch(action) {
		case 'i':
			list.insert(num, num);
			pthread_mutex_lock(&mtx_sum);
			sum+=num;
			if(num%2 == 1)
				odd++;
			pthread_mutex_unlock(&mtx_sum);
			break;

		case 'q':
			if(list.find(num) != num)
				cout << "ERROR: Not found(" << num << ")\n";
			break;

		case 'w':
			//usleep(num);
			break;

		case 'p':
			cout << list.printList() << endl;
			break;

		default:
			cout << "ERROR: Unrecognized action(" << action << ")\n";
			exit(EXIT_FAILURE);
	}
	if(pthread_barrier_wait(&barrier) == EINVAL) {
		cout << "ERROR: Mismatched synchronization" << endl;
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    struct timespec start, stop;


    // check and parse command line options
    if (argc != 2) {
        printf("Usage: %s <infile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *fn = argv[1];

    clock_gettime( CLOCK_REALTIME, &start);

    deque< tuple<char,long> > workqueue;

    // load input file
    long sz;
    loadwork(workqueue, fn);
    while(sz = workqueue.size()) {
        long cnt = sz>=MAX_THREAD ? MAX_THREAD : sz;
        pthread_barrier_init(&barrier, NULL, cnt);
        for(int i=0; i<cnt; ++i) {
			void* tpl = (void*)&workqueue[0];
        	pthread_create(&thread[i], NULL, dowork, tpl);
			workqueue.pop_front();
		}
		for(int i=0; i<cnt; ++i)
			pthread_join(thread[i], NULL);
		pthread_barrier_destroy(&barrier);
    }

    clock_gettime( CLOCK_REALTIME, &stop);

    // print results
    cout << "Elapsed time: " << (stop.tv_sec - start.tv_sec) + ((double) (stop.tv_nsec - start.tv_nsec))/BILLION << " sec" << endl;

    // clean up and return
    return (EXIT_SUCCESS);

}

