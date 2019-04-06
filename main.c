/* CSci4061 F2018 Assignment 1
* login: fanxx495
* date: 10/05/18
* name: Yuanhao ruan, Zhenyu Fan, Yiping Ren (for partner(s))
* id: 5085043, 5295805, 5070041 */

// This is the main file for the code
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

/*-------------------------------------------------------HELPER FUNCTIONS PROTOTYPES---------------------------------*/
void show_error_message(char * ExecName);
//Write your functions prototypes here
void show_targets(target_t targets[], int nTargetCount);
/*-------------------------------------------------------END OF HELPER FUNCTIONS PROTOTYPES--------------------------*/


/*-------------------------------------------------------HELPER FUNCTIONS--------------------------------------------*/

//This is the function for writing an error to the stream
//It prints the same message in all the cases
void show_error_message(char * ExecName)
{
	fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", ExecName);
	fprintf(stderr, "-f FILE\t\tRead FILE as a makefile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	exit(0);
}

void build(char*TargetName,target_t targets[],int nTargetCount) {
  int index=find_target(TargetName,targets,nTargetCount);     // find the index of target we are compiling


  if (index!=-1) {                          // target exist in target list
    if(targets[index].Status==FINISHED) {   //check the status
      return;
    }

    int i;
    for (i=0;i<targets[index].DependencyCount;i++) {          // recursively build all the dependency of the target
      char* dependency=targets[index].DependencyNames[i];
      int id=find_target(dependency,targets,nTargetCount);    //find whether or not the dependency as a target exists in makefile

      if (id!=-1) {                                           //dependency already exists      
        build(dependency,targets,nTargetCount);

        if(does_file_exist(TargetName)==0&&does_file_exist(dependency)==0) {   
          // check the modification time to determine whether or not it needs recompilation 
          int time=compare_modification_time(TargetName,dependency);
          if(time==-1) {
            printf("%s or/and %s does not exist\n",TargetName,dependency );
            exit(-1);
          }else if(time==2) {
            targets[index].Status=NEEDS_BUILDING;
            break;
          }else {
            targets[index].Status=FINISHED;
          }
        }
      }else {                                   //the dependency is not a target in makefile
          if(does_file_exist(dependency)==-1) {
            printf("%s does not exist, cannot build %s\n",dependency,TargetName );
            exit(-1);
          }else {
            if(does_file_exist(TargetName)==-1) {
              break;
            //when both dependency and target exists
            }else {
              //compare modification time
              int time=compare_modification_time(TargetName,dependency);
              if(time==-1) {
                printf("%s or/and %s does not exist\n",TargetName,dependency );
                exit(-1);
              }else if (time==0) {
                return;
              }else if(time==1) {
                return;
              }else {
                targets[index].Status=NEEDS_BUILDING;
                break;
              }
            }
          }
        }
      }
    }else {
      if(does_file_exist(TargetName)==-1) {
        printf("%s is not a file or a target\n",TargetName );
        exit(-1);
      }else {
        printf("%s is a file\n",TargetName );
        return;
      }
    }
    //execute the command based on targets status
    if(targets[index].Status==NEEDS_BUILDING) {
        pid_t pid=fork();
        if(pid>0) {
            //parent process
            int wstatus;
            wait(&wstatus);
            if (WEXITSTATUS(wstatus) != 0) {        //child exited with erro
                printf("child exited with error code=%d\n", WEXITSTATUS(wstatus));
                exit(-1);
            }

        }else if(pid==0) {
            //child process
            printf("%s\n", targets[index].Command);
            char * args[1024];
            parse_into_tokens(targets[index].Command, args, " ");
            execvp(args[0], args);
            perror("exec failed\n");      //error checking
            exit(-1);

        }else {
            //fork failure
            perror("fork problems");
            exit(-1);
        }
    }
    targets[index].Status=FINISHED;   //update target's status
  }




//Phase1: Warmup phase for parsing the structure here. Do it as per the PDF (Writeup)
void show_targets(target_t targets[], int nTargetCount)
{
    int i;
    int j;
    for (i=0;i<nTargetCount;i++) {
        printf("TargetName:%s\n",targets[i].TargetName);
        printf("DependencyCount:%d\n",targets[i].DependencyCount);
        printf("DependencyNames:");
        for (j=0;j<targets[i].DependencyCount;j++) {
            printf("%s ",targets[i].DependencyNames[j]);
        }
        printf("\n");
        printf("Command:%s\n",targets[i].Command);
    }
	
}

/*-------------------------------------------------------END OF HELPER FUNCTIONS-------------------------------------*/


/*-------------------------------------------------------MAIN PROGRAM------------------------------------------------*/
//Main commencement
int main(int argc, char *argv[])
{
  target_t targets[MAX_NODES];
  int nTargetCount = 0;

  /* Variables you'll want to use */
  char Makefile[64] = "Makefile";
  char TargetName[64];

  /* Declarations for getopt. For better understanding of the function use the man command i.e. "man getopt" */
  extern int optind;   		// It is the index of the next element of the argv[] that is going to be processed
  extern char * optarg;		// It points to the option argument
  int ch;
  char *format = "f:h";
  char *temp;

  //Getopt function is used to access the command line arguments. However there can be arguments which may or may not need the parameters after the command
  //Example -f <filename> needs a finename, and therefore we need to give a colon after that sort of argument
  //Ex. f: for h there won't be any argument hence we are not going to do the same for h, hence "f:h"
  while((ch = getopt(argc, argv, format)) != -1)
  {
	  switch(ch)
	  {
	  	  case 'f':
	  		  temp = strdup(optarg);
	  		  strcpy(Makefile, temp);  // here the strdup returns a string and that is later copied using the strcpy
	  		  free(temp);	//need to manually free the pointer
	  		  break;

	  	  case 'h':
	  	  default:
	  		  show_error_message(argv[0]);
	  		  exit(1);
	  }

  }

  argc -= optind;
  if(argc > 1)   //Means that we are giving more than 1 target which is not accepted
  {
	  show_error_message(argv[0]);
	  return -1;   //This line is not needed
  }

  /* Init Targets */
  memset(targets, 0, sizeof(targets));   //initialize all the nodes first, just to avoid the valgrind checks

  /* Parse graph file or die, This is the main function to perform the toplogical sort and hence populate the structure */
  if((nTargetCount = parse(Makefile, targets)) == -1)  //here the parser returns the starting address of the array of the structure. Here we gave the makefile and then it just does the parsing of the makefile and then it has created array of the nodes
	  return -1;


  //Phase1: Warmup-----------------------------------------------------------------------------------------------------
  //Parse the structure elements and print them as mentioned in the Project Writeup
  /* Comment out the following line before Phase2 */
  //show_targets(targets, nTargetCount);
  //End of Warmup------------------------------------------------------------------------------------------------------
   
  /*
   * Set Targetname
   * If target is not set, set it to default (first target from makefile)
   */
  if(argc == 1)
    strcpy(TargetName, argv[optind]);    // here we have the given target, this acts as a method to begin the building
  else
      strcpy(TargetName, targets[0].TargetName);  // default part is the first target

  /*
   * Now, the file has been parsed and the targets have been named.
   * You'll now want to check all dependencies (whether they are
   * available targets or files) and then execute the target that
   * was specified on the command line, along with their dependencies,
   * etc. Else if no target is mentioned then build the first target
   * found in Makefile.
   */
	
  //Phase2: Begins ----------------------------------------------------------------------------------------------------
  /*Your code begins here*/
  int k;
  for (k=0;k<nTargetCount;k++) {
    targets[k].Status=NEEDS_BUILDING;             //reset the status of targets
  }
  build(TargetName, targets, nTargetCount);
  
  
  /*End of your code*/
  //End of Phase2------------------------------------------------------------------------------------------------------

  return 0;
}
/*-------------------------------------------------------END OF MAIN PROGRAM------------------------------------------*/
