#ifndef FANCTRL_H
#define FANCTRL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char CMD_ACTIVE[];
extern const char CMD_LIST[];

int connect_to_fanctrl(void);

int send_command(int socket_fd, const char cmd[]);

int receive_response(int socket_fd, char buf[], int n);

int send2socket(const char cmd[], char *ret, size_t ret_size);

int get_active_strat(char *ret, size_t ret_size);

int get_all_strats(char *ret, size_t ret_size);

int set_strat(const char *strat, char *ret, size_t ret_size);

int parse_stratlist(char *to_parse, char *ret, size_t ret_size);

int parse_active_strat(char *to_parse, char *ret, size_t ret_size);

#ifdef __cplusplus
}
#endif

#endif /* FANCTRL_H */