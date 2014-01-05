/*
 *      ofpsvr.h
 *      
 *      Copyright 2011 P.S.V.R <ofpsvr.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifndef OFPSVR_H_ICWCQ40G
#define OFPSVR_H_ICWCQ40G

#define NDEBUG
#define WRITELOG(...) printf("{%d} ",(int)time(0));printf(__VA_ARGS__);fflush(stdout);


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>

#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <microhttpd.h>

#include <mysql/mysql.h>
#include <assert.h>
#include <setjmp.h> 


#define POSTBUFFERSIZE  512
#define OFPSVR_VERSION 8901
#define OFPSVR_PID_FILE "/var/run/ofpsvr.pid"
#define OFPSVR_LOG_FILE "/var/log/ofpsvr.log"

#include "db_config.h"

struct Comment {
	char *content;
	struct Comment *next;
};

struct Resource {
	char *filename;
	unsigned long sz;
	struct MHD_Response *response;
	struct Resource *next;
};

struct Article {
	long posted_at;
	char *rfc_posted_at;
	
	pthread_mutex_t hit_related_lock;
	int hit_count;
	
	struct Resource *resources;
	int resource_count;
	
	char *title;
	char *introduction;
	char *body;
	
	pthread_mutex_t comment_related_lock;
	int comment_count;
	struct Comment *comments;
	struct MHD_Response *response;
	size_t page_sz;
	
	//for safety
	uint32_t last_posted_client;
	int last_posted_client_cnt;
};

struct Article **articles;
unsigned long articles_len;

struct Resource *resources;

struct MHD_Response *response_404;
struct MHD_Response *response_500;
struct MHD_Response *response_index;

unsigned long cache_size;
int cache_size_silent;
int running;

//Global Functions

void add_comment(struct Article *a,struct Comment *c);

char *fill_content(char *name,char *email,char *website,char *body, long posted_at, int number);

int regenerate(struct Article *x,int id);

struct MHD_Response *generate_blog_response();
struct MHD_Response *generate_blog_response_rss();

int handler (void *cls, struct MHD_Connection *connection,
			 const char *url, const char *method,
			 const char *version, const char *upload_data,
			 size_t *upload_data_size, void **con_cls);

void request_completed (void *cls, struct MHD_Connection *connection,
						void **con_cls, enum MHD_RequestTerminationCode toe);

#endif

