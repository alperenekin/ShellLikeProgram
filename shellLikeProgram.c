#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct tsk_node_s { //necessary for keeping history in queue
	char* value; 
	struct tsk_node_s* next;
};
typedef struct tsk_node_s Node2;

Node2 *head= NULL;
Node2 * tail=NULL;
char **pipe1,**pipe2;

//list of functions
int cd(char **args);
int help(char **args);
int bye(char **args);
int dir(char ** args);
int history(char ** args);
int findloc(char ** args);
char **parsePipe(char** args);
char **parsePipe2(char** args);
int execPipe(char** parsed, char** parsedpipe);

void Task_enqueue(char* value){ //inserts a new task into task queue with linked list.)
	if(head==NULL){ //If the queue is empty,insert to beginning
		head = malloc(sizeof(Node2));
		head ->value =value;
		head->next=NULL;
		tail = head; //it means one element queue
	}
	else{
		tail->next = malloc(sizeof(Node2)); //adds end of the queue
		tail->next->value =value;
		tail = tail->next; //move the tail
		tail -> next =NULL;
	}

}

int cd(char **args)//builtin command for changing directory
{
  if (args[1] == NULL) { //check for directory element
    chdir(getenv("HOME"));//return the directory of "HOME"
  } 
  else {
    if (chdir(args[1]) != 0) { // If directory is not valid
      perror("Error");
    }
  }
  return 1; //always return 1 to continue executing
}

#define PATH_MAX 256
int dir(char **args){
	char cwd[PATH_MAX];
	   if (getcwd(cwd, sizeof(cwd)) != NULL) { //get the pathname of current working directory
	       printf("%s\n", cwd);
	   } else {
	       perror("getcwd() error");
	   }
	   return 1; //always return 1 to continue executing
}

int bye(char **args)
{
	
  return 0;//always return 0 to terminate executing
}

int findloc(char **args){

char src[500],dest[500],src2[500],str[500];
char *path,*cut_path;
path =getenv("PATH"); //storing list of paths
strcpy(str,path); //keep path list in str variable(necessary for keeping path variable same)
cut_path=strtok(str,":");// divide it to real paths
strcpy(src,"/"); // "/" is necessary for defining the path
strcpy(dest,args[1]);// args[1] is necassary for path for example /bin/ls
strcat(src,dest); //put src and dest together like /ls 
while(cut_path!=NULL){
	strcpy(src2,cut_path);//by the help of while loop we will loop around every possible path
	strcat(src2,src); //put all things together 
	if(access(src2,X_OK)==0){ //by using access we can search wheter the executable file in the location or not .
		printf("%s \n",src2);
		return 1; //always return 1 to continue executing
	}
	cut_path=strtok(NULL,":");// keep dividing the path
	
   }
printf("file is not found \n");
return 1; //always return 1 to continue executing
}
int history(char **args){ // We kept commands in the queue and print it from there.
	Node2* temp =head;
	int i =1;
	while(temp!=NULL){
		printf("[%d] %s\n",i,temp->value );
		temp=temp->next;	
		i++;	
		}
	return 1;//always return 1 to continue executing
}

int launch(char **args)
{
  pid_t pid;
  int i=0;
  char* temp;
  pid = fork();
	while(args[i]!=NULL){
		temp=args[i]; // with this loop we can hold the last string of input to check if it is "&" or not
		i++;		
	}
if (strcmp(temp,"&")==0){// Since & is not command we have to change it to not to execute it
	args[i-1]='\0';
	}

  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) { //If it is not built in function that defined from us ,execute it in child
      perror("Error");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("Error");
  } else {
    // Parent process
	if (strcmp(temp,"&")==0){ //if user input contains "&" dont wait for the child
	}
        else{
		wait(NULL);// else wait for the child till it  terminates the process.
	}
   }

  return 1;//always return 1 to continue executing
}

int execute(char **args)
{
  int i,a;
  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
  else if(strcmp(args[0],"cd")==0){ //if the command is cd call our cd function
	return cd(args);
  }
  else if(strcmp(args[0],"bye")==0){ //if the command is bye call our bye function
	return bye(args);
  }
  else if(strcmp(args[0],"dir")==0){//if the command is dir call our dir function
	return dir(args);
  } 
  else if(strcmp(args[0],"history")==0){//if the command is history call our history function
	return history(args);
  }
  else if(strcmp(args[0],"findloc")==0){//if the command is findloc call our findloc function
	return findloc(args);
  }
  else{
  	return launch(args);//If it is not one of our commands
  }
}

#define LSH_RL_BUFSIZE 1024
char *read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  char *buffer = malloc(sizeof(char) * bufsize);

  if (!buffer) {
    fprintf(stderr, "Error: allocation error\n");
    exit(EXIT_FAILURE);
  }
  while (1) {
    fgets(buffer,50,stdin);// Take input and store in buffer
    return buffer; 
   }
}


#define LSH_TOK_BUFSIZE 64
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*)); //creates a string array
  char *token;
  int str;
  if (!tokens) {
    fprintf(stderr, "Error: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, " "); // split our input by delimeter space
	
  while (token != NULL) { //when take the input from user it takes as "string/n" so we need to change the "\n" with "\0" to correct it as "string"
     str=strlen(token);
	if(str>1 && token[str-1]=='\n'){
		token[str-1]='\0';
	}
    tokens[position] = token; // store the strings in an array
    position++;
    token = strtok(NULL, " ");// Keep splitting
  }
  return tokens;
}
int execPipe(char** parsed, char** parsedpipe) 
{ 
   pid_t pid1, pid2;
   int pipefd[2];
   int status;

   // Create a pipe.
   pipe(pipefd);
   // Create our first process.
   pid1 = fork();
   if (pid1 == 0) { // executes the left side of the pipe
      //Use  stdout  to the write end of the pipe and close the read end of
      dup2(pipefd[1], STDOUT_FILENO);
      close(pipefd[0]);
     //start executing the left side
      execvp(parsed[0],parsed);
      perror("exec");
      return 1;
   }
   // Create our second process.
   pid2 = fork();
   if (pid2 == 0) {// executes the right side of the pipe
      dup2(pipefd[0], STDIN_FILENO);
      close(pipefd[1]);
      //start executing the right side
      execvp(parsedpipe[0], parsedpipe);
      perror("exec");
      return 1;
   }
   // Close both ends of the pipe. 
   close(pipefd[0]);
   close(pipefd[1]);
   // Wait for everything to finish and exit.
   waitpid(pid1,&status,0);
   waitpid(pid2,&status,0);
   return 1;
}
    

void loop(void)
{
  char *line;
  char **args;
  int status;
  int i=0;
  int flag=0;
  do {	
    flag=0;//To keep going the loop
    printf("myshell> ");
    line = read_line(); //Takes the input
    Task_enqueue(line); //To store it in queue
    args = split_line(line); //To put splittet strings in array
    while(args[i]!=NULL){ 
	if(strcmp(args[i],"|")==0){ //keep looping till find the pipe
		pipe1=parsePipe(args); //store the left side of the pipe
  		pipe2=parsePipe2(args); // STore the right side of the pipe
		flag=1; // change flag to not go into execute function again
		status=execPipe(pipe1,pipe2);
		break;
    	}
    i++;
    }
    if(flag==0){//To execute the execute function
	status = execute(args);
    }
    
  } while (status);// keep looping till bye function
}


char **parsePipe(char** args) //parse the left side of the pipe
{ 
char **piped1=malloc(1024 * sizeof(char*));
int i=0;
	while(strcmp(args[i],"|")!=0){
		piped1[i]=args[i];
		i++;
	}
return piped1;
} 
char **parsePipe2(char** args) 
{ 
char **piped2=malloc(1024 * sizeof(char*)); //parse the right side of the pipe
int i=0;
int j=0;
	while(strcmp(args[i],"|")!=0){
		i++;
	}
	i++;
	while(args[i]!= NULL){
		piped2[j]=args[i];
		j++;
		i++;
	}
return piped2;
} 


int main(int argc, char **argv)
{
loop();
  return EXIT_SUCCESS;
}
