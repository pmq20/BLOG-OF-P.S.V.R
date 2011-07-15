# OF P.S.V.R

This is the source code of site [OF P.S.V.R](http://blog.ofpsvr.org)
	* It is a blog engine written in C, as an experiment on the C-handled cache abilities for web application.
	* For more information about the code, Please review the content provided inside the ofpsvr.sql file.
	* You can use and modify this software freely under GNU General Public License.


## INSTALL

	$ make
	$ sudo make install
	$ mysql -u root -p
	mysql> create database ofpsvr;
	mysql> use ofpsvr;
	mysql> source ofpsvr.backup.sql;
	mysql> exit;



## TIPS FOR UBUNTU SYSTEMS
You probably need to
	$ sudo apt-get install libmysqlclient-dev
	$ sudo apt-get install libmicrohttpd-dev

If you have any problems, feel free to send me mails to ask
pmq2001 (at) gmail.com
