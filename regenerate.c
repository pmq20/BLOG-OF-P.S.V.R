/*
 *      regenerate.c
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
#include <openssl/md5.h>
#define OFPSVR_HEADER1 "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"\
					   "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"\
					   "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" >"\
					   "<head>"\
					   "<meta http-equiv=\"Content-type\" content=\"text/html; charset=UTF-8\" />"\
					   "<link href=\"/resources/master.css\" media=\"screen\" rel=\"stylesheet\" type=\"text/css\" />"\
					   "<script src=\"/resources/master.js\" type=\"text/javascript\"></script>"\
					   "<script type=\"text/x-mathjax-config\">"\
           	  "MathJax.Hub.Config({"\
           	  "  tex2jax: {"\
           	  "    inlineMath: [ ['$','$'] ],"\
           	  "    processEscapes: true"\
           	  "  }"\
           	  "});"\
            	"</script>	"\
            	"<style type=\"text/css\" media=\"screen\">"\
              " .post_body p{"\
              "   text-indent:20px;"\
              "   margin-bottom:10px;"\
              " }"\
              "</style>"\
					   "<link href=\"/blog.xml\" type=\"application/rss+xml\" rel=\"alternate\" title=\"Blog Of P.S.V.R RSS Feed\" />"\
             "<script type=\"text/javascript\" src=\"http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML\"></script>"
#define OFPSVR_HEADER2 "</head>"\
					   "<body>"\
             "<a href=\"https://github.com/pmq20/BLOG-OF-P.S.V.R\"><img style=\"position: absolute; top: 0; right: 0; border: 0;\" src=\"https://s3.amazonaws.com/github/ribbons/forkme_right_white_ffffff.png\" alt=\"Fork me on GitHub\"></a>"\
					   "<div id=\"top\"><div id=\"logo\"><a href=\"/\"><img alt=\"Logo\" src=\"/resources/logo.png\" /></a><a href=\"/\"><img alt=\"Logo\" src=\"/resources/logo2.png\" /></a></div></div>"\
					   "<table id=\"content\"><tr>"\
					   "<!-- ======Content====== -->"\
					   "<td id=\"main\">"
#define OFPSVR_FOOTER  "</td>"\
					   "<td id=\"sidebar\"><ul>"\
					   "<li><h2>About</h2><div id=\"about_me\" class=\"body\"><img alt=\"P.S.V.R\" id=\"psvr\" class=\"illustration\" src=\"http://www.gravatar.com/avatar/8002c84eb4c18170632f8fb7efb09288\" />Hello, I'm <strong>P.S.V.R</strong>, Living in happiness with <a href=\"http://insoeg.com/\"><strong>Insoeg</strong></a>.<br>[ <a href=\"http://ofpsvr.org/\">Home</a> ]<br style=\"clear:both\" /><strong>A.K.A.'s</strong>:Pan 平底锅 试管牛<br /><strong>Derivatives</strong>:平底牛 试管锅 事故按钮 ...</div></li>"\
					   "<li><h2>Subscription</h2><div class=\"body\"><a href=\"/blog.xml\"><img id=\"rss\" alt=\"rss\" width=\"200\" height=\"173\" src=\"/resources/rss.png\" /></a></div></li>"\
					   "<li><h2>Friends</h2><div class=\"body\"><ul><li><a href=\"http://www.zrapton.com/\">ZaviOr</a></li><li><a href=\"http://www.marisai.com/\">サイコ</a></li><li><a href=\"http://longyi.li/\">Longyi Lee</a></li><li><a href=\"http://www.sishuixie.com/\">Qiankjz</a></li><li><a href=\"http://www.wutj.info/\">Tianji Wu</a></li><li><a href=\"http://pine.im/\">Pine</a></li></ul></div></li>"\
					   "<li><h2>Contact</h2><div class=\"body\"><ul><li><img alt=\"address\" src=\"/resources/email.png\" /></li></ul></div></li>"\
					   "</ul></td>"\
					   "<!-- ======Content====== -->"\
					   "</tr></table>"\
					   "<div id=\"foot\"><p>Powered by <a href=\"https://github.com/pmq20/BLOG-OF-P.S.V.R\">BLOG-OF-P.S.V.R</a> v8901.<br /><a href=\"http://ofpsvr.org\">Home</a> | <a href=\"http://validator.w3.org/check?uri=referer\">Validate XHTML</a> | <a href=\"http://jigsaw.w3.org/css-validator/check/referer\">Validate CSS</a></p>"\
					   "<div id=\"foot2\"></div></div>"\
						 "<script type=\"text/javascript\">"\
						 "  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){"\
						 "  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),"\
						 "  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)"\
						 "  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');"\
						 "  ga('create', 'UA-45613300-2', 'ofpsvr.org');"\
						 "  ga('send', 'pageview');"\
						 "</script>"\
					   "</body>"\
					   "</html>"
#define OFPSVR_RSS_HEADER "<?xml version=\"1.0\"?>"\
						  "<rss version=\"2.0\">"\
						  "<channel>"\
						  "<title>Blog Of P.S.V.R</title>"\
						  "<link>http://ofpsvr.com/blog</link>"\
						  "<description>Blog Of P.S.V.R</description>"\
						  "<language>zh-cn</language>"\
						  "<docs>http://blogs.law.harvard.edu/tech/rss</docs>"\
						  "<generator>OFPSVR Server</generator>"
#define OFPSVR_RSS_FOOTER "</channel>"\
						  "</rss>"
void add_comment(struct Article *a,struct Comment *c){
	if(a->comments){
		struct Comment *comment_ptr;
		for(comment_ptr = a->comments; comment_ptr->next ; comment_ptr = comment_ptr->next) {}
		comment_ptr->next = c;
	}else{
		a->comments = c;
	}
	++a->comment_count;
}
static char *md5sum(char *src) {
	char *ret = malloc(33*sizeof(char));
	unsigned char *tmp = malloc(17*sizeof(unsigned char));
	if(!ret || !tmp){
		return 0;
	}
	MD5((unsigned char*)src,strlen(src),tmp);
	int i;
	char *now = ret;
	for(i=0;i<16;++i)
	{
		sprintf(now,"%02x",tmp[i]);
		now+=2;
	}
	free(tmp);
	return ret;
}
char *fill_content(char *name,char *email,char *website,char *body, long posted_at, int number) {
	char *email_md5;
	char *ret;
	if(!(email_md5 = md5sum(email))){
		return NULL;
	}
	if(posted_at % 2){
		if(asprintf(&ret,
				 "<table class=\"a_comment\"><tr>"
					 "<td class=\"author author0\">"
						 "<div class=\"pic\"><img width=\"32\" height=\"32\" class=\"illustration\" alt=\"%s\" src=\"http://www.gravatar.com/avatar/%s?s=32&amp;d=http%%3A%%2F%%2Fwww.gravatar.com%%2Favatar%%2Fad516503a11cd5ca435acc9bb6523536%%3Fs%%3D32&amp;r=G\"/></div>"
						 "<div class=\"name\">%s%s%s%s%s</div>"
					 "</td>"
					 "<td class=\"commentbody\">"
						 "<div class=\"info\"><div class=\"info2\">"
							 "<div class=\"date\">Posted at %ld (UNIX-Time) | #%d</div>"
							 "<div class=\"maininfo\"><div class=\"maininfo2\"><p>%s</p></div></div>"
						 "</div></div>"
					 "</td>"
				 "</tr></table>",
				 name,
				 email_md5,
				 (website)?"<a href=\"":"",
				 (website)?website:"",
				 (website)?"\">":"",
				 name,
				 (website)?"</a>":"",
				 posted_at,number,
				 body) < 0){return NULL;}
	}else{
		if(asprintf(&ret,
				 "<table class=\"a_comment\"><tr>"
				 "<td class=\"commentbody\">"
				 "<div class=\"info\"><div class=\"info2\">"
				 "<div class=\"date\">Posted at %ld (UNIX-Time) | #%d</div>"
				 "<div class=\"maininfo\"><div class=\"maininfo2\"><p>%s</p></div></div>"
				 "</div></div>"
				 "</td>"
				 "<td class=\"author author1\">"
				 "<div class=\"pic\"><img width=\"32\" height=\"32\" class=\"illustration\" alt=\"%s\" src=\"http://www.gravatar.com/avatar/%s?s=32&amp;d=http%%3A%%2F%%2Fwww.gravatar.com%%2Favatar%%2Fad516503a11cd5ca435acc9bb6523536%%3Fs%%3D32&amp;r=G\"/></div>"
				 "<div class=\"name\">%s%s%s%s%s</div>"
				 "</td>"
				 "</tr></table>",
				 posted_at,number,
				 body,
				 name,
				 email_md5,
				 (website)?"<a href=\"":"",
				 (website)?website:"",
				 (website)?"\">":"",
				 name,
				 (website)?"</a>":"") < 0){return NULL;}
	}
	assert(email_md5);
	free(email_md5);
	cache_size += (strlen(ret)+1)*sizeof(char);
	if(!cache_size_silent) WRITELOG("Cache Size: %lf MB (increased by fill_content)%s",(double)cache_size/1000000,(running?"\n":"..."));
	return ret;
}

int regenerate(struct Article *x,int id) {
	if(x->response){
		MHD_destroy_response(x->response);
		cache_size -= x->page_sz;
		if(!cache_size_silent) WRITELOG("Cache Size: %lf MB (decreased by regenerate)%s",(double)cache_size/1000000,(running?"\n":"..."));
	}
	char *page,*old;
	if(asprintf(&page,
				OFPSVR_HEADER1
				"<title>%s - Blog Of P.S.V.R</title>"
				OFPSVR_HEADER2
				"<table class=\"post\"><tr>"
					"<td class=\"post_bubble\"></td>"
					"<td class=\"post_header\">"
						"<h2 class=\"title\">%s</h2>"
						"<p class=\"byline\">Posted at %ld (UNIX-Time)</p>"
					"</td>"
				"</tr><tr>"
					"<td colspan=\"2\" class=\"post_body\">"
					"%s"
					"</td>"
				"</tr></table>"
				"<div class=\"intro\" style=\"margin-top:50px\">"
				"<p><span class=\"bracket\">{</span> <span>%d</span> files <span class=\"bracket\">}</span></p>"
				"</div>"
				"<div id=\"resources_list_wrapper\">"
				"<table id=\"resources_list\">"
				"<thead>"
				"<tr>"
				"<td width=\"25%%\">#</td>"
				"<td width=\"50%%\">File Name</td>"
				"<td width=\"35%%\">Size</td>"
				"</tr>"
				"</thead>"
				"<tbody>",
			 x->title,
			 x->title,
			 x->posted_at,
			 x->body,
		 	 x->resource_count) < 0){return 0;}
	struct Resource *resource_ptr;
	int cnt = 0;
	for (resource_ptr = x->resources; resource_ptr; resource_ptr = resource_ptr->next) {
		++cnt;
		old = page;
		char *ptr = strrchr(resource_ptr->filename,'.');
		if(asprintf(&page, "%s<tr><td>%d#</td><td><a class=\"%s\" href=\"/blog/%d/resources/%s\">%s</a></td><td>%d KB</td></tr>", old, cnt, ptr?(ptr+1):"no_ext", id, resource_ptr->filename, resource_ptr->filename, resource_ptr->sz/1000) < 0){return 0;}
		assert(old);free(old);
	}
	old = page;
	if(asprintf(&page,
				"%s"
				"</tbody>"
				"</table>"
				"</div>"
				"<div class=\"intro\" style=\"margin-top:50px\">"
				"<p><span class=\"bracket\">{</span> <span>%d</span> notes... Continue reading or <a rel=\"nofollow\" href=\"#comments_add\">Add a note</a> <span class=\"bracket\">}</span></p>"
				"</div>",
				old,
				x->comment_count) < 0){return 0;}
	assert(old);free(old);
	struct Comment *comment_ptr;
	for (comment_ptr = x->comments; comment_ptr; comment_ptr = comment_ptr->next) {
		old = page;
		if(asprintf(&page, "%s%s", old, comment_ptr->content) < 0){return 0;}
		assert(old);free(old);
	}
	old = page;
	if(asprintf(&page,"%s"
				 "<div class=\"intro\">"
					"<p>Add a note</p>"
				 "</div>"
				 "<form id=\"comments_add\" method=\"post\" onsubmit=\"return test_submit()\">"
					 "<p><input type=\"text\" tabindex=\"1\" size=\"22\" value=\"\" id=\"author\" name=\"reply[name]\"/>"
					 "<label for=\"author\">*Name</label>"
					 "<p><input type=\"text\" tabindex=\"2\" size=\"22\" value=\"\" id=\"email\" name=\"reply[email]\"/>"
					 "<label for=\"email\">*E-mail (will not be disclosed)</label></p>"
					 "<p><input type=\"text\" tabindex=\"3\" size=\"22\" value=\"\" id=\"url\" name=\"reply[website]\"/>"
					 "<label for=\"url\">Website (optional)</label></p>"
					 "<p><textarea tabindex=\"4\" rows=\"5\" cols=\"100\" id=\"comment\" name=\"reply[body]\"></textarea></p>"
					 "<p><input type=\"submit\" value=\"Submit\" tabindex=\"5\" id=\"submit\" name=\"commit\"/></p>"
				 "</form>"
				OFPSVR_FOOTER,
			    old) < 0){return 0;}
	assert(old);free(old);
	if(!(x->response = MHD_create_response_from_data(strlen(page),(void*)page, MHD_YES, MHD_NO))){
		return 0;
	}
	if(MHD_NO == (MHD_add_response_header (x->response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html"))){
		return 0;
	}
	x->page_sz = (strlen(page)+1)*sizeof(char);
	cache_size += x->page_sz;
	if(!cache_size_silent) WRITELOG("Cache Size: %lf MB (increased by regenerate)%s",(double)cache_size/1000000,(running?"\n":"..."));
	return 1;
}

static char *count2bgcolor(int count){
	if(0 == count) return "#4ABBFF";
	else if(count > 0 && count <= 10) return "#388CBD";
	else if(count > 10 && count <= 50) return "#0F68A0";
	else if(count > 50 && count <= 100) return "#094469";
	else if(count > 100) return "black";
	else return "red";
}
static char *count2str(int count){
	char *ret;
	if(count < 1000){
		if(asprintf(&ret, "%d", count) < 0)
			return NULL;
	}else if(count < 1000000){
		if(asprintf(&ret, "%dK", count/1000) < 0)
			return NULL;
	}else if(count < 1000000000){
		if(asprintf(&ret, "%dM", count/1000000) < 0)
			return NULL;
	}else{
		if(asprintf(&ret, "%d", count) < 0)
			return NULL;
	}
	assert(ret);
	return ret;
}
struct MHD_Response *generate_blog_response() {
	char *page,*old;
	if(asprintf(&page,
			 OFPSVR_HEADER1
			 "<title>Blog Of P.S.V.R</title>"
				OFPSVR_HEADER2) < 0 ){return NULL;}
	int i;
	for(i=articles_len-1;i>=0;--i)
	{
		old = page;
		char *hit_count_str,*comment_count_str,*resource_count_str;
		if(!(hit_count_str = count2str(articles[i]->hit_count))) return NULL;
		if(!(comment_count_str = count2str(articles[i]->comment_count))) return NULL;
		if(!(resource_count_str = count2str(articles[i]->resource_count))) return NULL;
		if(asprintf(&page, "%s"
				 "<table class=\"post\"><tr>"
				 "<td class=\"post_boxes\">"
				 "<div class=\"box b_hits\"><div class=\"number\">%s</div><div>hits</div></div>"
				 "<div class=\"box b_replies\" style=\"background-color:%s\"><div class=\"number\">%s</div><div>notes</div></div>"
				 "<div class=\"box b_resources\"><div class=\"number\">%s</div><div>files</div></div>"
				 "</td>"
				 "<td class=\"post_header\">"
				 "<h2 class=\"title\"><a href=\"/blog/%d\">%s</a></h2>"
				 "<p class=\"byline\">Posted at %ld (UNIX-Time) | #%d</p>"
				 "</td>"
				 "</tr><tr>"
				 "<td colspan=\"2\" class=\"post_body\">"
				 "<p>%s</p>"
				 "</td>"
				 "</tr></table>",
				 old,
				 hit_count_str,
				 count2bgcolor(articles[i]->comment_count),comment_count_str,
				 resource_count_str,
				 i,articles[i]->title,
				 articles[i]->posted_at,i,
				 articles[i]->introduction)<0){return NULL;}
		assert(old);free(old);
		assert(hit_count_str);free(hit_count_str);
		assert(comment_count_str);free(comment_count_str);
		assert(resource_count_str);free(resource_count_str);
	}
	old = page;
	if(asprintf(&page,"%s"
			 OFPSVR_FOOTER,
				old) < 0){return NULL;}
	assert(old);
	free(old);
	struct MHD_Response *ret;
	if(!(ret = MHD_create_response_from_data(strlen(page),(void*)page, MHD_YES, MHD_NO))){
		return NULL;
	}
	if(MHD_NO == MHD_add_response_header (ret, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html")){
		return NULL;
	}
	assert(ret);
	return ret;
}

struct MHD_Response *generate_blog_response_rss() {
	char *page,*old;
	if(asprintf(&page,
				OFPSVR_RSS_HEADER
				"<pubDate>%s</pubDate>"
				"<lastBuildDate>%s</lastBuildDate>",
				articles[articles_len-1]->rfc_posted_at,
				articles[articles_len-1]->rfc_posted_at) < 0 ){return NULL;}
	int i;
	for(i=articles_len-1;i>=0;--i)
	{
		old = page;
		if(asprintf(&page, "%s"
					"<item>"
					"<title>%s</title>"
					"<link>http://blog.ofpsvr.org/blog/%d</link>"
					"<description>%s</description>"
					"<pubDate>%s</pubDate>"
					"</item>",
					old,
					articles[i]->title,
					i,
					articles[i]->body,
					articles[i]->rfc_posted_at)<0){return NULL;}
		assert(old);free(old);
	}
	old = page;
	if(asprintf(&page,"%s"
				OFPSVR_RSS_FOOTER,
				old) < 0){return NULL;}
	assert(old);free(old);
	struct MHD_Response *ret;
	if(!(ret = MHD_create_response_from_data(strlen(page),(void*)page, MHD_YES, MHD_NO))){
		return NULL;
	}
	if(MHD_NO == MHD_add_response_header (ret, MHD_HTTP_HEADER_CONTENT_TYPE, "application/rss+xml")){
		return NULL;
	}
	assert(ret);
	return ret;
}
