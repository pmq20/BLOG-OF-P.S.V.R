/*
 *      handler.c
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
#define OFPSVR_Q_404 MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response_404)
#define OFPSVR_Q_500 MHD_queue_response(connection, MHD_HTTP_INTERNAL_SERVER_ERROR, response_500)

static int append_string(char **old,const char *data)
{
	char *brand_new;
	if(asprintf(&brand_new,"%s%s", (*old)?(*old):"",data?data:"") < 0){
		return 0;
	}
	if(*old) free(*old);
	*old = brand_new;
	return 1;
}

static int send_page(struct MHD_Connection *connection,char *content){
	int ret;
	struct MHD_Response *response;
  	if (!(response = MHD_create_response_from_data (strlen (content), (void *) content, MHD_NO, MHD_YES)))
		return MHD_NO;
	if(MHD_NO == MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html")){
		return MHD_NO;
	}
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  	MHD_destroy_response (response);
  	return ret;
}

static int sanitize(char **in){ //-1:fail / 0:NULL / 1:SUC
	if(NULL == *in) return 0;
	char *x, *ret, *old, *ptr1, *ptr2;
	
	//(1)"&"->"&amp;"
	if(!(x = strdup(*in)))
		return -1;
	ret = NULL;
	ptr1 = x;
	while(1){
		old = ret;
		if((ptr2 = strchr(ptr1,'&'))){
		   *ptr2 = '\0';
			if(asprintf(&ret,"%s%s&amp;",old?old:"",ptr1) < 0)
				return -1;
			ptr1 = ptr2 + 1;
		}else{
			if(asprintf(&ret,"%s%s",old?old:"",ptr1) < 0)
				return -1;
		}
		if(old)
			free(old);
		if(!ptr2)
			break;
	}
	free(x);
	
	//(2)"<"->"&lt;"
	assert(ret);
	if(!(x = strdup(ret)))
		return -1;
	ret = NULL;
	ptr1 = x;
	while(1){
		old = ret;
		if((ptr2 = strchr(ptr1,'<'))){
			*ptr2 = '\0';
			if(asprintf(&ret,"%s%s&lt;",old?old:"",ptr1) < 0)
				return -1;
			ptr1 = ptr2 + 1;
		}else{
			if(asprintf(&ret,"%s%s",old?old:"",ptr1) < 0)
				return -1;
		}
		if(old)
			free(old);
		if(!ptr2)
			break;
	}
	free(x);

	//(3)">"->"&gt;"
	assert(ret);
	if(!(x = strdup(ret)))
		return -1;
	ret = NULL;
	ptr1 = x;
	while(1){
		old = ret;
		if((ptr2 = strchr(ptr1,'>'))){
			*ptr2 = '\0';
			if(asprintf(&ret,"%s%s&gt;",old?old:"",ptr1) < 0)
				return -1;
			ptr1 = ptr2 + 1;
		}else{
			if(asprintf(&ret,"%s%s",old?old:"",ptr1) < 0)
				return -1;
		}
		if(old)
			free(old);
		if(!ptr2)
			break;
	}
	free(x);


	//(4)'"'->"&quot;"
	assert(ret);
	if(!(x = strdup(ret)))
		return -1;
	ret = NULL;
	ptr1 = x;
	while(1){
		old = ret;
		if((ptr2 = strchr(ptr1,'"'))){
			*ptr2 = '\0';
			if(asprintf(&ret,"%s%s&quot;",old?old:"",ptr1) < 0)
				return -1;
			ptr1 = ptr2 + 1;
		}else{
			if(asprintf(&ret,"%s%s",old?old:"",ptr1) < 0)
				return -1;
		}
		if(old)
			free(old);
		if(!ptr2)
			break;
	}
	free(x);
	

	//(5)'''->"&#39;"
	assert(ret);
	if(!(x = strdup(ret)))
		return -1;
	ret = NULL;
	ptr1 = x;
	while(1){
		old = ret;
		if((ptr2 = strchr(ptr1,'\''))){
			*ptr2 = '\0';
			if(asprintf(&ret,"%s%s&#39;",old?old:"",ptr1) < 0)
				return -1;
			ptr1 = ptr2 + 1;
		}else{
			if(asprintf(&ret,"%s%s",old?old:"",ptr1) < 0)
				return -1;
		}
		if(old)
			free(old);
		if(!ptr2)
			break;
	}
	free(x);
	
	
	//(6)" "->"&nbsp;"
	assert(ret);
	if(!(x = strdup(ret)))
		return -1;
	ret = NULL;
	ptr1 = x;
	while(1){
		old = ret;
		if((ptr2 = strchr(ptr1,' '))){
			*ptr2 = '\0';
			if(asprintf(&ret,"%s%s&nbsp;",old?old:"",ptr1) < 0)
				return -1;
			ptr1 = ptr2 + 1;
		}else{
			if(asprintf(&ret,"%s%s",old?old:"",ptr1) < 0)
				return -1;
		}
		if(old)
			free(old);
		if(!ptr2)
			break;
	}
	free(x);

	//(7)"\n"->"<br />"
	assert(ret);
	if(!(x = strdup(ret)))
		return -1;
	ret = NULL;
	ptr1 = x;
	while(1){
		old = ret;
		if((ptr2 = strchr(ptr1,'\n'))){
			*ptr2 = '\0';
			if(asprintf(&ret,"%s%s<br />",old?old:"",ptr1) < 0)
				return -1;
			ptr1 = ptr2 + 1;
		}else{
			if(asprintf(&ret,"%s%s",old?old:"",ptr1) < 0)
				return -1;
		}
		if(old)
			free(old);
		if(!ptr2)
			break;
	}
	free(x);
	
	assert(ret);
	*in = ret;
	return 1;
}

struct connection_info_struct
{
	int parsed_article_id;
	struct Comment *comment;
	struct MHD_PostProcessor *postprocessor;
	char *name,*email,*website,*body;
	long posted_at;
	enum {BAD_FORMAT,NAME_TOO_LONG,EMAIL_TOO_LONG,WEBSITE_TOO_LONG,BODY_TOO_LONG,BLANK} error_type;
	int finished;
};

static int iterate_new_comment (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
					   const char *filename, const char *content_type,
					   const char *transfer_encoding, const char *data, uint64_t off,
					   size_t size)
{
	if(size>0)
	{
		assert(coninfo_cls);
		struct connection_info_struct *con_info = (struct connection_info_struct *) coninfo_cls;
		assert(con_info);
		if(0 == strcmp (key, "reply[name]"))
		{
			if(!append_string(&(con_info->name),data)){
				return MHD_NO;
			}
			if(strlen(con_info->name) > 100){
				con_info->error_type = NAME_TOO_LONG;
				return MHD_NO;
			}
		}
		else if(0 == strcmp (key, "reply[email]"))
		{
			if(!append_string(&(con_info->email),data)){
				return MHD_NO;
			}
			if(strlen(con_info->email) > 100){
				con_info->error_type = EMAIL_TOO_LONG;
				return MHD_NO;
			}
		}
		else if(0 == strcmp (key, "reply[website]"))
		{
			if(!append_string(&(con_info->website),data)){
				return MHD_NO;
			}
			if(strlen(con_info->website) > 100){
				con_info->error_type = WEBSITE_TOO_LONG;
				return MHD_NO;
			}
		}
		else if(0 == strcmp (key, "reply[body]"))
		{
			if(!append_string(&(con_info->body),data)){
				return MHD_NO;
			}
			if(strlen(con_info->body) > 1000){
				con_info->error_type = BODY_TOO_LONG;
				return MHD_NO;
			}
		}
		else if (0 == strcmp (key, "commit"))
		{
			if(!con_info->name || !con_info->email || !con_info->body)
			{
				con_info->error_type = BLANK;
				return MHD_NO;
			}
			   
			if(sanitize(&con_info->name) < 0) return MHD_NO;
			if(sanitize(&con_info->email) < 0) return MHD_NO;
			if(sanitize(&con_info->website) < 0) return MHD_NO;
			if(sanitize(&con_info->body) < 0) return MHD_NO;
			
			
			//________________store__________________
			MYSQL mm;
			if(!mysql_init(&mm)) return MHD_NO;
			if(!mysql_real_connect(&mm, OFPSVR_DB_HOST, OFPSVR_DB_USER, OFPSVR_DB_PASSWD, OFPSVR_DB_DB, 0, NULL, 0)) return MHD_NO;
			if(mysql_set_character_set(&mm, "utf8")) return MHD_NO;
			
			char *name_escape,*email_escape,*website_escape,*body_escape;
			if(!(  name_escape = malloc((strlen(con_info->name)*2+10)*sizeof(char))  )) return MHD_NO;
			if(mysql_real_escape_string(&mm,name_escape,con_info->name,strlen(con_info->name)) <= 0) return MHD_NO;
			if(!(  email_escape = malloc((strlen(con_info->email)*2+10)*sizeof(char))  )) return MHD_NO;
			if(mysql_real_escape_string(&mm,email_escape,con_info->email,strlen(con_info->email)) <= 0) return MHD_NO;
			if(con_info->website){
				if(!(  website_escape = malloc((strlen(con_info->website)*2+10)*sizeof(char))  )) return MHD_NO;
				if(mysql_real_escape_string(&mm,website_escape,con_info->website,strlen(con_info->website)) <= 0) return MHD_NO;
			}
			if(!(  body_escape = malloc((strlen(con_info->body)*2+10)*sizeof(char))  )) return MHD_NO;
			if(mysql_real_escape_string(&mm,body_escape,con_info->body,strlen(con_info->body)) <= 0) return MHD_NO;
			
			char *sql;
			if(asprintf(&sql,"INSERT INTO comments(`id` ,`article_id` ,`posted_at` ,`name` ,`email` ,`website` ,`body`) VALUES"
												   "(NULL,   '%d'     ,   '%ld'    , '%s'  , '%s'   , '%s'     , '%s')",
					 con_info->parsed_article_id,con_info->posted_at,name_escape,email_escape,
					 (con_info->website)?website_escape:"",body_escape) < 0) return MHD_NO;
			assert(name_escape);free(name_escape);
			assert(email_escape);free(email_escape);
			if(con_info->website){assert(website_escape);free(website_escape);}
			assert(body_escape);free(body_escape);

			if(mysql_query(&mm, sql)){
				WRITELOG("INSERT error %d: %s\n",mysql_errno(&mm),mysql_error(&mm));
				return MHD_NO;
			}
			if(1!=mysql_affected_rows(&mm)) return MHD_NO;
			assert(sql);free(sql);
			mysql_close(&mm);
			//_______________________________________
			
			
			
			if(!(con_info->comment->content = fill_content(con_info->name, con_info->email, con_info->website, con_info->body, con_info->posted_at, articles[con_info->parsed_article_id]->comment_count+1))){
				return MHD_NO;
			}
			pthread_mutex_lock(&(articles[con_info->parsed_article_id]->comment_related_lock));
			add_comment(articles[con_info->parsed_article_id],con_info->comment);
			if(!regenerate(articles[con_info->parsed_article_id],con_info->parsed_article_id)){
				return MHD_NO;
			}
			pthread_mutex_unlock(&(articles[con_info->parsed_article_id]->comment_related_lock));
			con_info->finished = 1;
		    return MHD_NO;
		}else{
			return MHD_NO;
		}
	}
	return MHD_YES;
}

int handler (void *cls, struct MHD_Connection *connection,
			const char *url, const char *method,
			const char *version, const char *upload_data,
			size_t *upload_data_size, void **con_cls){
	if('G' == method[0]) { //GET
		char *u = (char *)url;
		
		if(strlen(u)>1 && '/' == u[strlen(u)-1]){
			u[strlen(u)-1] = '\0';
		}
		
		char *ptr,*ptr_another;

		int slash_count = 0;
		ptr = u;
		while(ptr = strchr(ptr,'/'))
			++slash_count,++ptr;
		
		char *extension = NULL;
		ptr = strrchr(u,'.');
		ptr_another = strrchr(u,'/');
		if(ptr && ptr_another && ptr > ptr_another)
		{
			if(!(extension = strdup(ptr+1))){
				WRITELOG("strdup failed in handler!");
				return OFPSVR_Q_500;
			}
		}
		
		switch (slash_count) {
			case 1:
				if('\0' == u[1]) { // "/"
					return MHD_queue_response(connection, MHD_HTTP_OK, response_index);
				}
				else if(strstr(u,"/blog")==u){ // "/blog";
					struct MHD_Response *response;
					if(extension && (0==strcmp("rss",extension)||0==strcmp("xml",extension))){
						if(!(response = generate_blog_response_rss())){
							return OFPSVR_Q_500;
						}
					}else{
						if(!(response = generate_blog_response())){
							return OFPSVR_Q_500;
						}
					}
					int ret;
					if(MHD_NO == (ret = MHD_queue_response (connection, MHD_HTTP_OK, response))){
						return OFPSVR_Q_500;
					}
					MHD_destroy_response (response);
					return ret;
				}
				break;
			case 2:
				ptr = strrchr(u,'/');
				if(!ptr) return OFPSVR_Q_404;
				++ptr;
				if('b' == u[1]) // "/blog/..."
				{
					int parsed_article_id;
					if(1!=sscanf(ptr, "%d", &parsed_article_id))
						return OFPSVR_Q_404;
					if(parsed_article_id>=0 && parsed_article_id<articles_len){
						pthread_mutex_lock(&(articles[parsed_article_id]->hit_related_lock));
						++articles[parsed_article_id]->hit_count;
						pthread_mutex_unlock(&(articles[parsed_article_id]->hit_related_lock));
						return MHD_queue_response(connection, MHD_HTTP_OK, articles[parsed_article_id]->response);
					}else{
						return OFPSVR_Q_404;
					}
				}
				else if('r' == u[1]) // "/resources/..."
				{
					struct Resource *resource_ptr;
					for (resource_ptr = resources; resource_ptr; resource_ptr = resource_ptr->next)
						if(0==strcmp(resource_ptr->filename,ptr))
							break;
					if(resource_ptr)
						return MHD_queue_response(connection, MHD_HTTP_OK, resource_ptr->response);
					else
						return OFPSVR_Q_404;
				}
				break;
			case 4: // "/blog/.../resources/..."
				ptr = strrchr(u,'/');
				ptr_another = strchr(u+1,'/');
				if(!ptr || !ptr_another) return OFPSVR_Q_404;
				++ptr;
				++ptr_another;
				char *ptr3 = strchr(ptr_another,'/');
				if(!ptr3) return OFPSVR_Q_404;
				*ptr3 = '\0';
				
				int parsed_article_id;
				if(1!=sscanf(ptr_another, "%d", &parsed_article_id))
					return OFPSVR_Q_404;
				if(!(parsed_article_id>=0 && parsed_article_id<articles_len))
					return OFPSVR_Q_404;
	
				struct Resource *resource_ptr;
				for (resource_ptr = articles[parsed_article_id]->resources; resource_ptr; resource_ptr = resource_ptr->next)
					if(0==strcmp(resource_ptr->filename,ptr))
						break;
				if(resource_ptr)
					return MHD_queue_response(connection, MHD_HTTP_OK, resource_ptr->response);
				else
					return OFPSVR_Q_404;
			default:
				return OFPSVR_Q_404;
		}
		return OFPSVR_Q_404;
	} else { //POST
		if (NULL == *con_cls)
	    {
			char *ptr = strrchr(url,'/');
			if(!ptr) return OFPSVR_Q_404;
			++ptr;

			int parsed_article_id;
			if(1!=sscanf(ptr, "%d", &parsed_article_id))
				return OFPSVR_Q_404;
			
			
			//for safety
			const union MHD_ConnectionInfo * info;
			if((info = MHD_get_connection_info(connection,MHD_CONNECTION_INFO_CLIENT_ADDRESS))){
				if( articles[parsed_article_id]->last_posted_client_cnt && articles[parsed_article_id]->last_posted_client == ((struct sockaddr_in*)(info->client_addr))->sin_addr.s_addr ){
					++articles[parsed_article_id]->last_posted_client_cnt;
					if(articles[parsed_article_id]->last_posted_client_cnt >= 4){
						return send_page(connection,"In order to discourage POST-robots, we don't allow 4 or more than 4 consecutive comments be posted by the same IP. Sorry.");
					}
				}else{
					articles[parsed_article_id]->last_posted_client = ((struct sockaddr_in*)(info->client_addr))->sin_addr.s_addr;
					articles[parsed_article_id]->last_posted_client_cnt = 1;
				}
			}else{
				WRITELOG("MHD_get_connection_info failed!\n");
			}
			
			
			
			struct connection_info_struct *con_info;
			if(!(con_info = calloc (1,sizeof (struct connection_info_struct)))){
				return OFPSVR_Q_500;
			}
			if(!(con_info->postprocessor = MHD_create_post_processor (connection, POSTBUFFERSIZE, iterate_new_comment, (void *) con_info))){
				free (con_info);
				return OFPSVR_Q_500;
			}
			con_info->parsed_article_id = parsed_article_id;
			if(!(con_info->comment = calloc(1,sizeof(struct Comment)))){
				free (con_info);
				return OFPSVR_Q_500;
			}
			con_info->posted_at = time(0);
			*con_cls = (void *) con_info;
			return MHD_YES;
	    }
		struct connection_info_struct *con_info = *con_cls;
		if (*upload_data_size){
			MHD_post_process (con_info->postprocessor, upload_data, *upload_data_size);
			*upload_data_size = 0;
			return MHD_YES;
        }else if (con_info->finished){
			char *infopage;
			asprintf(&infopage,"<html><head><meta http-equiv=\"refresh\" content=\"0;URL=/blog/%d\" /><meta http-equiv=\"Content-type\" content=\"text/html; charset=UTF-8\" /></head><body>评论已发表 谢谢!</body></html>",con_info->parsed_article_id,con_info->parsed_article_id);
			int ret = send_page(connection,infopage);
			free(infopage);
			return ret;
		}else{
			switch(con_info->error_type){
				case BAD_FORMAT:return send_page(connection,"BAD FORMAT");
				case NAME_TOO_LONG:return send_page(connection,"Name too long! (Please keep its length less than 100)");
				case EMAIL_TOO_LONG:return send_page(connection,"Email too long! (Please keep its length less than 100)");
				case WEBSITE_TOO_LONG:return send_page(connection,"Website too long! (Please keep its length less than 100)");
				case BODY_TOO_LONG:return send_page(connection,"Body too long! (Please keep its length less than 1000)");
				case BLANK:return send_page(connection,"Name,Email,Body are all required fields, please don't leave any of them blank.");
			}
			return OFPSVR_Q_500;
		}
	}
	return OFPSVR_Q_500;
}

void request_completed (void *cls, struct MHD_Connection *connection,
						void **con_cls, enum MHD_RequestTerminationCode toe){
	if(NULL == *con_cls) return;
	struct connection_info_struct *con_info = (struct connection_info_struct *) *con_cls;
	if (con_info->postprocessor)
		MHD_destroy_post_processor (con_info->postprocessor);
	if (con_info->name)
		free(con_info->name);
	if (con_info->email)
		free(con_info->email);
	if (con_info->website)
		free(con_info->website);
	if (con_info->body)
		free(con_info->body);
	free(con_info);
	*con_cls = NULL;
}
