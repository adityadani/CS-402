#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include "packet.h"

long getinstanttime()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return((tv.tv_sec*1000000) + tv.tv_usec);
}


void readfileline(long *at, long *st, int *token)
{
	char buf[1026];
	//char *cptr,*eptr;
	//int i=0;
	long arrt,servt;

	if(fgets(buf, sizeof(buf), fp)!=NULL) {
		if(buf[0] == ' ' || buf[0] == '\t'){
			sleep(1);
			fprintf(stderr,"\nError in tfile format\nLeading white/tab space detected\n\n");
			exit(1);
		}
		
		if(buf[strlen(buf) - 2] == ' ' || buf[strlen(buf) - 2] == '\t'){
			sleep(1);
			fprintf(stderr,"\nError in tfile format\nTrailing white/tab space detected\n\n");
			exit(1);
		}

		sscanf(buf,"%ld%d%ld",&arrt,token,&servt);
		*at = arrt * 1000;
		*st = servt * 1000;
	}
	/*	if(fgets(buf, sizeof(buf), fp)!=NULL){
		if(buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';
		cptr = buf;
		if(*cptr == ' ' || *cptr == '\t'){
			fprintf(stderr,"\nError in tfile format\nLeading white/tab space detected\n");
			exit(1);
		}
		else {
			eptr = cptr + 1;
			while(*eptr != '\0') {
				if (*eptr == ' ' || *eptr == '\t' || *eptr == '\0') {
					*eptr = '\0';
					if(i==0)
						*at = (long)(strtod(cptr,NULL)*1000);
					else if(i==1)
						*token = (int)(strtod(cptr,NULL));
					else
						*st = (long)(strtod(cptr,NULL)*1000);
					if(eptr!='\0') {
						if(i==2) {
							fprintf(stderr,"\nError in tfile format i=%d \nTrailing white/tab space detected\n",i);
							exit(1);
						}
						eptr++;
					}
					while(*eptr == ' ' || *eptr == '\t')
						eptr++;
					cptr = eptr;
					i++;
				}
				else
					eptr++;
			}
		}
	}
	*/
	
}


void getStatistics(long *at, long *st, int *token, int arrival_pkt_id)
{
	if(emulation_mode == 't') {
		readfileline(at,st,token);
	}
	else {
		*at = ((1/lambda)*1000000); // converting to micro seconds
		*st = ((1/mu)*1000000);
		*token = P;
	}
}


void *arrivalThread(void *id)
{
	My402ListElem *elem;
	Packet *pkt;
	int i;
	long pkt_sleep, pkt_service;
	int pkt_token;
	long sleep_time;
	long prev_pkt_time=0, instant_time;
	long avg_ia=0;


	for (i=0;i<num_packets;i++) {


		getStatistics(&pkt_sleep,&pkt_service,&pkt_token,i);		
		if(pkt_sleep > 10000000)
			pkt_sleep = 10000000;
		if(pkt_service > 1000000)
			pkt_service = 10000000;
				
		taend=getinstanttime();
		sleep_time = pkt_sleep - (taend-tastart);
		usleep(sleep_time);
		
		// Creating the Packet
		
		pkt = (Packet *)malloc(sizeof(struct tagPacket));
		pkt->pkt_id = i;
		pkt->num_tokens = pkt_token;
		pkt->service_time = pkt_service;

		pkt->sys_enter = tastart = getinstanttime();
		if(pkt->num_tokens > B) {
			// Drop the packet
			pkts_to_arrive--;
			PrintStat(getinstanttime());
			fprintf(stdout, "p%d arrives, Invalid token requirement, p%d Dropped!!\n", 
				pkt->pkt_id, pkt->pkt_id);
			continue;
		}
		else {
			instant_time = getinstanttime();
			if(prev_pkt_time==0) {
				prev_pkt_time = pkt->inter_arrival_time = (instant_time - temulation_start);
				prev_pkt_time = instant_time;
			}
			else {
				pkt->inter_arrival_time = instant_time - prev_pkt_time;
				prev_pkt_time = instant_time;
			}
			PrintStat(instant_time);
			avg_ia += pkt->inter_arrival_time;
			fprintf(stdout, "p%d arrives, needs %d tokens, inter-arrival time = %.3fms\n",
				pkt->pkt_id,pkt->num_tokens,(double)(pkt->inter_arrival_time)/1000);
		}

		pthread_mutex_lock(&my_mutex);
		if(My402ListEmpty(&queue1)){
			My402ListAppend(&queue1,pkt);
			pkt->q1_enter = getinstanttime();
			PrintStat(getinstanttime());
			fprintf(stdout,"p%d enters Q1\n", pkt->pkt_id);
			if(!My402ListEmpty(&queue2)) {
				pthread_cond_signal(&queue2_cond);
			}
			else {
				if(token_bucket >= pkt->num_tokens) {
					elem = My402ListFirst(&queue1);
					pkt = (Packet *)elem->obj;					

					My402ListUnlink(&queue1,elem);
					pkt->q1_exit = getinstanttime();
					token_bucket-=pkt->num_tokens;
					PrintStat(getinstanttime());
					fprintf(stdout, "p%d leaves Q1, time in Q1 = %.3fms, token bucket now has %d tokens\n", 
						pkt->pkt_id, (double)(pkt->q1_exit - pkt->q1_enter)/1000, token_bucket);
					avg_pkt_q1 += (pkt->q1_exit - pkt->q1_enter);
					pkts_left_q1++;
					My402ListAppend(&queue2,pkt);
					pkt->q1_enter = 0;
					pkt->q1_exit = 0;
					pkt->q2_enter = getinstanttime();
					PrintStat(getinstanttime());
					fprintf(stdout,"p%d enters Q2\n", pkt->pkt_id);
					pthread_cond_signal(&queue2_cond);
				}
			}
		}
		else {
			My402ListAppend(&queue1,pkt);
			pkt->q1_enter = getinstanttime();
			PrintStat(getinstanttime());
			fprintf(stdout,"p%d enters Q1\n", pkt->pkt_id);
		}
		pthread_mutex_unlock(&my_mutex);
	}
	pthread_mutex_lock(&my_mutex);
	pthread_cond_signal(&queue2_cond); // This can also be a false signal just to wake up server, if
	pthread_mutex_unlock(&my_mutex);   // say last packet is dropped and there is no pkt to be queued to q2.
	if(i==-1) {
		avg_inter_arrival = 0;
		pkt_drop_prob = 0;
	}
	else {
		avg_inter_arrival = (double)((double)avg_ia/(double)((i+1)*1000000));
		pkt_drop_prob = (double)(num_packets - pkts_to_arrive)/(i+1);
	}
	pthread_exit(NULL);
}

void *tokenThread(void *id)
{
	long sleep_time;
	int queue2_empty;
	int token_drop=0;
	Packet *pkt;
	My402ListElem *elem;
	while(1) {

		if(token_die==1) {
			token_drop_prob = (double)token_drop/tokencount;
			pthread_exit(NULL);
		}

		ttend=getinstanttime();
		sleep_time = tokenarrival - (ttend-ttstart);
		usleep(sleep_time);	
		ttstart = getinstanttime();
		pthread_mutex_lock(&my_mutex);
		if(token_bucket<B) {
			token_bucket++;
			tokencount++;
			PrintStat(getinstanttime());
			fprintf(stdout,"token t%d arrives, token bucket now has %d tokens\n",tokencount,token_bucket);
		}
		else {
			PrintStat(getinstanttime());
			tokencount++;
			token_drop++;
			fprintf(stdout,"token t%d arrives, token bucket full, token dropped!!\n",tokencount);
		}
		
		if(!My402ListEmpty(&queue1)) {
			elem = My402ListFirst(&queue1);
			pkt = (Packet *)elem->obj;
			if(token_bucket >= pkt->num_tokens) {
				token_bucket-=pkt->num_tokens;
				My402ListUnlink(&queue1,elem);
				pkt->q1_exit = getinstanttime();
				PrintStat(getinstanttime());
				fprintf(stdout, "p%d leaves Q1, time in Q1 = %.3fms, token bucket now has %d tokens\n", 
					pkt->pkt_id, (double)(pkt->q1_exit - pkt->q1_enter)/1000, token_bucket);
				avg_pkt_q1 += (pkt->q1_exit - pkt->q1_enter);
				pkts_left_q1++;
				queue2_empty = My402ListEmpty(&queue2);
				My402ListAppend(&queue2,pkt);
				pkt->q2_enter = getinstanttime();
				PrintStat(getinstanttime());
				fprintf(stdout,"p%d enters Q2\n", pkt->pkt_id);

				if(queue2_empty==1)
					pthread_cond_signal(&queue2_cond);
			}
		}
		pthread_mutex_unlock(&my_mutex);
		if(pkts_left_q1 == pkts_to_arrive) {
			token_drop_prob = (double)token_drop/tokencount;
			pthread_exit(NULL);	
		}
	}
}

void *serverThread(void *id)
{
	Packet *pkt;
	My402ListElem *elem;
	int i,pkt_processed = 0;
	long serv_start, serv_end, avg_st = 0, avg_pkst = 0,sys_time;
	double sys_avg = 0, sys_avg_sq = 0;
	double var_sys, var_sys_sq; 

	for(i=0;i<pkts_to_arrive;i++) {
		pthread_mutex_lock(&my_mutex);
		while(My402ListEmpty(&queue2) && server_die==0) {
			if(i==pkts_to_arrive)   // This is the case when the last packet is dropped. And the
				break; 		// server thread is waiting on q2 and no pkt is there to be queued to q2.
			pthread_cond_wait(&queue2_cond, &my_mutex);
		}
		if(i==pkts_to_arrive)
			server_die=1;
		if(server_die!=0) {
			My402ListUnlinkAll(&queue1);
			My402ListUnlinkAll(&queue2);
			pthread_mutex_unlock(&my_mutex);
			break;
		}
		elem=My402ListFirst(&queue2);
		pkt = (Packet *)elem->obj;
		My402ListUnlink(&queue2,elem);
		pkt->q2_exit = getinstanttime();
		PrintStat(getinstanttime());
		fprintf(stdout,"p%d begins service at S, time in Q2 = %.3fms\n", pkt->pkt_id,
			(double)(pkt->q2_exit - pkt->q2_enter)/1000);
		pthread_mutex_unlock(&my_mutex);
		avg_pkt_q2 += (pkt->q2_exit - pkt->q2_enter);
		serv_start = getinstanttime();
		usleep(pkt->service_time);
		pkt->sys_exit = serv_end = getinstanttime();
		pkt->serv_time = serv_end - serv_start;
		avg_st += pkt->serv_time;
		avg_pkst += (pkt->sys_exit - pkt->sys_enter);
		pkt_processed++;
		PrintStat(getinstanttime());
		sys_time = pkt->sys_exit - pkt->sys_enter;
		fprintf(stdout,"p%d departs from S, service time = %.3fms, time in system = %.3fms\n", pkt->pkt_id,
			(double)(pkt->serv_time)/1000,
			(double)(sys_time)/1000);
		avg_pkt_s += pkt->serv_time;
		sys_avg += (double)(sys_time)/1000000;
		sys_avg_sq += ((double)(sys_time)/1000000) * ((double)(sys_time)/1000000);

	}
	// Informing token thread to die.
	if(pkt_processed!=0) {
		avg_serv_time = (double)((double)avg_st/(double)(pkt_processed*1000000));
		avg_pkt_sys_time = (double)((double)avg_pkst/(double)(pkt_processed*1000000));
		var_sys = sys_avg/pkt_processed;
		var_sys_sq = sys_avg_sq/pkt_processed;
		std_deviation = sqrt(var_sys_sq - var_sys*var_sys);
	}
	else {
		avg_serv_time = 0;
		avg_pkt_sys_time = 0;
		var_sys = 0;
		var_sys_sq = 0;
		std_deviation = 0;		
	}
	token_die=1;
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	long t1=1, t2=2, t3=3;
	pthread_t arrThread, serThread, tokThread;
	long total_emulation_time;

	// Setting to default values

	lambda = 0.5;
	mu = 0.35;
	r = 1.5;
	P = 3;
	B = 10;
	num_packets = 20;

	token_bucket = 0;
	server_die = 0;
	token_die = 0;

	avg_pkt_q1 = avg_pkt_q2 = avg_pkt_s = 0;
	// Done

	// Initialize Queues
	
	My402ListInit(&queue1);
	My402ListInit(&queue2);

	pthread_mutex_init(&my_mutex,NULL);
	pthread_cond_init(&queue2_cond,NULL);

	parseCommandline(argc,argv);

	tokenarrival = (long)((1/r)*1000000);
	pkts_to_arrive = num_packets;


	pthread_create(&arrThread, NULL, arrivalThread,(void *)t1);
	pthread_create(&tokThread, NULL, tokenThread,(void *)t2);
	pthread_create(&serThread, NULL, serverThread, (void *)t3);

	temulation_start = tastart = ttstart =  getinstanttime();
	PrintStat(getinstanttime());
	fprintf(stdout, "Emulation Begins\n");



	pthread_join(arrThread, NULL);
	pthread_join(tokThread, NULL);
	pthread_join(serThread, NULL);

	temulation_end = getinstanttime();
	PrintStat(getinstanttime());
	fprintf(stdout, "Emulation Ends \n");

	total_emulation_time = temulation_end - temulation_start;
	fprintf(stdout,"Statistics: \n\n");
	fprintf(stdout,"\taverage packet inter arrival time = %.08g sec\n",avg_inter_arrival);
	fprintf(stdout,"\taverage packet service time = %.08g sec\n\n",avg_serv_time);
	fprintf(stdout,"\taverage number of packets in Q1 = %.08g\n",(double)avg_pkt_q1/total_emulation_time);
	fprintf(stdout,"\taverage number of packets in Q2 = %.08g\n",(double)avg_pkt_q2/total_emulation_time);
	fprintf(stdout,"\taverage number of packets at S = %.08g\n\n",(double)avg_pkt_s/total_emulation_time);
	fprintf(stdout,"\taverage time a packet spent in the system = %.08g sec\n",avg_pkt_sys_time);
	fprintf(stdout,"\tstandard deviation for time spent in system = %.08g sec \n\n",std_deviation);
	fprintf(stdout,"\ttoken drop probability = %.08g\n",token_drop_prob);
	fprintf(stdout,"\tpacket drop probability = %.08g\n",pkt_drop_prob);

	pthread_mutex_destroy(&my_mutex);
	pthread_cond_destroy(&queue2_cond);
	pthread_exit(NULL);


}
