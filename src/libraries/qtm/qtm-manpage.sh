#!/bin/sh
cat <<EOF
.TH qtm 1 "Jun 7, 2008" "version $1" "USER COMMANDS"
.SH NAME
qtm \- Desktop client for content management systems
.SH SYNOPSIS
.B qtm
.SH DESCRIPTION
QTM is a desktop client for content management systems such as Wordpress,
Movable Type, Drupal and Textpattern and for online content platforms like
wordpress.com and Squarespace (but not Blogger).  It uses XML-RPC to
connect to and post entries to websites which use the standard APIs.
.SH OPTIONS
QTM does not offer command-line options of its own, but can open entry
files from the command-line.  As a Qt application it supports Qt options; 
please see:

http://doc.trolltech.com/4.2/qapplication.html#QApplication
.SH AUTHOR
Matthew Smith (indigojo (at) blogistan.co.uk)
EOF
