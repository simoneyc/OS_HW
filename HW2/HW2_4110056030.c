#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <readline/readline.h>
#include <readline/history.h>
#define SSLEN 100

FILE * fout;
FILE * fhist;
char buffer[SSLEN];

// handler wait for exit of child process
void SIGCHLD_handler(){
	pid_t pid;
	int status;
}

// bg struct
struct BackgroundJob {
    pid_t pid;
    int running; // 1 = running
    char* name;
};

// get max in two number
int max(int a, int b) {
	if(a > b) {
		return a;
	}
	return b;
}

// check whether str is a number(only digit)
int isNumber(char* str) {
	int i;
	for(i = 0; i < strlen(str); i++) {
		if(isdigit(str[i]) == 0) {
			return 0;
		}
	}
	return 1;
}

// print initial prompt
void shell_init() {
	//printf("\033[0;36m");
	printf("----------Shell Start----------\n");
	//printf("\033[0;0m");
	return;
}
// ~~~


int main(int argc, char **argv) {
	int i;
	char* ps;
	char ss[SSLEN];
	char* ss_ptr = &ss[0];
	char ss2[SSLEN];
	char* ss2_ptr = &ss2[0];
	//
	//char *path = getenv("PATH");
	struct BackgroundJob background_jobs[10];
	int num_jobs = 0;
	//
	char *wave = getcwd(buffer, sizeof(buffer));
	int length = strlen(wave);// "~"'s location offset
    	char *path = getenv("PATH");
	//

	signal(SIGCHLD, SIGCHLD_handler);	// set signal handler
	using_history();
	read_history(".history_file");	// import history from .history
	shell_init();
	while(1) {
		
		// enter command
		char* username = getenv("USER");
		
		//
		char hostname[1000];
		int ps_size = 256; // Adjust the size as needed
		char* ps = (char*)malloc(ps_size); // Allocate memory for ps
		//
		
		char* t = getcwd(buffer, sizeof(buffer)); // get every times env
		
		if (gethostname(hostname, 1000) == 0){
			sprintf(ps, "\033[0;32m%s\033[0;0m@\033[0;35m%s\033[0;0m:\033[0;34m\033[0;36m~%s\033[0;0m$ ", username,hostname,t+length);
		}
		
		ss_ptr = readline(ps);

		if(ss_ptr == NULL) {	// exit myshell
			fflush(stdout);
			write_history(".history_file");
			clear_history();
			fhist = fopen(".history_file", "w");
			fclose(fhist);
			printf("~~~Shell END~~~\n");
			exit(0);
		}
		else if(strlen(ss_ptr) == 0) {
			continue;
		}

		strcpy(ss2_ptr, ss_ptr);

		// count numbers of arguments and modes
		char* arg = NULL;
		char* fout_name = "\0";
		int args_num = 0, read_mode = 0, bg_mode = 0, output_mode = 0;
		//
		char* bg_name;
		//
		// split ss by space
		arg = strtok(ss_ptr, " ");
		if(arg == NULL) {	// ss only space
			continue;
		}
		bg_name = arg;
		add_history(ss2_ptr);	// add into history buffer

		while(arg != NULL) {
			
			if(strcmp(arg, ">") == 0) {
				read_mode = 1;
				output_mode = 1;
				fout_name = strtok(NULL, " ");
			}
			else if(strcmp(arg, ">>") == 0) {
				read_mode = 1;
				output_mode = 2;
				fout_name = strtok(NULL, " ");
			}
			//
			else if(strcmp(arg, "2>") == 0) {
				read_mode = 1;
				output_mode = 3;
				fout_name = strtok(NULL, " ");
			}
			else if(strcmp(arg, "&>") == 0) {
				read_mode = 1;
				output_mode = 4;
				
				fout_name = strtok(NULL, " ");
			}
			//
			else if(strcmp(arg, "&") == 0) {
				read_mode = 1;
				bg_mode = 1;
			}
			if(read_mode == 0) {
				args_num++;
			}
			arg = strtok(NULL, " ");
		}
		// output redirection: overwrite or append
		int fn, copy;
		if(output_mode > 0) {
			if(output_mode == 1) {	// overwrite
				fout = fopen(fout_name, "w");
				fn = fileno(fout);
				copy = dup(1);
				dup2(fn, 1);
			}
			else if(output_mode == 2) {
				fout = fopen(fout_name, "a");
				fn = fileno(fout);
				copy = dup(1);
				dup2(fn, 1);
			}
			else if(output_mode == 3){
				fout = fopen(fout_name, "w");
				fn = fileno(fout);
				copy = dup(1);
				dup2(fn, 2);
			}
			else if(output_mode == 4){
				fout = fopen(fout_name, "w");
				fn = fileno(fout);
				copy = dup(2);
				dup2(fn, 2);
				fout = fopen(fout_name, "a");
				fn = fileno(fout);
				dup2(fn, 1);
			}
		}

		// split argument by space
		char* args[args_num+1];
		char arg_cat[] = " ";
		//
		char *add_path;
		int bg_count = 0;
		//
		arg = strtok(ss2_ptr, " ");
		args[0] = arg;
		for(i = 1; i < args_num; i++) {
			arg = strtok(NULL, " ");
			args[i] = arg;
			strcat(arg_cat, arg);
			if(i != args_num-1) {
				strcat(arg_cat, " ");
			}
		}
		args[args_num] = NULL;

		// case of command
		if(strcmp(args[0], "cd") == 0) {
			char* home_dir = getenv("HOME");
			if(strcmp(args[1], "~") == 0){
				chdir(home_dir);
			}
			else if(strncmp(args[1], "~/",2) == 0){
				chdir(home_dir);
				chdir(args[1]+2);
			}
			else{
				chdir(args[1]);
			}
			
		}
		else if(strcmp(args[0], "pwd") == 0) {
			printf("%s\n", getcwd(buffer, sizeof(buffer)));
		}
		else if(strcmp(args[0], "echo") == 0 && strcmp(args[1], "$PATH") == 0) {
			if(add_path != NULL) {printf("%s%s\n", path,add_path);}
			else {printf("%s\n", path);}
		}
		else if(strcmp(args[0], "echo") == 0) {
			printf("%s\n", arg_cat+1);
		}
		else if(strcmp(args[0], "export") == 0) {
			setenv("PATH", arg_cat, 0);
			add_path = strchr(arg_cat, ':');
		}
		else if(strcmp(args[0], "bg") == 0){
			if(num_jobs==0){
				printf("bash: bg: current: no such job\n");
			}
			else{
		        	if (background_jobs[num_jobs].running == 0) {
		        		printf("bash: bg: job has terminated\n");
		            		printf("[%d]+ Done\t\t\t\t%s\n", num_jobs,background_jobs[num_jobs-1].name);
		        	}
		        	else{
		        		for(int j = 0; j < 10 ;j++){
		        			if(background_jobs[j].running == 1) {
		        				bg_count++;
	 					}
		        		}
		        		printf("bash: bg: job %d already in background\n",bg_count);
		        	}
		    		num_jobs = 0;
		    	}
		}
		else if(strcmp(args[0], "exit") == 0) {
			fflush(stdout);
			write_history(".history_file");
			clear_history();
			fhist = fopen(".history_file", "w");
			fclose(fhist);
			exit(0);
		}
		else if(strcmp(args[0], "history") == 0) {
			// get history buffer
			HIST_ENTRY **hist_list = history_list();
			if(hist_list) {
				printf("Total history in buffer: %d\n", history_length);
				if(args_num == 1) {	// all history
					for(i = 0; hist_list[i]; i++) {
						printf("%4d: %s\n", i+history_base, hist_list[i]->line);
					}
				}
				else {
					if(isNumber(args[1])) {	// top number history
						int hist_offset = atoi(args[1]);
						for(i = max(history_length-hist_offset, 0); i < history_length; i++) {
							printf("%4d: %s\n", i+history_base, hist_list[i]->line);
						}					
					}
					else {
						if(strcmp(args[1], "-clear") == 0) {	// clear history
							clear_history();
							fhist = fopen(".history_file", "w");
							fclose(fhist);
						}
						else {
							printf("[ERROR] history: Invalid operand '%s'.\n", args[1]);
						}
					}
				}
			}
			else {
				printf("[ERROR] No history now!\n");
			} 
		}
		else {	// external command
			pid_t pid;
			pid = fork();
			if(pid < 0) {
				printf("[ERROR] Fork failed.\n");
				exit(-1);
			}
			else if (pid == 0) {	// child process
				if(execvp(args[0], args) == -1) {
					printf("[ERROR] Command '%s' not found.\n", args[0]);
				}
				exit(0);
			}
			else {		// parent process
				if(bg_mode == 0) {	// not background mode
					int status;
                    			waitpid(pid, &status, 0);
				}
				else{
					if (num_jobs < 10) {
						background_jobs[num_jobs].pid = pid;
						background_jobs[num_jobs].running = 1; //1 = running
						background_jobs[num_jobs].name = bg_name;
						num_jobs++;
						printf("[%d]%d\n", num_jobs, pid);
						if(num_jobs > 1){
							printf("[%d]  Done\t\t\t\t%s\n",num_jobs-1,background_jobs[i].name);
						}
						
                    			}
				}
			}
		}
		
		// check bg task finished or not
		for (int i = 0; i < num_jobs; i++) {
            		int status;
            		pid_t result = waitpid(background_jobs[i].pid, &status, WNOHANG);
            		if (result > 0) {
                		background_jobs[i].running = 0; // label as finished
            		}
        	}		

		// clear stdout buffer
		fflush(stdout);
		// recover stdout
		if(output_mode == 1 || output_mode == 2 || output_mode == 3 || output_mode == 4)  {
			dup2(copy, 1);
			fclose(fout);
		}
	}
}
