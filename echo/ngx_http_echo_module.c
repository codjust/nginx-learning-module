// echo module
#include <ngx_http.h>

static ngx_int_t ngx_http_echo_handler(ngx_http_request_t *r);
static char *ngx_http_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_http_echo_create_loc_conf(ngx_conf_t* cf);

typedef struct {
    /* data */
    ngx_str_t  msg;
} ngx_http_echo_loc_conf_t;


static ngx_command_t ngx_http_echo_cmds[] = {
    {
        ngx_string("echo"), // command name
        NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_http_echo,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(
            ngx_http_echo_loc_conf_t, 
            msg
        ),
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_echo_module_ctx = 
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ngx_http_echo_create_loc_conf,
    NULL,
};

ngx_module_t ngx_http_echo_module =
{
    NGX_MODULE_V1,
    &ngx_http_echo_module_ctx,
    ngx_http_echo_cmds,
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

static void *
ngx_http_echo_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_echo_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_echo_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }
  
    return conf;
}

static char *
ngx_http_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t* clcf;
    char* rc = ngx_conf_set_str_slot(cf, cmd, conf); // 手动调用配置解析函数，主要为了后面的handler的注册
    if ( rc != NGX_CONF_OK ) {
        return rc;
    }

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_echo_handler; // 通过第二种方式注册handler

    return NGX_CONF_OK;
}



static ngx_int_t ngx_http_echo_handler(ngx_http_request_t *r)
{
    ngx_int_t rc;
    ngx_http_echo_loc_conf_t* lcf;
    size_t len;

    // 请求方法必须是GET
    if (!(r->method & NGX_HTTP_GET)) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    // 丢弃请求体
    rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK) {
        return rc;
    }

    // 获取配置数据，即在配置里配了什么指令等信息
    lcf = ngx_http_get_module_loc_conf(r, ngx_http_echo_module);
    
    len = lcf->msg.len;
    if(r->args.len) {
        len += r->args.len + 1; // 总长度加多一个‘，’的长度
    }

    // 操作响应头设置状态码和响应体长度，再发送响应头
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = len;

    // 发送响应头
    rc = ngx_http_send_header(r);
    if( rc == NGX_ERROR || rc > NGX_OK || r->header_only) {  // headers_only无须发送body
        return rc;
    }

    // 最后通过内存池分配缓冲区、创建数据块链发送响应体数据
    ngx_buf_t* b = ngx_create_temp_buf(r->pool, len);

    if(r->args.len) {
        // 请求uri有参数则拷贝到buff
        b->last = ngx_copy(b->pos, r->args.data, r->args.len);
        *(b->last++) = ',';
    }

    b->last = ngx_copy(b->last, lcf->msg.data, lcf->msg.len);

    b->last_buf = 1; //表明是最后一块数据
    b->last_in_chain = 1;

    // 分配chain节点
    ngx_chain_t * out = ngx_alloc_chain_link(r->pool);
    out->buf = b;
    out->next = NULL;

    return ngx_http_output_filter(r, out);
}
