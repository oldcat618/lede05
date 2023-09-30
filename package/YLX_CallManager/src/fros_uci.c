#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uci.h>
#include "fros_uci.h"

int fros_uci_add_section(struct uci_context * ctx, char *package_name, char *section)
{
    struct uci_package *p = NULL;
    struct uci_section *s = NULL;
    int ret;

    ret = uci_load(ctx, package_name , &p);
    if (ret != UCI_OK)
    	goto done;

    ret = uci_add_section(ctx, p, section, &s);
    if (ret != UCI_OK)
    	goto done;

    ret = uci_save(ctx, p);

done:
    if (ret != UCI_OK)
        printf("add section %s failed\n", section);
    else if (s)
        fprintf(stdout, "%s\n", s->e.name);

    return ret;
}


int fros_uci_delete(struct uci_context *ctx, char *key)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};    
    strcpy(param_tmp, key);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    ret = uci_delete(ctx, &ptr);
    if (ret == UCI_OK)
       ret = uci_save(ctx, ptr.p);

	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}


int fros_uci_add_list(struct uci_context *ctx, char *key, char *value)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[2048] = {0};    
    sprintf(param_tmp, "%s=%s", key, value);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    ret = uci_add_list(ctx, &ptr);
    if (ret == UCI_OK)
       ret = uci_save(ctx, ptr.p);

	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}


int fros_uci_add_int_list(struct uci_context *ctx, char *key, int value)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[2048] = {0};    
    sprintf(param_tmp, "%s=%d", key, value);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    ret = uci_add_list(ctx, &ptr);
    if (ret == UCI_OK)
       ret = uci_save(ctx, ptr.p);

	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}

int fros_uci_del_list(struct uci_context *ctx, char *key, char *value)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};    
    sprintf(param_tmp, "%s=%s", key, value);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    ret = uci_del_list(ctx, &ptr);
    if (ret == UCI_OK)
       ret = uci_save(ctx, ptr.p);

	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}


int fros_uci_commit(struct uci_context *ctx, const char * package) {
    struct uci_ptr ptr;
    int ret = UCI_OK;
    if (!package){
        return -1;
    }
    if (uci_lookup_ptr(ctx, &ptr, package, true) != UCI_OK) {
        return -1;
    }   

    if (uci_commit(ctx, &ptr.p, false) != UCI_OK) {
    	ret = -1;
        goto done;
    }
done:
	if (ptr.p)
		uci_unload(ctx, ptr.p);

    return UCI_OK;
}

int fros_uci_set_value(struct uci_context *ctx, char *key, char *value)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[2048] = {0};    
    sprintf(param_tmp, "%s=%s", key, value);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    
    e = ptr.last;
    ret = uci_set(ctx, &ptr);
    if (ret == UCI_OK)
       ret = uci_save(ctx, ptr.p);

	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}

int fros_uci_set_int_value(struct uci_context *ctx, char *key, int value)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};    
    sprintf(param_tmp, "%s=%d", key, value);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    e = ptr.last;
    ret = uci_set(ctx, &ptr);
    if (ret == UCI_OK)
       ret = uci_save(ctx, ptr.p);

    if (ptr.p)
        uci_unload(ctx, ptr.p);
    return ret;
}


int fros_uci_get_int_value(struct uci_context *ctx, char *key)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = -1;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};
    strcpy(param_tmp, key);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        return ret;
    }
    
    if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
        ctx->err = UCI_ERR_NOTFOUND;
        goto done;
    }
    
    e = ptr.last;
    switch(e->type) {
        case UCI_TYPE_SECTION:
            ret = -1;
			goto done;
        case UCI_TYPE_OPTION:
            ret = atoi(ptr.o->v.string);
			goto done;
        default:
            break;
    }
done:
	
	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}



int fros_uci_get_list_value(struct uci_context *ctx, char *key, char *output, int out_len, char *delimt)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = -1;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};
    strcpy(param_tmp, key);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        return ret;
    }
    
    if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
        ctx->err = UCI_ERR_NOTFOUND;
        goto done;
    }
    int sep = 0;
    e = ptr.last;
	int len = 0;
    switch(e->type) {
        case UCI_TYPE_SECTION:
            ret = -1;
			goto done;
        case UCI_TYPE_OPTION:
			if (UCI_TYPE_LIST == ptr.o->type){
				memset(output, 0x0, out_len);
				uci_foreach_element(&ptr.o->v.list, e) {
					len = strlen(output);
					if (sep){
						strncat(output + len, delimt, out_len);
					}
					len = strlen(output);
					sprintf(output + len, "%s", e->name);
					sep = 1;
				}
				ret = 0;
			}
			goto done;
        default:
            break;
    }
done:	
	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}


int fros_uci_get_value(struct uci_context *ctx, char *key, char *output, int out_len)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};
    strcpy(param_tmp, key);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    
    if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
        ctx->err = UCI_ERR_NOTFOUND;
        ret = 1;
        goto done;
    }
    
    e = ptr.last;
    switch(e->type) {
        case UCI_TYPE_SECTION:
            snprintf(output, out_len, "%s", ptr.s->type);
            break;
        case UCI_TYPE_OPTION:
            snprintf(output, out_len, "%s", ptr.o->v.string);
			break;
        default:
			ret = 1;
            break;
    }
done:    
	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}



int fros_uci_set_int64_value(struct uci_context *ctx, char *key, long long value)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    int ret = UCI_OK;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};    
    sprintf(param_tmp, "%s=%lld", key, value);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        ret = 1;
        return ret;
    }
    e = ptr.last;
    ret = uci_set(ctx, &ptr);
    if (ret == UCI_OK)
       ret = uci_save(ctx, ptr.p);

    if (ptr.p)
        uci_unload(ctx, ptr.p);
    return ret;
}


long long fros_uci_get_int64_value(struct uci_context *ctx, char *key)
{
    struct uci_element *e;
    struct uci_ptr ptr;
    long long ret = -1;
    int dummy;
    char *parameters ;
    char param_tmp[128] = {0};
    strcpy(param_tmp, key);
    if (uci_lookup_ptr(ctx, &ptr, param_tmp, true) != UCI_OK) {
        return ret;
    }
    
    if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
        ctx->err = UCI_ERR_NOTFOUND;
        goto done;
    }
    
    e = ptr.last;
    switch(e->type) {
        case UCI_TYPE_SECTION:
            ret = -1;
			goto done;
        case UCI_TYPE_OPTION:
            ret = strtoll(ptr.o->v.string, NULL, 10);
			goto done;
        default:
            break;
    }
done:
	
	if (ptr.p)
		uci_unload(ctx, ptr.p);
    return ret;
}


int fros_uci_get_array_value(struct uci_context *ctx, char *key_fmt, int index, char *output, int out_len)
{
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_get_value(ctx, key, output, out_len);
}

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
int fros_uci_get_array_list_value(struct uci_context *ctx, char *key_fmt, int index, char *output, int out_len, char * delimit)
{
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_get_list_value(ctx, key, output, out_len, delimit);
}


int fros_uci_get_array_int_value(struct uci_context *ctx, char *key_fmt, int index)
{
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_get_int_value(ctx, key);
}

long long  fros_uci_get_array_int64_value(struct uci_context *ctx, char *key_fmt, int index)
{
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_get_int64_value(ctx, key);
}

int fros_uci_del_array_value(struct uci_context *ctx, char *key_fmt, int index){
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_delete(ctx, key);
}

int fros_uci_set_array_value(struct uci_context *ctx, char *key_fmt, int index, char *value){
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_set_value(ctx, key, value);
}

int fros_uci_set_array_int_value(struct uci_context *ctx, char *key_fmt, int index, char *value){
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_set_int_value(ctx, key, value);
}

int fros_uci_set_array_int64_value(struct uci_context *ctx, char *key_fmt, int index, long long value){
    char key[128] = {0};
    sprintf(key, key_fmt, index);
    return fros_uci_set_int64_value(ctx, key, value);
}

void fros_uci_set_tmp_int_value(char *key, int value)
{
    int i;
    struct uci_context * ctx = uci_alloc_context();
    if (!ctx){
        return -1;
    }
	ctx->confdir = strdup("/tmp/config");
    fros_uci_set_int_value(ctx, key, value);
    uci_free_context(ctx);
}

int fros_uci_get_tmp_int_value(char *key)
{
    int i;
    int ret = 0;
    struct uci_context * ctx = uci_alloc_context();
    if (!ctx){
        return -1;
    }
	ctx->confdir = strdup("/tmp/config");
    ret = fros_uci_get_int_value(ctx, key);
    uci_free_context(ctx);
    return ret;
}


int fros_uci_get_section_index(struct uci_context * ctx, char *package, char *section, char *option, int value){
    char buf[128] = {0};
    struct uci_ptr p;
    struct uci_element *e;
    struct uci_package *pkg = NULL;

    if (UCI_OK != uci_load(ctx, package, &pkg)){
        uci_free_context(ctx);
        printf("load section failed\n");
        return -1;
    }
    char *p_value = NULL;
    int index = 0;
    uci_foreach_element(&pkg->sections, e) {
        struct uci_section *s = uci_to_section(e);
        if (strcmp(s->type, section)){
            continue;
        }
        if (NULL == (p_value = uci_lookup_option_string(ctx, s, option)))  
            continue;
        if (atoi(p_value) == value){
            goto DONE;
        }
        index ++;
    }
    index = -1;
DONE:
    uci_unload(ctx, pkg);
    return index;
}


int fros_uci_get_section_index_by_str_opt(struct uci_context * ctx, char *package, char *section, char *option, char *value){
    char buf[128] = {0};
    struct uci_ptr p;
    struct uci_element *e;
    struct uci_package *pkg = NULL;

    if (UCI_OK != uci_load(ctx, package, &pkg)){
        uci_free_context(ctx);
        printf("load section failed\n");
        return -1;
    }
    char *p_value = NULL;
    int index = 0;
    uci_foreach_element(&pkg->sections, e) {
        struct uci_section *s = uci_to_section(e);
        if (strcmp(s->type, section)){
            continue;
        }
        if (NULL == (p_value = uci_lookup_option_string(ctx, s, option)))  
            continue;
        if (0 == strcmp(p_value, value))
            goto DONE;
      
        index ++;
    }
    index = -1;
DONE:
    uci_unload(ctx, pkg);
    return index;
}


int fros_get_uci_list_num(struct uci_context * ctx, char *package, char *section){
    struct uci_ptr p;
	struct uci_element *e;
	struct uci_package *pkg = NULL;
    int count = 0;
    if (UCI_OK != uci_load(ctx, package, &pkg)){
		printf("load failed\n");
        return -1;
    }
    uci_foreach_element(&pkg->sections, e){
        struct uci_section *s = uci_to_section(e);
        if (strcmp(s->type, section)){
            continue;
        }
        count++;
    }
    uci_unload(ctx, pkg);
    return count;
}

/*
	desc: get section name
	eg: 
	get wifi-device name ra0
	section: wifi-device
	name: ra0
	config wifi-device 'ra0'
		option type 'mac80211'
		option path 'platform/soc/c000000.wifi'
		option band '5g'
		option htmode 'HE80'
		option country 'US'
		option channel '6'

*/
int fros_uci_get_section_name(struct uci_context *ctx, char *package, char *section, int index, char *name){
    struct uci_ptr p;
    struct uci_element *e;
    struct uci_package *pkg = NULL;

    if (UCI_OK != uci_load(ctx, package, &pkg)){
        return -1;
    }
    char *p_value = NULL;
    int uci_index = 0;
    uci_foreach_element(&pkg->sections, e) {
        struct uci_section *s = uci_to_section(e);
		if (0 != strcmp(s->type, section))
			continue;

		if (index == uci_index){
			if (!s->anonymous){
				strcpy(name, e->name);
			}
			else
				strcpy(name, "");			
			uci_unload(ctx, pkg);
			return 0;
		}
        uci_index++;
    }
DONE:
    uci_unload(ctx, pkg);
    return -1;
}


