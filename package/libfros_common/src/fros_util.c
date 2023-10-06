#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <stdarg.h>
#include "fros_util.h"

#define MAC_ADDR_LEN 6

int fros_read_int_from_file(const char* fileName) {
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Failed to open file %s\n", fileName);
        return 0;
    }

    int num;
    if (fscanf(fp, "%d", &num) == 1) {
        fclose(fp);
        return num;
    }

    fclose(fp);
    return 0;
}


int fros_write_int_to_file(const char* fileName, int num) {
    FILE* fp = fopen(fileName, "w");
    if (fp == NULL) {
        printf("Failed to open file %s\n", fileName);
        return -1;
    }
    
    fprintf(fp, "%d", num);
    fclose(fp);
    
    return 0;
}



unsigned int fros_get_timestamp(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

unsigned int fros_get_uptime(void)
{
    struct sysinfo stat;
    sysinfo(&stat);            
    return stat.uptime;
}


int fros_get_cur_hour(void)
{
	struct tm *t = NULL;
    struct tm now_time;
	time_t timep = 0;
	time(&timep);
	t = localtime_r(&timep, &now_time);
	return now_time.tm_hour;
}

int fros_get_week_day(void)
{
	struct tm *t = NULL;
    struct tm now_time;
	time_t timep = 0;
	time(&timep);
	t = localtime_r(&timep, &now_time);
	return now_time.tm_wday;
}

char * fros_get_lan_mac(void){
    FILE *fp = NULL;
    char buf[64] = {0};
    fp = fopen("/sys/class/net/br-lan/address", "r");
    if (!fp)
        return NULL;
    if (fgets(buf, sizeof(buf), fp)){
        fros_trim(buf);
        fclose(fp);
        return strdup(buf);
    }
    else{
        fclose(fp);
        return NULL;
    }
}

int mac_to_hex(u_int8_t *mac, u_int8_t *mac_hex){
    u_int32_t mac_tmp[MAC_ADDR_LEN];
    int ret = 0, i = 0;
    ret = sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
        (unsigned int *)&mac_tmp[0],
        (unsigned int *)&mac_tmp[1],
        (unsigned int *)&mac_tmp[2],
        (unsigned int *)&mac_tmp[3],
        (unsigned int *)&mac_tmp[4],
        (unsigned int *)&mac_tmp[5]);
    if (MAC_ADDR_LEN != ret)
        return -1;
    for (i = 0; i < MAC_ADDR_LEN; i++)
    {
    	mac_hex[i] = mac_tmp[i];
    }
    return 0;
}

#define MAX_EXEC_CMD_LEN 128
int fros_exec(const char *fmt, ...)
{
     va_list vargs;
     char  cmd_buf[MAX_EXEC_CMD_LEN] = {0};
     va_start(vargs, fmt);
     vsnprintf(cmd_buf, sizeof(cmd_buf), fmt, vargs);
     va_end(vargs);
	 return system(cmd_buf);
} 


char *fros_trim(char *s) {
    char *start, *last, *bk;
    int len;

    start = s;
    while (isspace(*start))
        start++;

    bk = last = s + strlen(s) - 1;
    while (last > start && isspace(*last))
        last--;

    if ((s != start) || (bk != last)) {
        len = last - start + 1;
        strncpy(s, start, len);
        s[len] = '\0';
    }   
    return s;
}


int fros_get_cmd_result(char *cmd, char *result, int len)
{
    FILE *fp = NULL;
	if (!cmd || !result || !len)
		return -1;
    fp = popen(cmd, "r");
    if (!fp) 
        return -1;
    fgets(result, len, fp);   
    pclose(fp);
	return 0;
}

int fros_get_cmd_result_buf(char *cmd, char *result_buf, int buf_len)
{
	FILE *fp = NULL;
	int read_len = 0;
	
	if (!cmd || !result_buf || !buf_len)
		return -1;
	
    fp = popen(cmd, "r");
    if (!fp) 
        return -1;
	
	read_len = fread(result_buf, 1, buf_len, fp);
	if (read_len >= 0 && read_len < buf_len){
		result_buf[read_len] = '\0';
	}
    pclose(fp);
	return read_len;
}
int fros_time_str_to_min(char *str)
{
	int hour,min;
	if (strlen(str) < 4 || !strstr(str, ":")) {
		printf("invalid time format:%s\n", str);
		return -1;
	}
	sscanf(str, "%d:%d", &hour, &min);
	return hour * 60 + min;
}

int fros_get_cur_min(void)
{
	struct tm *t = NULL;
    struct tm now_time;
	time_t timep = 0;
	time(&timep);
	t = localtime_r(&timep, &now_time);
	return now_time.tm_hour * 60 + now_time.tm_min;
}

int fros_get_cur_week_day()
{
	struct tm *t = NULL;
    struct tm now_time;
	time_t timep = 0;
	time(&timep);
	t = localtime_r(&timep, &now_time);
	return now_time.tm_wday;
}
/*
	10:00-12:00
*/
int fros_match_time_str(char *time_str){
	char start[64] = {0};
	char end[64] = {0};
	if (!time_str)
		return 0;
	if (strlen(time_str) < 8 || strlen(time_str) > 12)
		return 0;
	if (2 != sscanf(time_str, "%[^-]-%s", start, end))
		return 0;
	if (strlen(start) < 4 || strlen(end) < 4) {
		return 0;
	}
	int start_min = fros_time_str_to_min(start);
	int end_min = fros_time_str_to_min(end);
	int cur_min = fros_get_cur_min();
	if (start_min > cur_min || end_min < cur_min)
		return 0;
	return 1;
}



