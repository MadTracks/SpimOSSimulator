#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char ** argv){

	char string[1000];

	while(0==0){

		printf("C-Shell >");
		scanf("%s",string);
		if(strcmp(string,"exit")==0){
			return 0;
		}
		
		if(fork()!=0){
			int status;
			waitpid(-1,&status,0);
		}
		else{
			if(open(string,O_RDONLY)>=0){

				char spim[]="spim -f ";
				strcat(spim,string);
				system(spim);
			}
			else{
				printf("No such file or directory.\n");
			}
			exit(-1);
		}		
	}
	return 0;

}
