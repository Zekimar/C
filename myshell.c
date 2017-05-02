#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
/* TODO: add extra credit? 
	-redirection
	open(char*, mode) returns an int
	dup2 (int, 0/1) takes return value of open
	close(int) also takes return value of open
	
	-and/or 
	make code populating args into function
	when && or || is found as token, switch to new args array
	????? dunno how to make it do the next one
*/
typedef struct{
	char* stack[4];
	int size;
} Stack;
Stack s;

void push(char* c){
	if (s.size < 3){
		s.size += 1;
		s.stack[s.size] = c;
	}else{
		fprintf(stderr, "ERROR: stack is full.\n");
	}
}

char* pop(){
	char* c = NULL;
	if (s.size > -1){
		c = s.stack[s.size];
		s.stack[s.size] = NULL;
		s.size -= 1;
		return c;
	}else{
		fprintf(stderr, "ERROR: stack is empty. \n");
		return c;
	}
}

void run_program(char* args[], char* redirect_in, char* redirect_out){
	signal(SIGINT, SIG_DFL);
	if (fork() == 0){
		if (redirect_in != NULL){
			freopen(redirect_in, "r", stdin);
		}
		if (redirect_out != NULL){
			freopen(redirect_out, "w", stdout);
		}
		execvp(args[0], &args[0]);
		perror("ERROR");
		exit(1);
	}else{
		//printf("2\n");
		signal(SIGINT, SIG_IGN);
		int stat;
		waitpid(-1, &stat, 0);
		if (stat == -1){
			perror("ERROR: ");
		}else if (stat == 0){
			fprintf(stderr, "exited successfully.\n");
		}else if (stat != 0){
			if (stat == SIGSEGV || stat == SIGBUS || stat == SIGILL || stat == SIGFPE || stat == SIGABRT || SIGINT || stat == SIGTERM || stat == SIGKILL){
				fprintf(stderr, "terminated due to signal %s\n", strsignal(WTERMSIG(stat)));
			}else{
				fprintf(stderr, "exited with code %d\n", stat);
			}
		}else{
			fprintf(stderr, "exited some other way\n");
		}
	}
}

void change_directory(char* args[]){
	if (!strcmp(args[0], "pushd")){
		char* cwd;
		cwd = getcwd(NULL, 0);
		if (cwd != NULL){
			fprintf(stderr, "%s\n", cwd);
			push(cwd);
		}
		chdir(args[1]);
	}else if (!strcmp(args[0], "popd")){
		chdir(pop());
	}else{
		chdir(args[1]);
	}
}

void exit_shell(char* args[]){
	int status = 0;
	if (args[1] != NULL){
		status = atoi(args[1]);
	}
	exit(status);	
}

int main(){
	s.size = -1;
	while(1){
		signal(SIGINT, SIG_IGN);
		printf("myshell> ");
		char str[500];
		char* args[50];
		char *token; 
		char* redirect_in = NULL;
		char* redirect_out = NULL;
		fgets(str, 500, stdin);
		token = strtok(str, " \t\n()|&;");
		int i = 0;
		while (token != NULL){
			if (redirect_in != NULL && redirect_out != NULL){
				break;
			}
			if (strcmp(token, "<") == 0){
				if (redirect_in == NULL){
					token = strtok(NULL, " \t\n()|&;");
					redirect_in = token;
					token = strtok(NULL, " \t\n()|&;");
				}else{
					fprintf(stderr, "ERROR: multiple redirections to stdin.\n");
				}
			}else if (strcmp(token, ">") == 0){
				if (redirect_out == NULL){
					token = strtok(NULL, " \t\n()|&;");
					redirect_out = token;
					token = strtok(NULL, " \t\n()|&;");
				}else{
					fprintf(stderr, "ERROR: multiple redirections to stdout.\n");
				}
			}else{
				args[i] = token;
				token = strtok(NULL, " \t\n()|&;");
				i++;
			}
		}
		args[i] = NULL;
		if (strcmp(args[0], "exit") == 0){
			exit_shell(args);
		}else if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "pushd") == 0 || strcmp(args[0], "popd") == 0){
			change_directory(args);
		}else{
			run_program(args, redirect_in, redirect_out); 
		}
	}
}
