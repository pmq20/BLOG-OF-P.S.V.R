/*
 *      main.c
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


#include "ofpsvr.h"
jmp_buf main_loop;

static char *utime2rfctime(long u)
{
	struct tm broken_down_time;
	if(&broken_down_time != gmtime_r(&u,&broken_down_time)){
		WRITELOG("gmtime_r failed\n");
		return NULL;
	}
	size_t sz = 100*sizeof(char);
	char *buffer;
	cache_size += sz;
	if(!(buffer = malloc(sz))){
		WRITELOG("malloc failed\n");
		return NULL;
	}
	if(!strftime(buffer, sz, "%a, %d %b %Y %H:%M:%S %z", &broken_down_time)){
		WRITELOG("strftime failed\n");
		return NULL;
	}
	assert(buffer);
	return buffer;
}
void terminate(int sig){
	if(running){
		running = 0;
		longjmp (main_loop, 1);
	}
}
int main (int argc, const char * argv[]) {
	
	if(0!=getuid()){
		WRITELOG("Please be root to run this program.\n");
		return EXIT_FAILURE;
	}
	if (access(OFPSVR_PID_FILE, F_OK) == 0)
	{
		FILE *fp = fopen(OFPSVR_PID_FILE,"r");
		int existing_pid;
		if(1!=fscanf(fp,"%d",&existing_pid)){
			WRITELOG("fscanf failed.\n");
			return EXIT_FAILURE;
		}
		if(kill(existing_pid, 0) == 0){
			WRITELOG("ofpsvr is already running!\n");
			return EXIT_FAILURE;
		}else{
			if(remove(OFPSVR_PID_FILE)<0){
				WRITELOG(OFPSVR_PID_FILE" cannot be deleted!\n");
				return EXIT_FAILURE;
			}
		}
		fclose(fp);
	}
	
	
      pid_t pid, sid;
      pid = fork();
      if (pid < 0) {
		WRITELOG("fork failed!\n");
              exit(EXIT_FAILURE);
      }
      if (pid > 0) {
		WRITELOG("ofpsvr started.\n");
              exit(EXIT_SUCCESS);
      }
      umask(0);
      sid = setsid();
      if (sid < 0) {
		WRITELOG("setsid failed!\n");
              exit(EXIT_FAILURE);
      }
      if ((chdir("/")) < 0) {
		WRITELOG("chdir failed!\n");
              exit(EXIT_FAILURE);
      }

	FILE *fp = fopen(OFPSVR_PID_FILE,"w");
	fprintf(fp,"%d\n",getpid());
	fclose(fp);

       close(STDIN_FILENO);
	if(!freopen(OFPSVR_LOG_FILE,"a+",stdout)){
		WRITELOG("freopen %s failed!\n",OFPSVR_LOG_FILE);
		exit(EXIT_FAILURE);
	}
       close(STDERR_FILENO);
	
	WRITELOG("______________OFPSVR.COM______Server______________\n");
	WRITELOG("Version %d;\n",OFPSVR_VERSION);
	WRITELOG("Linked with libmicrohttpd version %x;\n",MHD_VERSION);
	WRITELOG("Written by P.S.V.R of snailize team.\n");
	WRITELOG("__________________________________________________\n\n");
	
	MYSQL my;
	struct MHD_Daemon *d;
	size_t sz;
	cache_size = 0;
	cache_size_silent = 1;

	//0.Connecting DB
	printf("Connecting DB...");fflush(stdout);
	if(!mysql_init(&my)){
		WRITELOG("mysql_init failed!\n");
		return EXIT_FAILURE;
	}
	if(!mysql_real_connect(&my, OFPSVR_DB_HOST, OFPSVR_DB_USER, OFPSVR_DB_PASSWD, OFPSVR_DB_DB, 0, NULL, 0)){
		WRITELOG("mysql_real_connect failed!\n");
		if(mysql_errno(&my)){
			WRITELOG("error %d: %s\n",mysql_errno(&my),mysql_error(&my));
		}
		return EXIT_FAILURE;
	}
	if(mysql_set_character_set(&my, "utf8")){
		WRITELOG("mysql_set_character_set failed!");
		return EXIT_FAILURE;
	}
	printf("OK\n");




	//1.Retrieving Articles
	if(mysql_query(&my, "SELECT * FROM ofpsvr_articles ORDER BY id")){
		WRITELOG("SELECT error: %s\n",mysql_error(&my));
		return EXIT_FAILURE;
	}
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;
	if(!(res_ptr=mysql_store_result(&my))){
		WRITELOG("Retrieve error: %s\n",mysql_error(&my));
		return EXIT_FAILURE;
	}
	articles_len = (unsigned long)mysql_num_rows(res_ptr);
	sz = articles_len*sizeof(struct Article*);
	cache_size += sz;
	if(!(articles = malloc(sz))){
	   WRITELOG("malloc failed when allocating mem-space for %lu struct Article pointers.\n",articles_len);
	   return EXIT_FAILURE;
	}
	int i,parsed_article_id;
	for (i=0; i<articles_len; ++i) {
		printf("Retrieving Article #%d...",i);fflush(stdout);
		sz = sizeof(struct Article);
		cache_size += sz;
		if(!(articles[i] = calloc(1,sz))){
			WRITELOG("calloc failed when allocating mem-space for Article #%d.\n",i);
			return EXIT_FAILURE;
		}
		sqlrow = mysql_fetch_row(res_ptr);
		sscanf(sqlrow[0], "%d", &parsed_article_id);
		if(i != parsed_article_id){WRITELOG("inconsecutive id discovered at Article #%d (actual id is %d).\n",i,parsed_article_id);return EXIT_FAILURE;}
		sscanf(sqlrow[1], "%ld", &articles[i]->posted_at);
		sscanf(sqlrow[2], "%d", &articles[i]->hit_count);
		if(!(articles[i]->title = strdup(sqlrow[3]))){WRITELOG("strdup failed for Article #%d.\n",i);return EXIT_FAILURE;}
		if(!(articles[i]->introduction = strdup(sqlrow[4]))){WRITELOG("strdup failed for Article #%d.\n",i);return EXIT_FAILURE;}
		if(!(articles[i]->body = strdup(sqlrow[5]))){WRITELOG("strdup failed for Article #%d.\n",i);return EXIT_FAILURE;}
		cache_size += (strlen(sqlrow[3])+1)*sizeof(char);
		cache_size += (strlen(sqlrow[4])+1)*sizeof(char);
		cache_size += (strlen(sqlrow[5])+1)*sizeof(char);
		if(pthread_mutex_init(&(articles[i]->hit_related_lock),NULL)){WRITELOG("pthread_mutex_init hit_related_lock failed for Article #%d.\n",i);return EXIT_FAILURE;}
		if(pthread_mutex_init(&(articles[i]->comment_related_lock),NULL)){WRITELOG("pthread_mutex_init comment_related_lock failed for Article #%d.\n",i);return EXIT_FAILURE;}
		if(!(articles[i]->rfc_posted_at = utime2rfctime(articles[i]->posted_at))){WRITELOG("utime2rfctime failed for Article #%d.\n",i);return EXIT_FAILURE;}
		printf("OK\n");
	}
	mysql_free_result(res_ptr);




	//2.Retrieving Comments
	if(mysql_query(&my, "SELECT * FROM ofpsvr_comments ORDER BY id")){
		WRITELOG("SELECT error: %s\n",mysql_error(&my));
		return EXIT_FAILURE;
	}
	if(!(res_ptr=mysql_use_result(&my))){
		WRITELOG("Retrieve error: %s\n",mysql_error(&my));
		return EXIT_FAILURE;
	}
	struct Comment *comment_ptr;
	int parsed_comment_id;
	long parsed_comment_posted_at;
	while((sqlrow = mysql_fetch_row(res_ptr)))
	{
		sz = sizeof(struct Comment);
		cache_size += sz;
		if(!(comment_ptr = calloc(1,sz))){
			WRITELOG("calloc failed when allocating mem-space for a single struct Comment.\n");
			return EXIT_FAILURE;
		}
		sscanf(sqlrow[0], "%d", &parsed_comment_id);
		printf("Retrieving Comment #%d...",parsed_comment_id);fflush(stdout);
		sscanf(sqlrow[1], "%d", &parsed_article_id);
		if(!(parsed_article_id>=0 && parsed_article_id<articles_len)){
			WRITELOG("Bad article id %d encounted when handling comment #%d.\n",parsed_article_id,parsed_comment_id);
			return EXIT_FAILURE;
		}
		sscanf(sqlrow[2], "%ld", &parsed_comment_posted_at);
		if(!(comment_ptr->content = fill_content(sqlrow[3], sqlrow[4], sqlrow[5], sqlrow[6], parsed_comment_posted_at, articles[parsed_article_id]->comment_count+1)))
		{
			WRITELOG("fill_content failed when handling comment #%d.\n",parsed_comment_id);
			return EXIT_FAILURE;
		}
		add_comment(articles[parsed_article_id],comment_ptr);
		printf("OK\n");
	}
	mysql_free_result(res_ptr);





	//3.Retrieving Resources
	if(mysql_query(&my, "SELECT * FROM ofpsvr_resources ORDER BY article_id")){
		WRITELOG("SELECT error: %s\n",mysql_error(&my));
		return EXIT_FAILURE;
	}
	if(!(res_ptr=mysql_use_result(&my))){
		WRITELOG("Retrieve error: %s\n",mysql_error(&my));
		return EXIT_FAILURE;
	}
	struct Resource *resource_ptr;
	int parsed_resource_id;
	unsigned long *lengths;
	while((sqlrow = mysql_fetch_row(res_ptr)))
	{
		printf("Retrieving Resource (%s)`%s`...",sqlrow[1],sqlrow[2]);fflush(stdout);
		sz = sizeof(struct Resource);
		cache_size += sz;
		if(!(resource_ptr = calloc(1,sz))){
			WRITELOG("calloc failed when allocating mem-space for a single struct Resource.\n");
			return EXIT_FAILURE;
		}
		sscanf(sqlrow[0], "%d", &parsed_resource_id);
		if(!(lengths = mysql_fetch_lengths(res_ptr))){
			WRITELOG("mysql_fetch_lengths failed at Resource #%d.\n",parsed_resource_id);
			return EXIT_FAILURE;
		}
		if(!(resource_ptr->filename = strdup(sqlrow[2]))){WRITELOG("strdup failed for Resource #%d.\n",parsed_resource_id);return EXIT_FAILURE;}
		cache_size += (strlen(sqlrow[2])+1)*sizeof(char);
		resource_ptr->sz = lengths[4];
		cache_size += lengths[4];
		if(!(resource_ptr->response = MHD_create_response_from_data ((size_t)lengths[4],(void*)sqlrow[4], MHD_YES, MHD_YES))){
			WRITELOG("MHD_create_response_from_data failed at Resource #%d.\n",parsed_resource_id);
			return EXIT_FAILURE;
		}
		if(MHD_NO == MHD_add_response_header (resource_ptr->response, MHD_HTTP_HEADER_CONTENT_TYPE, sqlrow[3])){
			WRITELOG("MHD_add_response_header failed at Resource #%d.\n",parsed_resource_id);
			return EXIT_FAILURE;
		}
		sscanf(sqlrow[1], "%d", &parsed_article_id);
		if(-1 == parsed_article_id){
			resource_ptr->next = resources;
			resources = resource_ptr;
			if(0==strcmp(resource_ptr->filename,"404.html")){
				response_404 = resource_ptr->response;
			}else if(0==strcmp(resource_ptr->filename,"500.html")){
				response_500 = resource_ptr->response;
			}else if(0==strcmp(resource_ptr->filename,"index.html")){
				response_index = resource_ptr->response;
			}
		}else if(parsed_article_id>=0 && parsed_article_id<articles_len){
			resource_ptr->next = articles[parsed_article_id]->resources;
			articles[parsed_article_id]->resources = resource_ptr;
			++articles[parsed_article_id]->resource_count;
		}else{
			WRITELOG("Bad article id %d encounted when handling Resource #%d.\n",parsed_article_id,parsed_resource_id);
			return EXIT_FAILURE;
		}
		printf("OK(%lfKB)\n",(double)(lengths[4])/1000);
	}
	if(!response_404){
		WRITELOG("Please provide a 404.html resource for root.\n");
		return EXIT_FAILURE;
	}
	if(!response_500){
		WRITELOG("Please provide a 500.html resource for root.\n");
		return EXIT_FAILURE;
	}
	if(!response_index){
		WRITELOG("Please provide a index.html resource for root.\n");
		return EXIT_FAILURE;
	}
	mysql_free_result(res_ptr);
	mysql_close(&my);




	//4.Regenerating Articles
	for (i=0; i<articles_len; ++i) {
		printf("Regenerating Aritcle #%d...",i);fflush(stdout);
		if(!regenerate(articles[i],i)){
			WRITELOG("regenerate failed for article #%d.\n",i);
			return EXIT_FAILURE;
		}
		printf("OK\n");
	}
	WRITELOG("Cache Size: %lf MB\n",(double)cache_size/1000000);
	cache_size_silent = 0;


	//5.Statring the daemon
		d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,OFPSVR_VERSION,NULL,NULL,
											&handler,NULL,
											MHD_OPTION_NOTIFY_COMPLETED,request_completed,
											NULL,MHD_OPTION_END);
	if(NULL == d)
	{
		WRITELOG("MHD Daemon cannot be started.\n");
		return EXIT_FAILURE;
	}
	WRITELOG("MHD Daemon started.\n");
	running = 1;
	(void) signal(SIGINT,terminate);
        (void) signal(SIGTERM,terminate);
        (void) signal(SIGHUP,terminate);

	setjmp (main_loop);
	
       while (running) {
          sleep(30);
       }


	//6.Shutting down
	WRITELOG("OFPSVR.COM Server Shutting down\n");
	MHD_stop_daemon(d);
	WRITELOG("MHD Daemon stopped.\n");
	for (i=0; i<articles_len; ++i) {
		WRITELOG("articles[%d]->hit_count = %d\n", i, articles[i]->hit_count);
	}
	WRITELOG("Now dumping hit_count data\n");
	printf("Connecting DB...");fflush(stdout);
	if(!mysql_init(&my)){
		WRITELOG("mysql_init failed!\n");
		return EXIT_FAILURE;
	}
	if(!mysql_real_connect(&my, OFPSVR_DB_HOST, OFPSVR_DB_USER, OFPSVR_DB_PASSWD, OFPSVR_DB_DB, 0, NULL, 0)){
		WRITELOG("mysql_real_connect failed!\n");
		if(mysql_errno(&my)){
			WRITELOG("error %d: %s\n",mysql_errno(&my),mysql_error(&my));
		}
		return EXIT_FAILURE;
	}
	if(mysql_set_character_set(&my, "utf8")){
		WRITELOG("mysql_set_character_set failed!");
		return EXIT_FAILURE;
	}
	printf("OK\n");
	for (i=0; i<articles_len; ++i) {
		printf("Dumping #%d...",i);fflush(stdout);
		char *sql;
		if(asprintf(&sql,"UPDATE ofpsvr_articles SET `hit_count` = '%d' WHERE `id`=%d LIMIT 1", articles[i]->hit_count, i) < 0){
			WRITELOG("asprintf failed!");
			continue;
		}
		if(mysql_query(&my, sql)){
			WRITELOG("UPDATE error %d: %s\n",mysql_errno(&my),mysql_error(&my));
			free(sql);
			continue;
		}
		free(sql);
		if(0 == mysql_affected_rows(&my)){
			printf("OK but didn't change\n");
		}else if(mysql_affected_rows(&my) > 1){
			WRITELOG("mysql_affected_rows > 1!\n");
		}else{
			printf("OK\n");
		}
	}
	mysql_close(&my);
	WRITELOG("Bye.\n");
	return EXIT_SUCCESS;
}
