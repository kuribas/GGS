#! /bin/tcsh -f

chmod -R o-rwx .
chmod g=rwx *
chmod g=rw  txt/*
chmod g=rwx bin/*
chmod g=rwx */*/.src 
chmod g=rw  */*/.src/* 
chmod g=rwx */*/msg
chmod g=rw  */*/msg/*
chmod g=rwx */*/bak
chmod g=rw  */*/bak/*
chmod g=rwx */*/bak/.week
chmod g=rx  */*/db
chmod g=r   */*/db/*
chmod g=rx  */*/log
chmod g=r   */*/log/*
