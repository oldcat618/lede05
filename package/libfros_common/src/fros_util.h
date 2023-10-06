#ifndef __FROS_UTIL_H__
#define __FROS_UTIL_H__
#include <sys/types.h>
#define MAC_ADDR_LEN 6

#define LICENSE_DEFAULT_IV "31313131312D2D2D"
#define LICENSE_DEFAULT_K  "31313131312D2D2D"
#define LICENSE_DEFAULT_PASSWORD "frosis666.$@-*2shs"
#define MAX_EXEC_CMD_LEN 128

unsigned int fros_get_timestamp(void);
unsigned int fros_get_uptime(void);

int fros_read_int_from_file(const char* fileName);
int fros_write_int_to_file(const char* fileName, int num);

int mac_to_hex(u_int8_t *mac, u_int8_t *mac_hex);
char *fros_trim(char *s) ;

int fros_time_str_to_min(char *str);
int fros_get_cur_min(void);

int fros_get_cur_week_day();
/*
	10:00-12:00
*/
int fros_match_time_str(char *time_str);

char * fros_get_lan_mac(void);
int fros_get_cmd_result(char *cmd, char *result, int len);
int fros_get_cmd_result_buf(char *cmd, char *result_buf, int buf_len);
int fros_exec(const char *fmt, ...);
#endif

