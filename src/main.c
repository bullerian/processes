#include "main.h"

#define ARG_COUNT       2
#define NAME_INDEX      1
#define ENDCOUNT_INDEX  2
#define EXEC            0
#define FORK            1
#define SYSTEM          2

//##### PRIVATE FUNCTION PROTOTYPES #############
static void counterAm(char * name, int max_count);
static void skeleton_daemon(void);
static pid_t create_child(int mode);
//###############################################

char app_path[255];

int main(int argc, char **argv)
{
    pid_t forked_child;
    char * my_name;
    int endcount, status=0;

    // get full path to program
    strcpy(&app_path[0], argv[0]);

    // If program is called with right number of arguments
    // it just counts numbers
    if(ARG_COUNT == --argc){
        // get name and endcount from commands arguments
        my_name = argv[NAME_INDEX];
        endcount = atoi(argv[ENDCOUNT_INDEX]);

        counterAm(my_name, endcount);
        exit(EXIT_SUCCESS);
    }

    // if program was called with less than ARG_COUNT arguments
    printf("The Parent #%d is here!\n", (int)getpid());

    forked_child = create_child(FORK);
    create_child(EXEC);
    create_child(SYSTEM);

    printf("Waiting for child #%d\n", forked_child);
    waitpid(forked_child, &status, 0);
    printf("Child #%d has terminated %snormaly\n", forked_child, (WIFEXITED(status))?"":"un");

    // daemonize my self
    skeleton_daemon();

    sleep(120);
    exit(EXIT_SUCCESS);
}

static pid_t create_child(int mode){
    pid_t child;
    int errsv;

    if (SYSTEM == mode){
      system("./psspawner SYSTEM 20");
      return 0;
      //system("echo SYSTEM 180");
    }

    switch (child=fork()) {
    // error case
    case -1:
        errsv = errno;
        printf("Error #%d has occured", errsv);
        exit(EXIT_FAILURE);
        break;
    // forked code
    case 0:
        // FSM for children
        switch (mode) {
        case EXEC:
            // TODO: replace hardcoded program name
            execl(&app_path[0], "psspawner", "EXECL", "30", (char *)NULL);
            errsv=errno;
            // should not reach here
            printf("Something is really wrong, here is the error #%d", errsv);
            exit(EXIT_FAILURE);
            break;
        case FORK:
            // already forked, so just count
            counterAm("FORK", 10);
            exit(EXIT_SUCCESS);
            break;
        // case SYSTEM:
        //     // TODO: replace hardcoded program name
        //     // and just ignore the retval
        //     system("./psspawner SYSTEM 80");
        //     //system("echo SYSTEM 180");
        //     exit(EXIT_SUCCESS);
        //     break;
        default:
            printf("%d is invalid child creation mode\n", mode);
            exit(EXIT_FAILURE);
            break;
        }

        break;
    // parent may continue. here is the childs pid
    default:
        return child;
        break;
    }

    // shouldn't reach here
    return 1;
}

static void counterAm(char * name, int max_count){
    int counter=0;

    printf("%s child, PID=%d, count_val=%d\n", name, (int)getpid(), max_count);

    while (++counter <= max_count){
        printf("\t(Child %s says): \t%d\n", name, counter);
        sleep(1);
    }
    printf("%s child done counting\n", name);
}

static void skeleton_daemon(void){
    pid_t pid;
    int errsv;

    // fork from parent
    pid = fork();

    // there was some eror
    if (pid < 0){
        errsv = errno;
        printf("First fork error! PID=%d errorn num %d\n", pid, errsv);
        exit(EXIT_FAILURE);
      }
      // forking was succesfull. parent can terminate
      else if (pid > 0){
        exit(EXIT_SUCCESS);
      }

    // set child as leader
    if (setsid() < 0){
        exit(EXIT_SUCCESS);
      }

    // fork again to hide the trails
    pid = fork();

    // second fork is broken
    if (pid < 0){
        exit(EXIT_FAILURE);
      }
      // forking was succesfull. parent can terminate
      else if (pid > 0){
          exit(EXIT_SUCCESS);
        }

      //just in case
      umask(0);
      chdir("/");

    // now it's a daemon!
    // but this message can't be printed, becasue now process has
    // no streams connected
}
