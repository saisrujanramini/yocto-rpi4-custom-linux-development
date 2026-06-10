//program to build communication between two process via a file used to write into it and read fromm it by both processes, blocked all the signals to preven tthe communication not to be interupted except the SIGINT signal used as the triggering/acknowledgement / resume signal 
//writte by R SAI SRUJAN on 15th april 11:45
//uppdated on 16th april

#include<stdio.h>
#include<fcntl.h>
#include<signal.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>



//defenation for self written behaviour for signal (handler)
void handler(int sig){
        printf("\n");
}



int main(int argc,char *argv[])
{ 	
	


	// blocking all the signals for the critical section
	sigset_t mask,oldmask;
	sigemptyset(&mask);
	sigemptyset(&oldmask);
	sigfillset(&mask);//filled all the signals into the mask
	sigdelset(&mask,SIGINT);//deleted the SIGINT signal from the mask to use it in communication
	sigprocmask(SIG_BLOCK,&mask,&oldmask);// blocked 
	int pid2=0;	

	//checking for valif command line arguments
	if(argc<2){
		printf("usage: %s <filename>\n",argv[0]);
		return 1;
	}


	//opening the file given as command line argument
	int fd=open(argv[1],O_RDWR|O_TRUNC | O_CREAT,0666);
	


	//checking for success or failure in opening the file
	if(fd<0){
		printf("ERROR : ENOENT or EACCESS :failed to open the file:\n");
		return 0;
	}
	
	
	
	//extracting the pid of ths process (self pid)
	char wrbuffer[30]={0};
	int pid=getpid();
	printf("My pid : %d\n\n",pid);
	int length=sprintf(wrbuffer,"%d",pid);
	

	//writing the pid to the file
	int written =write(fd,wrbuffer,length);
	
	//checking for successfull or failure writing
	if(written<0)
		printf("Failed to write into the file\n");
	else
		printf("Success in Writing the PID to the file\n\n");
	
	
	
	//changing the behaviour of the signal SIGINT
	void(*ini_bhav)(int);
        ini_bhav=signal(SIGINT,handler);

	
	
	// pausing the process untilll it receives the signal for other process
	printf("Process paused \nCan be resumed when received a signal\n\n");

	pause();
	
	
	
	//reading the other process's pid from the file
	char rdbuffer[30]={0};
	int b_read=read(fd,rdbuffer,sizeof(rdbuffer)-1);

	
	
	// checking for successful reding
	if(b_read>0){
		rdbuffer[b_read]='\0';
		printf("Success in reading the process 2 PID:  %s\n\n",rdbuffer);
		pid2=atoi(rdbuffer);
		//printf("pid of process2 :  %d\n\n",pid2);
	}
	else
		printf("failed to read\n");
	printf("\t-------------------------------------\n");

	int sig_stat=0;
	while(1){
		wrbuffer[30]='0';
		printf("Enter the msg to send:\t");
		fgets(wrbuffer,sizeof(wrbuffer),stdin);
		written=write(fd,wrbuffer,strlen(wrbuffer));
		if(written<=0)
			printf("MESSAGE NOT WRITTEN\n");
		sig_stat=kill(pid2,SIGINT);
		if(sig_stat==0)
			printf("\t\tMESSAGE SENT\n\n");
		else
			printf("filed to send message\n");
		printf("\t-------------------------------------\n");
		printf("\t\tWaiting for the reply:\n");
		pause();
		rdbuffer[30]='0';
		b_read=read(fd,rdbuffer,sizeof(rdbuffer)-1);
		if (b_read>0)
			rdbuffer[b_read]='\0';
		printf("Received reply :\t%s\n\n",rdbuffer);
		printf("\t-------------------------------------\n");
	}
	
	sigprocmask(SIG_UNBLOCK,&mask,&oldmask);//unblocking 


	//closing the file or clearing the fd
	close(fd);

}
