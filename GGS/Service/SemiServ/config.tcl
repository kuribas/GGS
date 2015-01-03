#config.tcl
#this is the only file the administrator is allowed to modify

set default_socket {"ftp.nj.nec.com" "5000"}
#standard socket

set user_socket {"ftp.nj.nec.com" "5000"}
#socket set by the user

set default_user {"xmav" "blabla"}
#default user and default password

set this_user {"ThisUser" "ThisPassword"}
#user data of this user and this password

set user_server "dsdu"
#for initialisation:
#dsdu:default socket, default user
#dstu:default socket, this user
#usdu:user socket, default user
#ustu:user socket, this user
