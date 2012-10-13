#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include "my402list.h"

typedef struct tagPacket {
	int num_tokens;
	long service_time;
	int pkt_id;
	long inter_arrival_time;
	long sys_enter,q1_enter,q1_exit,q2_enter,q2_exit,sys_exit,serv_time;
} Packet;

#define PrintStat(time) fprintf(stdout,"%012.3f: ",(double)(time-temulation_start)/1000);

// Global Variables
double lambda, mu, r; // command line arguments
int B, P, num_packets, pkts_to_arrive,pkts_left_q1;
char emulation_mode;
long tokenarrival;
int tokencount;
int token_die;			// To inform token thread that it should stop now.
int server_die;			// To inform server thread that it should stop now.
FILE *fp;
// Global Variable Ends


// Time Stamps for Calculating Differences
long tastart, taend;
long ttstart, ttend;
long temulation_start, temulation_end;
// Time Stamps Ends

// Token Bucket
int token_bucket;

// Mutexes and Condition Variables
pthread_mutex_t my_mutex;
pthread_cond_t queue2_cond;
// Ends

// Queues
My402List queue1;
My402List queue2;
// Ends

//Statistics
double avg_inter_arrival, avg_serv_time, avg_pkt_sys_time, std_deviation;
double token_drop_prob, pkt_drop_prob;
long avg_pkt_q1, avg_pkt_q2, avg_pkt_s;
//Ends

// Prototype Definitions

void Usage();
int validate_argument(char *);
void parseCommandline(int, char **);
