//program(2 ) to build a communication between two processes and share messqages via a file used to  write to it and read from it by bith the processes , blocked all the sognals to prevent interuption in the communication except SIGINT to use it as resume signal or acknowledgment signal 
//writen by R SAI SRUJAN on 15th april 11:45

#include<stdio.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

//defenation for self written behaviour for signal (handler)
void handler(int sig){
        printf("\t");
}


int main(int argc,char *argv[])
{	


	//blocking all the signals for the critical signal
	sigset_t mask,oldmask;
	sigemptyset(&mask);
	sigemptyset(&oldmask);
	sigfillset(&mask);
	sigdelset(&mask,SIGINT);
	sigprocmask(SIG_BLOCK,&mask,&oldmask);//blocked
	//checking for valif command line arguments
	if (argc<2){
		printf("usage: %s <filename>\n",argv[0]);
		return 1;
	}
	
	
	//opening the file given in the command line argument
	int fd=open(argv[1],O_RDWR | O_CREAT,0666);

	//checking for successfull opening
	if(fd<0){
		printf("ERROR: ENOENT or EACCESS :failed to open the fil:\n");
		return 0;
	}
	printf("The file is successfully  opend\n\n");
	
	//eting the offset address (cursor position) to start
	lseek(fd,0,SEEK_SET);

	//reading from the file
	char rdbuffer[30]={0};
	int b_read=read(fd,rdbuffer,sizeof(rdbuffer)-1);

	//checking for unsuccessfull reading
	if(b_read<=0){
		printf("read unsuccessful\n");
		return 0;
	}

	printf("Reading successful from the file\n\n");
	
	//converting the char read from the file from char to int to access it and use as pid
	int pid1=atoi(rdbuffer);
	printf("PID of process 1 read from file is : %d\n\n",pid1);
	

	//extracting the pid of this process(self pid)
	char wrbuffer[30];
	int pid=getpid();
	printf("My pid: %d\n\n",pid);
	int length=sprintf(wrbuffer,"%d",pid);

	//writing the pid to the file
	int written=write(fd,wrbuffer,length);
	if(written<0)
		printf("failed to write the self pid to the file\n");
	else
		printf("Success in writing the pid to file\n");


	//sending a signal to process 1 to resume its operation 
	int signal_status=kill(pid1,SIGINT);//sent sifint signal
	
	//checking if the signal is sent or not
	if(signal_status==0)
		printf("Signal sent to process 1\n\n");
	else
		printf("failed to send signal\n");
	 printf("\t-------------------------------------\n");
        void(*ini_bhav)(int);
        ini_bhav=signal(SIGINT,handler);
	
	while(1){
		printf("\t\tWaiting for reply:\n\n");
		pause();
		rdbuffer[30]='0';
		b_read=read(fd,rdbuffer,sizeof(rdbuffer)-1);
		if (b_read>0)
                        rdbuffer[b_read]='\0';
		printf("Received reply:\t%s\n\n",rdbuffer);
	 	printf("\t-------------------------------------\n");
                wrbuffer[30]='0';
                printf("Enter the msg to send:\t");
                fgets(wrbuffer,sizeof(wrbuffer),stdin);
                written=write(fd,wrbuffer,strlen(wrbuffer));
                if(written<=0)
                        printf("MESSAGE NOT WRITTEN\n");
                signal_status=kill(pid1,SIGINT);
                if(signal_status==0)
                        printf("\t\tMESSAGE SENT\n");
                else
                        printf("filed to send message\n");
	 	printf("\t-------------------------------------\n");


	}
	sigprocmask(SIG_UNBLOCK,&mask,&oldmask);//unblocked 
	//closing the file or clearing the fd
	close(fd);
}

