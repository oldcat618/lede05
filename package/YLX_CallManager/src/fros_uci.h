#ifndef __FROS_UCI_H__
#define __FROS_UCI_H__
#include <uci.h>
int fros_uci_get_value(struct uci_context *ctx, char *key, char *output, int out_len);
int fros_uci_set_value(struct uci_context *ctx, char *key, char *value);
int fros_uci_set_int_value(struct uci_context *ctx, char *key, int value);
int fros_uci_get_int_value(struct uci_context *ctx, char *key);
int fros_uci_delete(struct uci_context *ctx, char *key);
int fros_uci_add_section(struct uci_context * ctx, char *package_name, char *section);
int fros_uci_get_section_index(struct uci_context * ctx, char *package, char *section, char *option, int value);
int fros_uci_get_section_index_by_str_opt(struct uci_context * ctx, char *package, char *section, char *option, char *value);
int fros_get_uci_list_num(struct uci_context * ctx, char *package, char *section);
int fros_uci_add_list(struct uci_context *ctx, char *key, char *value);
int fros_uci_del_list(struct uci_context *ctx, char *key, char *value);
int fros_uci_commit(struct uci_context *ctx, const char * package);
int fros_uci_set_int64_value(struct uci_context *ctx, char *key, long long value);
long long fros_uci_get_int64_value(struct uci_context *ctx, char *key);
int fros_uci_get_array_value(struct uci_context *ctx, char *key_fmt, int index, char *output, int out_len);
int fros_uci_get_array_int_value(struct uci_context *ctx, char *key_fmt, int index);
long long  fros_uci_get_array_int64_value(struct uci_context *ctx, char *key_fmt, int index);
int fros_uci_set_array_value(struct uci_context *ctx, char *key_fmt, int index, char *value);
int fros_uci_set_array_int_value(struct uci_context *ctx, char *key_fmt, int index, char *value);
int fros_uci_set_array_int64_value(struct uci_context *ctx, char *key_fmt, int index, long long value);
void fros_uci_set_tmp_int_value(char *key, int value);
int fros_uci_get_tmp_int_value(char *key);
int fros_uci_add_int_list(struct uci_context *ctx, char *key, int value);
int fros_uci_get_list_value(struct uci_context *ctx, char *key, char *output, int out_len, char *delimt);
int fros_uci_get_section_name(struct uci_context *ctx, char *package, char *section, int index, char *name);

/*
key_fmt:app_filter.@rule[%d].appid
index: 0
config rule
	list appid 1001
	list appid 1002
	list appid 2001
config rule
	list appid 2001
	list appid 3001
	list appid 4001
output: 
	if delimit is ','
	1001,1002,2001
*/
int fros_uci_get_array_list_value(struct uci_context *ctx, char *key_fmt, int index, char *output, int out_len, char * delimit);

#endif
