#!/bin/bash
source ../git_sensitive.sh
mysqldump -u root -p$GIT_SENSITIVE_MYSQL_PASSWORD ofpsvr > ../ofpsvr.backup.sql
