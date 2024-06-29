#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int leaf(){
	char file_name[20];
	sprintf(file_name, "%d.txt", getpid());
	FILE *fp = fopen(file_name, "w");
	srand(getpid());
	fprintf(fp, "%d\n", (rand()%100));
	fclose(fp);
}

int parent(int pid1, int pid2){
	wait(NULL);/*multiple child processes causes race condition, 
	we waited before but when it comes to two child process one 
	of them might end before other child process so it may cause race 
	condition again so we make a double check here to block segmentation error
	*/
	char file_name1[20];
	sprintf(file_name1, "%d.txt", pid1);
	char file_name2[20];
	sprintf(file_name2, "%d.txt", pid2);
	char file_name3[20];
	sprintf(file_name3, "%d.txt", getpid());

	FILE *fp1, *fp2;
	fp1 = fopen(file_name1,"r");
	fp2 = fopen(file_name2,"r");
	int left, right;
	fscanf(fp1,"%d", &left);
	fscanf(fp2,"%d", &right);
	fclose(fp1);
	fclose(fp2);

	FILE *fp3 = fopen(file_name3,"w");
	fprintf(fp3, "%d", left + right);
	fclose(fp3);
}

// if we dont use this function carefully we get sefgmentation error because of the race condition
void create_tree(int height){
	if(height==0){
		return;
	}
	else if (height == 1){
		leaf();
	}
	int pid = fork();
	if(pid>0){
		//parent process
		int pid2 = fork();
		if(pid2>0){
			//parent process
			wait(NULL);// eliminate race condition by waiting child processes

			printf("parent: %d, c1: %d, c2: %d\n", getpid(), pid, pid2);
			fflush(stdout);

			parent(pid, pid2);
		}
		else if(pid2==0){
			//child process 2
			create_tree(--height);
		}
	}
	else if(pid==0){
		//child process 1
		create_tree(--height);
	}
}

int main(){
	create_tree(3);
}