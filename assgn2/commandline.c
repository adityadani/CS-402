#include <ctype.h>
#include "packet.h"

void Usage()
{
	fprintf(stderr,"\n./warmup2 [-lambda lambda] [-mu mu] [-r r]"); 
	fprintf(stderr,"[-B B] [-P P] [-n num] [-t tsfile] \n");
}

int validate_argument(char *buf)
{
	int i;
	for (i=0; i<strlen(buf) ; i++) {
		if(!isdigit(buf[i])) {
			if(buf[i] != '.')
				return 1;
		}
	}
	return 0;
}


void parseCommandline(int argc, char **argv) 
{
	int i,ret=0,my_error_num;
	char test;
	char buf[80];
	emulation_mode = 'd';
	for(i=1 ; i < argc ; i++) {
		if (strncmp(argv[i],"-lambda",strlen("-lambda")) == 0) {
			i++;
			if(i==argc) {
				fprintf(stderr, "\nInvalid No. of Arguments");
				Usage();
				exit(1);
			}
			ret = validate_argument(argv[i]);
			if (ret == 0)
				lambda = strtod(argv[i],NULL);
			else {
				fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
				Usage();
				exit(1);
			}
		}
		else if (strncmp(argv[i],"-mu",strlen("-mu")) == 0){
			i++;
			if(i==argc) {
				fprintf(stderr, "\nInvalid No. of Arguments");
				Usage();
				exit(1);
			}
			ret = validate_argument(argv[i]);
			if(ret == 0)
				mu = strtod(argv[i],NULL);
			else {
				fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
				Usage();
				exit(1);
			}
		}
		else {
			if (strlen(argv[i]) == 2 && argv[i][0] == '-') {
				test = argv[i][1];
				i++;
				if(i==argc) {
					fprintf(stderr, "\nInvalid No. of Arguments");
					Usage();
					exit(1);
				}
				switch(test) {
				case 'B':
					ret = validate_argument(argv[i]);
					if (ret == 0)
						B = (int)strtod(argv[i],NULL);
					else {
						fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
						Usage();
						exit(1);
					}
					break;

				case 'r':
					ret = validate_argument(argv[i]);
					if (ret == 0) {
						r = strtod(argv[i],NULL);
						if((double)(1/r) > 10)
							r=0.1;
						tokenarrival = (1/r)*1000000;
					}
					else {
						fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
						Usage();
						exit(1);
					}
					break;

				case 'n':
					ret = validate_argument(argv[i]);
					if (ret == 0) {
						num_packets = (int)strtod(argv[i],NULL);
						pkts_to_arrive = num_packets;
					}
					else {
						fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
						Usage();
						exit(1);
					}
					break;

				case 'P':
					ret = validate_argument(argv[i]);
					if (ret == 0)
						P = (int)strtod(argv[i],NULL);
					else {
						fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
						Usage();
						exit(1);
					}
					break;
					
				case 't':
					fp = fopen(argv[i],"r");
					my_error_num=errno;
					if(fp==NULL) {
						fprintf(stderr,"\nError in opening file: %s ",argv[2]);
						fprintf(stderr,"\nError : %s\n",strerror(my_error_num));
						exit(1);	
					}
					if (fgets(buf, sizeof(buf), fp)!=NULL) {
						if(buf[strlen(buf) - 1] == '\n')
							buf[strlen(buf) - 1] = '\0';
						ret = validate_argument(buf);
						if (ret == 0)
							num_packets = (int)strtod(buf,NULL);
						else {
							fprintf(stderr, "\nError in tfile\n");
							exit(1);
						}
						
					}
					emulation_mode = 't';
					fprintf(stdout, "\nEmulation Parameters: \n\ttfile: %s",argv[i]);
					break;
				}
			}
			else {
				fprintf(stderr, "\nInvalid Argument : %s",argv[i]);
				Usage();	
				exit(1);
			}
		}
		
	}
	if(emulation_mode == 't') {
		fprintf(stdout, "\n\tr = %f\n\tB = %d\n\tnumber_to_arrive = %d\n",r,B,num_packets);
	}
	else {
		fprintf(stdout, "\nEmulation Parameters: \n\tlambda = %f",lambda);
		fprintf(stdout, "\n\tmu = %f\n\tr = %f\n\tB = %d\n\tp = %d\n\tnumber to arrive = %d\n",mu,r,B,P,num_packets);
	}

}

