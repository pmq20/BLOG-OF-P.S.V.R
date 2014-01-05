# OF P.S.V.R

This is the source code of site [OF P.S.V.R](http://blog.ofpsvr.org)

* It is a blog engine written in C, as an experiment for in-mem caching of web applications.
* For more information about the code, Please review the article included inside the ofpsvr.sql file.
* You can use and modify this software freely under GNU General Public License.


## INSTALL

First you need to adjust some constants in db\_config.h (see `db_config.h.sample`), after which,

	$ make
	$ sudo make install
	$ mysql -u root -p
	mysql> create database ofpsvr;
	mysql> use ofpsvr;
	mysql> source ofpsvr.backup.sql;
	mysql> exit;
	$ sudo ofpsvr

to see what is happening in the background,
	
	$ tail -f /var/log/ofpsvr.log



## TIPS FOR UBUNTU SYSTEMS
You probably need to

	$ sudo apt-get install libmysqlclient-dev
	$ sudo apt-get install libmicrohttpd-dev
	$ sudo apt-get install libssl-dev

If you have any problems, feel free to send me mails to ask
pmq2001 (at) gmail.com
