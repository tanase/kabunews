# -*- coding: utf-8 -*-
import sys, urllib2, os, sqlite3, glob, re

print """
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"> 
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en"> 
<head> 
<meta http-equiv="content-type" content="text/html;charset=UTF-8" /> 
<title>無料株式新聞 バックナンバー</title>
</head>
<body>
<h1>無料株式新聞バックナンバー</h1>
"""

files = glob.glob('[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9].html')
files.sort()
files.reverse()
for file in files:
    year  = file[:4]
    month = file[4:6]
    day   = file[6:8]
    print "<a href=\"%s%s%s.html\">%s年%s月%s日</a><br />" % (year,month,day,year,month,day)

print """
</body>
</html>
"""    
