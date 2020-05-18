// ndg_hello module
#include <ngx_http.h>

static ngx_int_t ngx_http_hello_handler(ngx_http_request_t *r);
typedef struct {
    /* data */
    ngx_flag_t  enable;
} ngx_http_hello_loc_conf_t;


static ngx_command_t ngx_http_hello_cmds[] = {
    {
        ngx_string("hello"), // command name
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(
            ngx_http_hello_loc_conf_t, 
            enable
        ),
        NULL
    },
    ngx_null_command
};

static void *
ngx_http_hello_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_hello_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_hello_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    conf ->enable = NGX_CONF_UNSET;
    return conf;
}


static ngx_int_t 
ngx_http_hello_init(ngx_conf_t* cf)
{
    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers);
    *h = ngx_http_hello_handler;

    return NGX_OK;
}

static ngx_http_module_t ngx_http_hello_module_ctx = 
{
    NULL,
    ngx_http_hello_init,
    NULL,
    NULL,
    NULL,
    NULL,
    ngx_http_hello_create_loc_conf,
    NULL,
};

ngx_module_t ngx_http_hello_module =
{
    NGX_MODULE_V1,
    &ngx_http_hello_module_ctx,
    ngx_http_hello_cmds,
    NGX_HTTP_MODULE,
    NULL,           // init master
    NULL,           // init module
    NULL,           // init process
    NULL,           // init thread
    NULL,           // exit thread
    NULL,           // exit process
    NULL,           // exit master
    NGX_MODULE_V1_PADDING
};

static ngx_int_t 
ngx_http_hello_handler(ngx_http_request_t *r)
{
    ngx_http_hello_loc_conf_t* lcf;

    lcf = ngx_http_get_module_loc_conf(r, ngx_http_hello_module);

    if (lcf->enable) {
        printf("hello nginx module, uri: %s\n", (char*)r->uri.data);
    } else {
        printf("hello nginx module dsiable\n");
    }

    return NGX_DECLINED;
}