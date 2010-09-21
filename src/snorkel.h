/**
 *
 * snorkel.h - definitions/declarations defining the snorkel API
 *
 * Copyright (c) Walter E. Capers <walt.capers@compuware.com>.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, and/or the phrase "Server powered by Snorkel" on
 *    any visable web-page or startup notification.  This list of
 *    conditions and the following disclaimer and dedication in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Purpose:
 *  The Snorkel API defines a set of routines designed to provide cross
 *  platform embedded HTTP/HTTPS support.
 *
 * Comments:
 * version 1.0.7
 *
 **/
#ifndef SNORKEL_H
#define SNORKEL_H
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif


  typedef void *snorkel_obj_t;
  typedef unsigned char byte_t;
  typedef byte_t *lpbyte_t;
  typedef char *lpchar_t;

#define HTTP_ERROR  snorkel_make_return_value ("The server has encountered an unexpected\r\n"\
"condition, which prevents it from fulfilling\r\n"\
"your request.\r\n")
#define PROTOCOL_ERROR \
snorkel_make_return_value ("an unexpected condition prevents"\
"the server from processing the request")

#define ERROR_STRING(s) snorkel_make_return_value (s)

#define HTTP_SUCCESS ((const char *)0)
#define PROTOCOL_SUCCESS ((const char *)0)
  typedef const char *call_status_t;

  typedef byte_t (*snorkel_task_t) (void *);

  typedef
    call_status_t (*snorkel_uri_callback_t)
    (snorkel_obj_t, snorkel_obj_t);

  typedef
    call_status_t (*snorkel_proto_callback_t)
    (snorkel_obj_t);

  typedef
    call_status_t (*snorkel_mime_callback_t)
    (snorkel_obj_t, snorkel_obj_t, char *);

  typedef byte_t (*bubble_main_t) (snorkel_obj_t);

#define BUBBLE_EXT ".bbl"

  typedef enum
  {
    encodingtype_binary, encodingtype_text
  } encodingtype_t;

/*
 *
 * better describes data content so as not to confuse
 * with UU encode
 *
 */
#define contenttype_binary encodingtype_binary
#define contenttype_text   encodingtype_text
#define contenttype_t      encodingtype_t

#define POST  0x01
#define GET   0x02
#define HEAD  0x04
#define PROTOCOL 0x08
#define MIME  0x10
//#define URI   0x20
#define IGNORE_MIME 0xFFFF

#define SNORKEL_SUCCESS			0
#define SNORKEL_ERROR          -1
#define SNORKEL_BUSY            2
#define SNORKEL_SSL_CERT        0x02
#define SNORKEL_STORE_AS_DUP    0x02
#define SNORKEL_STORE_AS_REF    0x04

#define TAG_ENDER              "stagend"

#if defined(WIN32) || defined(WIN64)
#define SNORKEL_EXPORT _declspec (dllexport)
#else
#define SNORKEL_EXPORT
#endif

/*
 *
 * object types
 *
 */
  typedef enum
  {
    snorkel_obj_server,  /* args: (int) number_handlers, (char *)html_root_dir */
    snorkel_obj_mutex,   /* args: none */
    snorkel_obj_event,   /* args: none */
    snorkel_obj_thread,  /* args: (snorkel_thread_func_t) callback */
    snorkel_obj_httpreq,
    snorkel_obj_buffer,         /* internal usage only */
    snorkel_obj_log,            /* creates a shared log file, a log file
                                   that can log both application messages and API
                                   messages
								   args: (char *)logfile */
    snorkel_obj_stream, /* args: (char *)hostname, (char *)protocol_name || (char *)0,
						         (int)port, (int)timeout */
    snorkel_obj_protocol, /* args: (char *)protocol_name */
    snorkel_obj_chunk, /* not supported */
    snorkel_obj_sys,
    snorkel_obj_unknown,
    snorkel_obj_worker_thread, /* arg: (int) heap_size, (int) queue_size */
    snorkel_obj_appl_log, /* creates a log object that will not log
                             snorkel errors only messages printed by the application - entries
                             are not time stamped
							 args: (char *)logfile */
    snorkel_obj_appl_tstamped_log, /* same as above; however, entries are time stamped
								   args: (char *)logfile */
    snorkel_obj_ssl_stream,  /* creates a simple client-ssl connection without authentication
	                            args: (char *)hostname, (char *)protocol_name||(char *)0, (int)port,
								      (int)timeout, (char *)certificate||(char *)0, (char *)key_file||(char *)0 */
  } snorkel_obj_type_t;

  /*
   *
   * object operators
   *
   */
  typedef enum
  {
    snorkel_attrib_header,
    /*
       usage:
       retrieve the value of a header variable from the http request.

       snorkel_obj_get (snorkel_obj_t http_request_object,
       snorkel_attrib_header,
       char *buffer,
       size_t size_of_buffer) */
    snorkel_attrib_post,
    /*
       usage:
       retrieve the value of a HTTP post variable.

       snorkel_obj_get (snorkel_obj_t http_request_object,
       snorkel_attrib_post,
       char *buffer,
       size_t size_of_buffer)
     */
    snorkel_attrib_mime,
    /*
       usage:
       overloads a MIME... associates a function with an extension-file type
       for interpreting-converting the file type to a form viewable in a
       browser.

       snorkel_obj_set (snorkel_obj_t server_object,
       char *HTTP_file_type (see HTTP file types above),
       encodingtype_t format_of_data,
       snorkel_mime_callback_t intepreter_function)
     */
    snorkel_attrib_size,
    /*
       usage:
       not currently used.
     */
    snorkel_attrib_index_file,
    /*
       usage:
       sets the index file name for an http server object. if used, function
       can only be called prior to starting the server with snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_index_file,
       char *filename_with_extension);
     */
    snorkel_attrib_pagespan,
    /*
       usage:
       sets the life span of an http page before it must be
       revalidated.  Used for Cache control. Note: refers client-side
	   cache not server side.

       snorkel_obj_set (snorkel_obj_t http_request_object,
       snorkel_attrib_pagespan,
       int seconds)
     */
    snorkel_attrib_ssi,
    /*
       usage:
       not fully supported
     */
    snorkel_attrib_tcpbuffsize,
    /*
       usage:
       sets the tcp window size. note: if used, can only be used on server objects
       prior to starting the object using snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t (http_request_object, server_object, or
       stream_object),
       int buffer_size_in_bytes)
     */
    snorkel_attrib_threadheap_size,
    /*
       usage:
       sets global thread heap storage size. use snorkel_get_sys to retrieve system
       object. note: if used, must be called prior to starting the server using snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t system_object,
       snorkel_attrib_threadheap_size,
       int heap_size_in_bytes)
     */
    snorkel_attrib_threadheap_keep,
    /*
       usage:
       depricated */
    snorkel_attrib_listener,
    /*
       usage:
       associates a port number with a http server object.
       note: if used, must be called prior to starting the server object using
       snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_listener,
       int port_number,
       int SSL_flag (enabled=1, disabled=0)
     */
    snorkel_attrib_ssl,
    /*
       usage:
       identify the location of the SSL key file. note: if used,
       must be called prior to starting the server using snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_ssl,
       char *pem_file)
     */
    snorkel_attrib_show_dir,
    /*
       usage:
       allow directory listings in directories that do not contain
       an index file. note: if used, must be called before server is started
       using snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_show_dir,
       int flag (enabled=1, disabled=0)
     */
    snorkel_attrib_bubbles,
    /*
       usage:
       enables bubble support. note: if used, must be called before server is started
       using snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_bubbles,
       char *bubble_directory or NULL)
       note: if bubble directory is NULL startup directory assumed.
     */
    snorkel_attrib_cookie,
    /*
       usage:
       sets the value of a cookie.

       snorkel_obj_set (snorkel_obj_t http_request_object,
       char *cookie_name,
       char *cookie_value,
       char *domain (null or a string containing the domain for which the cookie is valid),
       int secure (enabled=1, disabled=0),
       int duration_in_seconds (can be zero if cookie never expires))
     */
    /* to keep from confusing myself about the difference between a uri and a url
       here is a definition of the two:
       uri: universal resource identifier, refers to a resource by name but does
       not identify the actual location of the resource.  For example, my name
       is Walter E. Capers.  Even though it is my name it does not identify my
       location or my uniqueness -- there my be other Walter E. Capers in the world.
       In another example, /index.html is a resource; however it is not unique.
       Just about every web page has an index.html file.
       url: universal resource locator, refers to the unique location of a resource.
       For example, http://walt.com/file.htm refers to a specific file identified
       by both location and name.
     */
    snorkel_attrib_uri,
    /*
       usage:
       sets a callback-overloads how a uri is processed by the server.  instead
       of looking to the file system to resolve the uri the server calls the
       provided callback to provide the responce. note: if used, must be called
       prior to starting the server.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_uri,
       int METHOD (POST or GET),
       char *uri,
       encodingtype_t type (encodingtype_binary or encodingtype_text),
       snorkel_uri_callback_t callback)
     */
    snorkel_attrib_uri_content,
    /*
       usage:
       sets-overloads a uri with a provided buffer.  instead of looking to the file system
       the server uses the contents of the provided buffer to build the responce.
       note: if used, must be called prior to starting the server with snorkel_obj_start.
       note: only applies to existing listeners.

       snorkel_obj_set(snorkel_obj_t server_object,
       snorkel_attrib_uri_content,
       int method (must be GET),
       char *uri,
       char *content_buffer,
       int storage_method (SNORKEL_STORE_AS_REF each listener recieves a reference to the content
       SNORKEL_STORE_AS_DUP each listener recieves a copy of the buffer (faster)))
     */
    snorkel_attrib_uri_cache,
    /*
       usage:
       create a content buffer using a file.  note: if used, must be called prior to calling snorkel_obj_start.
       note: only applies to existing listeners.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_uri_cache,
       char *uri,
       char *filename,
       int storage_method (SNORKEL_STORE_AS_REF each listener recieves a reference to the content
       SNORKEL_STORE_AS_DUP each listener recieves a copy of the buffer (faster)))
     */
    snorkel_attrib_uri_path,
    /*
       usage:
       retrieves the uri path-parent uri from an http_request object.

       snorkel_obj_get (snorkel_obj_t http_request_object,
       snorkel_attrib_uri_path,
       char *buffer,
       int size_of_buffer)
     */
    snorkel_attrib_local_url,
    /*
       usage:
       retrieves the fully qualified path to the requested URL.

       snorkel_obj_get (snorkel_obj_t http_request_object,
       snorkel_attrib_local_url,
       char *buffer,
       int size_of_buffer)
     */
    snorkel_attrib_local_url_path,
    /*
       usage:
       retrieves the parent directory of the requested url.

       snorkel_obj_get (snorkel_obj_t http_request_object,
       snorkel_attrib_local_url_path,
       char *buffer,
       int size_of_buffer)
     */
    snorkel_attrib_post_ref,
    /*
       usage:
       works like snorkel_attrib_post, retrieves post variable values.  it gets value
       references instead of copying them into a buffer.

       snorkel_obj_get (snorkel_obj_t http_request_object,
       snorkel_attrib_post_ref,
       char *variable,
       char **value,
       int *size_of_data_returned_in_value)
     */

#define SOCK_SET                0x01
#define SOCK_CLR                0x00
#define IPVERS_IPV6             0x01
#define IPV6_ALLOW_SAME_SITE    0x02
#define IPV6_ALLOW_EXTERN_SITE  0x04
#define IPV6_ALLOW_NAT          0x08
#define IPVERS_IPV4             0x10
    snorkel_attrib_ipvers,      /* internet protocol version, refer to documentation */
#define SNORKEL_LINGER_DISABLED    -1   /* don't linger at all */
#define SNORKEL_LINGER_USE_DEFAULT -2   /* use system default linger time, usually
                                           its about 60 seconds */
    snorkel_attrib_linger,
    /*
       usage:
       set the TCP linger value -- how long to keep a socket open after it is closed.
       note: only applied to existing listeners.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_linger,
       int time_in_seconds)
     */
    snorkel_attrib_timeout,
    /*
       usage:
       sets server timeout for existing listeners -- specifies how long a listener will wait
       for a client responce before giving up.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_timeout,
       int time_in_seconds)
     */
    snorkel_attrib_thread_governor,
    /*
       usage:
       overrides the global thread governor by forcing it to accept a different
       max-thread count, ignoring the number of CPUs. requires system object.
       use snorkel_get_sys to retrieve the system object.

       snorkel_obj_set (snorkel_obj_t system_object,
       snorkel_attrib_thread_governor,
       int max_threads)

       note: use sparingly -- to many threads can degrade performance.
     */
    snorkel_attrib_query,
    /*
       usage:
       retrieve query string value pairs.

       snorkel_obj_get (snorkel_obj_t http_request_object,
       snorkel_attrib_query,
       char *variable,
       char *value,
       int size_of_value_buffer)
     */
    snorkel_attrib_keepalive,
    /*
       usage:
       sets the maximum number of keep-alive requests a handler will process on a
       single connections. (refer to http protocol for a better description). note:
       keep-alive is disabled by default.  note: if used, must be called prior to
       starting the server using snorkel_obj_start.

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_keepalive,
       int max_number_of_requests)
     */
    snorkel_attrib_keepalive_timeout,
    /*
       usage:
       set the maximum time in seconds a thread will keep a connection open
       waiting for new requests when keep-alive is enabled. note: if used,
       must be called prior to starting the server using snorkel_obj_start,
       default (15 seconds)

       snorkel_obj_set (snorkel_obj_t server_object,
       snorkel_attrib_keepalive_timeout,
       int seconds)
     */
    snorkel_attrib_scrub_mem,
    /*
       usage:
       when thread heap storage memory is cycled identifies whether memory
       is to be swipped clean.  note: slows performance.  operates on system
       object... use snorkel_get_sys to retrieve system object.

       snorkel_obj_set (snorkel_obj_t system_object,
       snorkel_attrib_scrub_mem,
       int flag (enabled=1, disabled=0))
     */

    snorkel_attrib_sendfile_threshold,
    /*
       usage:
       sets the minimum file size in bytes for which sendfile (i.e. Zero-copy file transfers) can be used.
       note: requires system object.  default file size is 1024000.

       snorkel_obj_set (snorkel_obj_t system_object,
       snorkel_attrib_sendfile_threshold,
       int minimum_size_in_bytes)
     */
    snorkel_attrib_file_info_refresh,
    /*
       usage:
       describes the frequency in seconds for which the server will check for changes in
       a files status.  Higher values improve overall performance.  Use higher values when
       file data rarely changes and lower if file data changes frequently.  The default is 10 seconds.

       snorkel_obj_set (snorkel_obj_t system_object,
       snorkel_attrib_file_info_refresh,
       int time_in_seconds)
     */
    snorkel_attrib_secure_children,
    /*
       usage:
       specifies whether sub-directories of a restricted directory inherit the restrictions of the parent
       directory when an .htaccess file is not present in the subdirectory. note: can affect performance
       with deep tree structures. (default disabled)

       snorkel_obj_set (snorkel_obj_t system_object,
       snorkel_attrib_secure_children,
       int flag (enabled=1, disabled=0))
     */

    snorkel_attrib_cbprintf,
      /*
         usage:
         by default, calling snorkel_printf using a raw stream object always proceeds the output string
         with a numerical value identifying the length of the string.  This attribute provides a means
         to disable or enable the feature.

         snorkel_obj_set (snorkel_obj_t stream,
         snorkel_attrib_cbprintf,
         int flag (enabled=1, disabled=0)

       */
    snorkel_attrib_get_ipaddr,
       /*
         usage:
          retrieve the ip address of the connected system -- the system on the other end
          of the connection.

          snorkel_obj_get (snorkel_obj_t stream,
                           snorkel_attrib_get_ipaddr,
                           char *buffer,
                           int cbbuffer)
          returns SNORKEL_SUCCESS on success and the size of the required buffer on failure.

        */
    snorkel_attrib_file_caching,
	   /*
	     usage:
		  enable/disable file caching - note enabled by default
		  snorkel_obj_set (snorkel_obj_t sys,
		                   snorkel_attrib_file_caching,
						   int enabled=1|disabled=0)
		*/
	snorkel_attrib_ssl_cypher_list,
	    /*
		   usage:
		    changes the SSL cypher list from the default ""AES:ALL:!aNULL:!eNULL:+RC4:@STRENGTH"
			to a user specified value
			snorkel_obj_set (snorkel_obj_t sys,
			                 snorkel_attrib_ssl_cypher_list,
							 char *list)
			note: only supported for SSL enabled runtime... also can only be set once per execution
			instance
		*/
#define AUTH_METHOD_DIGEST 0x01  /* MD5 METHOD */
#define AUTH_METHOD_BASIC  0x02  /* Base 64 encoded method */
	snorkel_attrib_auth_method,
	    /*
		  usage:
		    set the http authentication method: AUTH_METHOD_BASIC for basic or AUTH_METHOD_DIGEST for DIGEST
			snorkel_obj_set (snorkel_obj_t sys,
			                 snorkel_attrib_auth_method,
							 AUTH_METHOD_DIGEST || AUTH_METHOD_BASIC)

			note: AUTH_METHOD_DIGEST, the securer of the two is the default.
		*/
	snorkel_attrib_http_realm,
	    /*
		   usage:
		     instructs client on which set of passwords to use...  defines their realm.
			 snorkel_obj_set (snorkel_obj_t sys,
			                  snorkel_attrib_http_realm,
							  char *realm_name)
			note: realm_name can include spaces.
		*/
	snorkel_attrib_http_user,
	    /*
		  usage: edits an htaccess files
		  snorkel_obj_set (snorkel_obj_t sys,
		                   snorkel_attrib_http_user,
						   char *haccess_file,
						   char *username,
						   char *passwd (leave null if del entry is one otherwise password must be less than 64 bytes)
						   int del (set this to 1 to remove an entry) )
		*/
	snorkel_attrib_get_sys_log,
#define HTTPS_READ            0x01
#define HTTP_READ             0x02
#define RESTRICT_PORT         0x04
	snorkel_attrib_uri_access,
	/*
	   usage: controls access type for a particular uri.  if access is HTTPS_READ only
	   ports that use HTTPS can be used to access the associated resource. access can
	   be either HTTPS_READ,HTTP_READ, or HTTPS_READ|HTTP_READ.
	   snorkel_obj_set (snorkel_obj_t server_object,
	                    snorkel_attrib_uri_access,
	                    char *uri,
						int access)
	*/
	snorkel_attrib_uri_ext
  } snorkel_attrib_t;

  typedef void *lpvoid_t;

 typedef struct
 {
	 long min;
	 long max;
	 long net;
 } metric_t;
typedef struct
{
	clock_t min;
	clock_t max;
	clock_t net;
} metric_clock_t;

 typedef struct
 {
	 size_t http_samples;
	 size_t proto_samples;
	 metric_t heap_alloc;
	 metric_t outof_heap_alloc;
	 metric_t rcv;
	 metric_t snd;
	 metric_clock_t protocol;
	 metric_clock_t http;
	 time_t t;
 } metrics_t, *lpmetrics_t;

typedef struct
{
	long id;
	metrics_t m;
} snorkel_thread_metrics_t,
*lpsnorkel_thread_metrics_t;


#ifndef SNORKEL_C

#define SNORKEL_UUENCODE 0x02
#define SNORKEL_BINARY    0x04
#define SNORKEL_USE_SENDFILE 0x08
#define SNORKEL_FILE_SEND    0x10
#define SNORKEL_FILE_RECEIVE 0x20


#if defined(WIN32) && !defined(RUNTIME_EXPORTS)

  void __declspec (dllimport) snorkel_debug (int);
  snorkel_obj_t __declspec (dllimport)
    snorkel_get_sys ();

  lpchar_t __declspec (dllimport)
	  snorkel_time_to_metric_time(time_t, char *, size_t, int);

 int __declspec (dllimport)
  snorkel_server_get_metrics (snorkel_obj_t ,
                              void * ,
					          size_t ,
					          size_t,
					          int *);
  int __declspec (dllimport)
   snorkel_smtp_message (char *,
					     int ,
					    char *,
					    char *,
					    char *,
					    char *,
					    ...);
  /* syntax:
     snorkel_smtp_message ( char *smtp_server,
     int port,
     char *from,
     char *to,
     char *format,
     arg0,
     arg1,
     ...) */

  void __declspec (dllimport)
	  snorkel_thread_sleep (size_t );
  int __declspec (dllimport)
    snorkel_file_stream (snorkel_obj_t, char *,
                         size_t, int);
  lpvoid_t __declspec (dllimport)
    snorkel_mem_alloc (size_t);
  void __declspec (dllimport)
    snorkel_mem_free (void *);
  int __declspec (dllimport) snorkel_init ();
  int __declspec (dllimport)
    snorkel_worker_task (snorkel_obj_t,
                         snorkel_task_t, void *,
                         size_t);

  call_status_t __declspec (dllimport)
    snorkel_make_return_value (char *);
  byte_t __declspec (dllimport)
    snorkel_thread_continue (void *);
  int __declspec (dllimport)
    snorkel_mutex_lock (snorkel_obj_t);
  int __declspec (dllimport)
    snorkel_mutex_unlock (snorkel_obj_t);
  int __declspec (dllimport)
    snorkel_event_set (snorkel_obj_t);
  int __declspec (dllimport)
    snorkel_event_wait (snorkel_obj_t);
  int __declspec (dllimport)
    snorkel_event_waittimed (snorkel_obj_t, long);
  int __declspec (dllimport)
    snorkel_obj_destroy (snorkel_obj_t);
  int __declspec (dllimport)
    snorkel_obj_set (snorkel_obj_t, ...);
  int __declspec (dllimport)
    snorkel_obj_get (snorkel_obj_t, ...);
  int __declspec (dllimport)
    snorkel_obj_start (snorkel_obj_t);
  int __declspec (dllimport)
    snorkel_printf (snorkel_obj_t, ...);
  int __declspec (dllimport)
    snorkel_uprintf (snorkel_obj_t, ...);
  int __declspec (dllimport)
    snorkel_rcv (snorkel_obj_t, byte_t *, size_t);
  int __declspec (dllimport)
    snorkel_marshal (snorkel_obj_t, ...);
  int __declspec (dllimport)
    snorkel_unmarshal (snorkel_obj_t, ...);
  int __declspec (dllimport)
    snorkel_put (snorkel_obj_t, byte_t *, size_t);
  int __declspec (dllimport)
    snorkel_putf (snorkel_obj_t, byte_t *,
                  size_t);
  lpbyte_t __declspec (dllimport)
    snorkel_getf (snorkel_obj_t, int *);
  snorkel_obj_t __declspec (dllimport)
    snorkel_obj_create (snorkel_obj_type_t, ...);
  int __declspec (dllimport)
    snorkel_bubble_load (snorkel_obj_t, char *);
  int __declspec (dllimport)
    snorkel_bubble_pinfo (char *);
  int __declspec (dllimport)
    snorkel_bubble_assign_prop (char *, short,
                                char *, ...);
  int __declspec (dllimport)
    snorkel_version (char *,
				     size_t *);
#else

  int snorkel_version (char *, size_t *);
  lpchar_t snorkel_time_to_metric_time (time_t, char *, size_t, int);
  void snorkel_debug (int);
  snorkel_obj_t snorkel_get_sys ();
  void snorkel_thread_sleep (size_t ); /* thread-safe sleep */
  int snorkel_init ();
  int snorkel_server_get_metrics (snorkel_obj_t ,
                     void * ,
					 size_t ,
					 size_t ,
					 int *);
  int snorkel_lock (snorkel_obj_t);
  int snorkel_file_stream (snorkel_obj_t, char *,
                           size_t, int);
  int snorkel_worker_task (snorkel_obj_t,
                           snorkel_task_t, void *,
                           size_t);
  call_status_t snorkel_make_return_value (char
                                           *);
  lpvoid_t snorkel_mem_alloc (size_t);
  int snorkel_smtp_message (char *, int ,
                            char *, char *,
                            char *,  char *,
                            ...);
  void snorkel_mem_free (void *);
  int snorkel_unlock (snorkel_obj_t);
  int snorkel_event_set (snorkel_obj_t);
  int snorkel_event_wait (snorkel_obj_t);
  int snorkel_event_waittimed (snorkel_obj_t,
                               long);
  int snorkel_obj_destroy (snorkel_obj_t);
  byte_t snorkel_thread_continue (void *);
  int snorkel_obj_set (snorkel_obj_t, ...);
  int snorkel_obj_get (snorkel_obj_t, ...);
  int snorkel_obj_start (snorkel_obj_t);
  int snorkel_printf (snorkel_obj_t, ...);
  int snorkel_uprintf (snorkel_obj_t, ...);
  int snorkel_rcv (snorkel_obj_t, byte_t *,
                   size_t);
  int snorkel_marshal (snorkel_obj_t, ...);
  int snorkel_unmarshal (snorkel_obj_t, ...);
  int snorkel_put (snorkel_obj_t, byte_t *,
                   size_t);
  int snorkel_putf (snorkel_obj_t, byte_t *,
                    size_t);
  byte_t *snorkel_getf (snorkel_obj_t, int *);
    snorkel_obj_t
    snorkel_obj_create (snorkel_obj_type_t, ...);
  int snorkel_bubble_load (snorkel_obj_t, char *);
  int snorkel_bubble_pinfo (char *);
  int snorkel_bubble_assign_prop (char *, short,
                                  char *, ...);
#endif

#endif

/* HTTP file types */
#define FILE_HTML "text/html"
#define FILE_TEXT "text/plain"
#define FILE_RICHTEXT "text/richtext"
#define FILE_RTF "text/rtf"
#define FILE_XML "text/xml"
#define FILE_JPEG "image/jpeg"
#define FILE_GIF "image/gif"
#define FILE_PNG "image/png"
#define FILE_TIFF "image/tiff"
#define FILE_AUDIO_MIDI "audio/midi"
#define FILE_AUDIO_MPEG "audio/mpeg"
#define FILE_AUDIO_WAV  "audio/x-wav"
#define FILE_VIDEO_MPEG "video/mpeg"
#define FILE_VIDEO_QUICKTIME "video/quicktime"
#define FILE_EXCEL "application/vnd.ms-excel"
#define FILE_PDF "application/pdf"
#define FILE_POSTSCRIPT "application/postscript"
#define FILE_POWERPOINT "application/mspowerpoint"
#define FILE_ZIP "application/zip"
#define FILE_VRML "model/vrml"
#define FILE_CSS "text/css"
#define FILE_PSD "image/psd"
#define FILE_ICON "image/vnd.microsoft.icon"
#define FILE_WMV "video/x-ms-wmv"
#define FILE_ASF FILE_WMV
#define FILE_ASX FILE_WMV
#define FILE_WVX "video/x-ms-wvx"
#define FILE_WM "video/x-ms-wm"
#define FILE_WMX "video/x-ms-wmx"
#ifdef __cplusplus
}
#endif


#endif
