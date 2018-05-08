#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
void readinput(char* BUF, int *e);
void encipher(char* BUF);
void print_res(char* BUF, int *e);
int main(void)
{
int e=1;
char *BUF;
int shmid;
sem_t *sem1;
sem_t *sem2;
sem_t *sem3;
int semValue = 0;
if ((shmid = shmget(ftok("/tmp",1), 100, IPC_CREAT | 0666)) < 0) 
{
perror("shmget");
exit(1);
}
if ((BUF=(char*)shmat(shmid,NULL,0))==NULL) 
{
perror("shmat");
exit(1);
}
sem1 = sem_open ("sharedSem1", O_CREAT | O_EXCL, 0666, semValue);
sem2 = sem_open ("sharedSem2", O_CREAT | O_EXCL, 0666, semValue);
sem3 = sem_open ("sharedSem3", O_CREAT | O_EXCL, 0666, semValue);
sem_unlink("sharedSem1");
sem_unlink("sharedSem2");
sem_unlink("sharedSem3");
if(fork()==0)
{
printf("Proces %d %d utworzony.\n", getpid(), getppid());
while(1)
{
readinput(BUF,&e);
sem_post(sem2);
sem_wait(sem1);
}
}
if(fork()==0)
{
printf("Proces %d %d utworzony.\n", getpid(), getppid());
while(1)
{
sem_wait(sem2);
encipher(BUF);
sem_post(sem3);
}
}
if(fork()==0)
{
printf("Proces %d %d utworzony.\n", getpid(), getppid());
while(1)
{
sem_wait(sem3);
print_res(BUF,&e);
sem_post(sem1);
}
}
pause();
return 0;
}
void readinput(char* BUF, int *e)
{
if(*e)
printf("\nPodaj tekst do zaszyfrowania: \n");
fgets(BUF,sizeof(char)*100,stdin);
if(BUF[strlen(BUF)-1]!='\n')
*e=0;
else
*e=1;
}
void encipher(char* BUF)
{
int i;
int key=5;
char* etxt=malloc(sizeof(char)*100);
for(i=0;i<strlen(BUF);i++)
{
if((BUF[i]>='A') && (BUF[i]<='Z'))
{
etxt[i]=BUF[i]+key;
if(etxt[i]>'Z')
etxt[i]=(etxt[i]%90)+64;
}
else
{
if((BUF[i]>='a') && (BUF[i]<='z'))
{
etxt[i]=BUF[i]+key;
if(etxt[i]>'z')
etxt[i]=(etxt[i]%122)+96;
}
else
etxt[i]=BUF[i];
}
}
memcpy(BUF,etxt,sizeof(char)*strlen(BUF));
}
void print_res(char* BUF, int *e)
{
if(*e)
printf("\nzaszyfrowany tekst:\n");
printf(BUF);
if(BUF[strlen(BUF)-1]!='\n')
*e=0;
else
*e=1;
}