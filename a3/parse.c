#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"


/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;

    while(cur != NULL) {
        if(cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);

            // Print dependencies
            Dependency *dep = cur->dependencies;
            while(dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");

            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}

/* Create the rules data structure and return it.
   Figure out what to do with each line from the open file fp
     - If a line starts with a tab it is an action line for the current rule
     - If a line starts with a word character it is a target line, and we will
       create a new rule
     - If a line starts with a '#' or '\n' it is a comment or a blank line
       and should be ignored.
     - If a line only has space characters ('', '\t', '\n') in it, it should be
       ignored.
 */
Rule *parse_file(FILE *fp) {

    // TODO
    struct rule_node *rn = NULL;
    Rule *last_scanned = NULL;
    char nextline[MAXLINE];
    while (fgets(nextline, MAXLINE, fp) != NULL){
      nextline[strlen(nextline) - 1] = '\0';
      if ((nextline[0] == '\t') && (is_comment_or_empty(nextline) == 0)){ //action line
        char *action_line = nextline + 1;
        // struct rule_node *current_rule = rn;
        // while ((current_rule->next_rule) != NULL){
        //   current_rule = current_rule->next_rule;
        // }
        struct action_node *curr_an = last_scanned->actions;
        struct action_node *new_action = malloc(sizeof(struct action_node));
        if (curr_an == NULL){
          curr_an = new_action;
          last_scanned->actions = curr_an;
        } else{
          while ((curr_an->next_act) != NULL){
            curr_an = curr_an->next_act;
          }
          curr_an->next_act = new_action;
        }
        new_action->next_act = NULL;
        new_action->args = build_args(action_line);
      } else if ((nextline[0] >= 'a' && nextline[0] <= 'z') || (nextline[0] >= 'A' && nextline[0] <= 'Z')){//target line
        struct rule_node *search_rules = rn;
        char *nline = strchr(nextline, ':');
        nextline[nline - nextline - 1] = '\0';
        while (search_rules != NULL && (strcmp(search_rules->target, nextline) != 0)){
          search_rules = search_rules->next_rule;
        }
        last_scanned = search_rules;
        if (search_rules == NULL){
          struct rule_node *current_rn = rn;

          struct rule_node *new_rn = malloc(sizeof(struct rule_node));
          if (current_rn == NULL){
            current_rn = new_rn;
            rn = current_rn;
          } else{
            while ((current_rn->next_rule) != NULL){
              current_rn = current_rn->next_rule;
            }
          }
          current_rn->next_rule = new_rn;
          new_rn->actions = NULL;
          new_rn->next_rule = NULL;
          new_rn->dependencies = NULL;
          new_rn->target = malloc(sizeof(char) * (strlen(nextline) + 1));
          strcpy(new_rn->target, nextline);
          (new_rn->target)[strlen(nextline)] = '\0';
          last_scanned = new_rn;
        }

        //dependencies
        nline = strtok(nline + 1, " ");
        struct rule_node *curr_rule = rn;
        while (nline != NULL){
          while (strcmp(curr_rule->target, nextline) != 0){
                curr_rule = curr_rule->next_rule;
              }
          struct dep_node *current_dn = curr_rule->dependencies;
          struct dep_node *new_dn = malloc(sizeof(struct dep_node));
          if (current_dn == NULL){
            current_dn = new_dn;
            curr_rule->dependencies = current_dn;
          } else {
            while ((current_dn->next_dep) != NULL){
              current_dn = current_dn->next_dep;
            }
            current_dn->next_dep = new_dn;
          }
          new_dn->next_dep = NULL;
          new_dn->rule = NULL;
          struct rule_node *rules = rn;
          while (rules != NULL && (strcmp(rules->target, nline) != 0)){
            rules = rules->next_rule;
          }
          if (rules == NULL){
            rules = malloc(sizeof(struct rule_node));
            rules->target = malloc(sizeof(char) * (strlen(nline) + 1));
            strcpy(rules->target, nline);
            (rules->target)[strlen(nline)] = '\0';
            rules->dependencies = NULL;
            rules->actions = NULL;

            struct dep_node *dn_dn = curr_rule->dependencies;
            if (dn_dn == new_dn){ //1st dependency
              rules->next_rule = curr_rule->next_rule;
              curr_rule->next_rule = rules;
            } else {
              while (dn_dn->next_dep != new_dn){
                dn_dn = dn_dn->next_dep;
              }
              rules->next_rule = dn_dn->rule->next_rule;
              dn_dn->rule->next_rule = rules;
            }
            new_dn->rule = rules;
          }
          nline = strtok(NULL, " ");
        }
      }
    }
    return rn;
}
