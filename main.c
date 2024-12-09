#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

void multi_partition_mpi(long long *input, int n, long long *p, int np, long long *output, int *nO) {

}

long long geraAleatorioLL() {
	int a = rand();
	int b = rand();
	long long v = (long long)a * 100 + b;
	return v;
}

void swap(long long *a, long long *b) {
	long long t = *a;
	*a = *b;
	*b = t;
}

#define MAX 10

int main(int argc, char **argv) {

	srand(time(NULL));

	long long p[] = {LLONG_MIN, 0, LLONG_MAX};

	int p1, p2;
	p1 = p2 = 0;

	long long in[MAX];

	// Array creation
	for (int i=0; i<MAX; i++) {
		in[i] = 50-(rand()%100);
	}

	// Particion count
	for (int i=0; i<MAX; i++) {
		if (in[i] > p[0] && in[i] <= p[1])
			p1 += 1;
		if (in[i] > p[1] && in[i] <= p[2])
			p2 += 1;
	}

	int head1 = 0;
	int head2 = p1;

	int needle = head1;

	while (needle < MAX) {

		if (in[needle] > p[0] && in[needle] <= p[1]) {
			head1 += 1;
			needle += 1;
		}

		if (needle >= p1)
			needle = head2;

		if (in[needle] > p[1] && in[needle] <= p[2]) {
			swap(&in[needle], &in[head2]);
			head2 += 1;
		}

	}
	


	// arr print
	for (int i=0; i<MAX; i++) {
		printf("%lld\n", in[i]);
	}

	printf("%d el in 1\n", p1);
	printf("%d el in 2\n", p2);


	return 0;
}