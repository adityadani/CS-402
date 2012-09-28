#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char **argv)
{
	int i=1, ret;
	double lambda, mu, r; 
	int B, p, num_packets;
	FILE *tfile;
	char test;

	// Setting to default values

	lambda = 0.5;
	mu = 0.35;
	r = 1.5;
	p = 3;
	B = 10;
	num_packets = 20;

	// Done

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
					if (ret == 0)
						r = strtod(argv[i],NULL);
					else {
						fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
						Usage();
						exit(1);
					}
					break;

				case 'n':
					ret = validate_argument(argv[i]);
					if (ret == 0)
						num_packets = (int)strtod(argv[i],NULL);
					else {
						fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
						Usage();
						exit(1);
					}
					break;

				case 'P':
					ret = validate_argument(argv[i]);
					if (ret == 0)
						p = (int)strtod(argv[i],NULL);
					else {
						fprintf(stderr, "\nInvalid Argument : %s %s", argv[i-1], argv[i]);
						Usage();
						exit(1);
					}
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

	fprintf(stdout, "\nEmulation Parameters: \n\tlambda = %f",lambda);
	fprintf(stdout, "\n\tmu = %f\n\tr = %f\n\tB = %d\n\tp = %d\n\tnumber to arrive = %d\n",mu,r,B,p,num_packets);
	return 0;
}
