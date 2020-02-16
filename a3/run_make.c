#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#include "pmake.h"


void run_make(char *target, Rule *rules, int pflag) {
    // TODO
    Rule *rule = rules;
    if (target == NULL){
      target = rules->target;
    }
    // while (rule != NULL){
    //   printf("%s ", rule->target);
    //   rule = rule->next_rule;
    // }
    while ((rule != NULL) && (strcmp(rule->target, target) != 0)){
      rule = rule->next_rule;
    }
    if (rule != NULL){
      Dependency *dep = rule->dependencies;

      while (dep != NULL){
        run_make(dep->rule->target, rules, pflag);
        dep = dep->next_dep;
      }
      int exe = 0;
      struct stat info_dep;
      struct stat info_tar;
      long sec_tar;
      long nano_sec_tar;
      dep = rule->dependencies;

      if (access(target, F_OK) == -1){
        exe = 1;
      } else {
        stat(target, &info_tar);
        sec_tar = info_tar.st_mtim.tv_sec; //chang it to linux
        nano_sec_tar = info_tar.st_mtim.tv_nsec; //chang it to linux
      }

      while (dep != NULL && exe == 0){
        stat(dep->rule->target, &info_dep);
        //st_mtim
        //st_mtimespec
        long sec_dep = info_dep.st_mtim.tv_sec; //chang it to linux
        long nano_sec_dep = info_dep.st_mtim.tv_nsec; //chang it to linux
        if ((sec_dep > sec_tar) || ((sec_dep == sec_tar) && (nano_sec_dep == nano_sec_tar))){
          exe = 1;
        }

        dep = dep->next_dep;
      }
      pid_t result;
      Action *action = rule->actions;
      int par_parent = 0;
      int status;
      while (exe != 0 && action != NULL){
        result = fork();
        if (result > 0 && pflag == 0){ //parent
          if (waitpid(result, &status, 0) == -1){
            perror("wait");
            exit(1);
          }
          if (WEXITSTATUS(status) == 1){
            perror("child");
            exit(1);
          }
        } else if (result > 0 && pflag != 0){
          par_parent = 1;
        } else if (result == 0) { //child
          if (execvp((action->args)[0], action->args) < 0){
            perror("execvp");
            exit(1);
          }
        } else {
          perror("fork");
        }
        action = action->next_act;
      }
      if (pflag == 1 && par_parent == 1){
        while ((result=waitpid(-1, &status, 0)) != -1){
          if (WEXITSTATUS(status) == 1){
            perror("child");
            exit(1);
          }
        }
      }
    }

    return ;
}
