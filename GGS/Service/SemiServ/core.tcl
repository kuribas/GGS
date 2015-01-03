#included
source [pwd]/config.tcl
package require BWidget
# client procedures:
#global variables
set socket_id ""
set timer_values {{00:15:00 00:15:00}}
#default values for all timers
set connection_state false
set tables {}
set play_state neutral
set lost_reasons left
set full_tables {}
set ordered_tables {}
set who_list {}
set polling_clock 1
set timer_clock 1000
#timer intervall= 1s
set game_ids {}
set reqids {}
for {set i 1} {$i<24} {incr i} {
 set game_ids "$game_ids {}"
 set reqids "$reqids {}"
 set timer_values "$timer_values {00:15:00 00:15:00}"
 set lost_reasons "$lost_reasons left"
 set play_state "$play_state neutral"
}
set count_values $timer_values
set eingabe ""

proc refresh_room {} {
 #tells to the semiserv to give the participants and the configuration of the tables
 global socket_id
 puts $socket_id "t semiserv get_participants"
 flush $socket_id
 puts $socket_id "t semiserv show_config_tables"
 flush $socket_id
}

proc error_message {err_type message_out} {
#displays errors
#err_type can be:
#"socket":problems with socket connect
#"fileIO": problems to save the config.tcl
#"move":false move was made
 tk_messageBox -title Error -icon error -message "$err_type: $message_out"
}

proc display_chat {chat_message} {
#displays chat
 puts $chat_message
}

proc countdown {mytime} {
#needs a time and executes a countdown until 0
 set listtime [split $mytime ":"]
 if {[lindex $listtime 2]==""} {
  set hour 0
  set buf [string trimleft [lindex $listtime 0] "0"]
  if {$buf==""} {
   set minute 0
  } else {
   set minute [expr $buf]
  }
  set buf [string trimleft [lindex $listtime 1] "0"]
  if {$buf==""} {
   set second 0
  } else {
   set second [expr $buf]
  }
 } else {
  set buf [string trimleft [lindex $listtime 0] "0"]
  if {$buf==""} {
  set hour 0
   } else {
   set hour [expr $buf]
  }
  set buf [string trimleft [lindex $listtime 1] "0"]
  if {$buf==""} {
   set minute 0
  }  else {
  set minute [expr $buf]
  }
  set buf [string trimleft [lindex $listtime 2] "0"]
  if {$buf==""} {
   set second 0
   } else {                       set second [expr $buf]
  }
 }
 if {$hour==0 && $minute==0 && $second==0} {return $mytime}
 if {[expr $second]>0} {set second [expr $second -1]
 } elseif {[expr $minute>0]} {
         set second 59
         set minute [expr $minute -1]
 } else {
  set second 59
  set minute 59
  set hour [expr $hour -1]
 }
 if {$second<10} {set second "0$second"}
 if {$minute<10} {set minute "0$minute"}
 if {$hour<10} {set hour "0$hour"}
 return "$hour:$minute:$second"
}

proc theclock {} {
 #this is the timer clock which decreases the time of the player, if they are playing and if
 #they have to move
 global connection_state play_state count_values  timer_clock
 for {set i 0} {$i<24} {incr i} {
     set timepack [lindex $count_values $i]
     if {[lindex $play_state $i]=="black"} {
      set buftime [lindex $timepack 0]
      puts $buftime
      set buftime [countdown $buftime]
      set timepack [lreplace $timepack 0 0 $buftime]
      display_timer t[expr $i+1] $buftime [lindex $timepack 1]
      set count_values [lreplace $count_values $i $i $timepack]
      } elseif {[lindex $play_state $i]=="white"} {
      set buftime [lindex $timepack 1]
      puts $buftime
      set buftime [countdown $buftime]
      set timepack [lreplace $timepack 1 1 $buftime]
      display_timer t[expr $i+1] [lindex $timepack 0] $buftime
      set count_values [lreplace $count_values $i $i $timepack]
     }
     puts "timepack: $timepack"
    }
    #recursive, after 1s, it calls itself
    if {$connection_state=="true"} {
     after $timer_clock {
     puts "ticktack"
     theclock}
   }
}

proc save_config {} {
#writes the data to the config.tcl file
     global default_socket user_socket default_user this_user user_server
     if [catch {
     set fid [open "config.tcl" w+]
     puts $fid "#config.tcl"
     puts $fid "#this is the only file the administrator is allowed to modify"
     puts $fid ""
     puts $fid "set default_socket \{\"[lindex $default_socket 0]\" \"[lindex $default_socket 1]\"\}"
     puts $fid "#standard socket"
     puts $fid ""
     puts $fid "set user_socket \{\"[lindex $user_socket 0]\" \"[lindex $user_socket 1]\"\}"
     puts $fid "#socket set by the user"
     puts $fid ""
     puts $fid "set default_user \{\"[lindex $default_user 0]\" \"[lindex $default_user 1]\"\}"
     puts $fid "#default user and default password"
     puts $fid ""
     puts $fid "set this_user \{\"[lindex $this_user 0]\" \"[lindex $this_user 1]\"\}"
     puts $fid "#user data of this user and this password"
     puts $fid ""
     puts $fid "set user_server \"$user_server\""
     puts $fid "#for initialisation:"
     puts $fid "#dsdu:default socket, default user"
     puts $fid "#dstu:default socket, this user"
     puts $fid "#usdu:user socket, default user"
     puts $fid "#ustu:user socket, this user"
     close $fid
     }   err] {
     error_message "fileIO" $err
      }
   }

proc get_table_id {player1 player2} {
 #searches the player of a table and gives out the index of the table
 global full_tables
 set i 0
 set j 0
 while {[lindex [lindex $full_tables $i] 1]!=$player1 || [lindex [lindex $full_tables $i] 2]!=$player2} {
       if {$i>23} return -1
        incr i
       }
 return $i
}

proc polling {} {
  #most important procedure
  #it cares about the administrative work to read from the socket und call the right
  #graphic procedures
  global socket_id connection_state eingabe tables who_list polling_clock full_tables game_ids timer_values lost_reasons reqids count_values play_state
   while {$eingabe!="" && $connection_state=="true"} {
   puts "$eingabe wird ausgewertet."
   #programm fertig ist
      if {[lindex $eingabe 0]==":"} {
       puts "Doppelpunkt"
       switch $eingabe {
       ": ERR user 'semiserv' not found." {
                           error_message "Semiserver" "This service was not started."

                              }
       }
      while {$eingabe!="READY"} {
            puts "von echo: $eingabe"
            set eingabe [gets $socket_id]
            }
      puts "von echo: $eingabe"
      #ist das ein echo
      } elseif {[lindex $eingabe 0]=="/os:"} {
       #what is ggs telling me?
       puts "/os:"
       puts $eingabe
       if {[lindex $eingabe 1]=="update" || [lindex $eingabe 1]=="join"} {
          #is it a board?
          set buffer [lindex $eingabe 1]
          set tmp_game_id [lindex $eingabe 2]
          while {[set eingabe [gets $socket_id]]==""} {}
          if {$buffer=="join"} {
            while {[set eingabe [gets $socket_id]]==""} {}
          }
          set player1 ""
          while {[set eingabe [gets $socket_id]]==""} {}
          set player1 [lindex [string trim $eingabe |] 0]
          set timeblack [lindex $eingabe 3]
          set timeblack [lindex [split $timeblack ","] 0]
          puts $timeblack
          if {[llength [split $timeblack ":"]]<3} {
             set timeblack "00:$timeblack"
          }
          while {[set eingabe [gets $socket_id]]==""} {}
          set player2 [lindex [string trim $eingabe |] 0]
          set timewhite [lindex $eingabe 3]
          set timewhite [lindex [split $timewhite ","] 0]
          puts $timewhite
          if {[llength [split $timewhite ":"]]<3} {
             set timewhite "00:$timewhite"
          }
          set tmp_tisch_id [get_table_id $player1 $player2]
          display_timer t[expr $tmp_tisch_id +1] $timeblack $timewhite
          set count_values [lreplace $count_values $tmp_tisch_id $tmp_tisch_id "$timeblack $timewhite"]
          #display_timer
          puts "player1: $player1"
          puts "player2: $player2"

          if {$buffer=="join"} {

             set game_ids [lreplace $game_ids $tmp_tisch_id $tmp_tisch_id $tmp_game_id]
          }
          puts "Game Ids: $game_ids"
          #spieler drin, brett einlesen
          while {[set eingabe [gets $socket_id]]==""} {}
          while {[set eingabe [gets $socket_id]]==""} {}
           set board_puffer ""
           for {set i 0} {$i<8} {incr i} {
           while {[set eingabe [gets $socket_id]]==""} {}
           set board_puffer $board_puffer[join [string trim [string trim $eingabe "| "] [expr $i +1]] ""]
           }
           while {[set eingabe [gets $socket_id]]==""} {}
           puts "wichtig: $eingabe"
           while {[set eingabe [gets $socket_id]]==""} {}
           puts "wichtig: $eingabe"
           while {[set eingabe [gets $socket_id]]==""} {}
           puts "wichtig: $eingabe"
           if {[lindex [string trim $eingabe "|"] 0]=="*"} {
            display_turn t[expr $tmp_tisch_id +1] "black"
            set play_state [lreplace $play_state $tmp_tisch_id $tmp_tisch_id "black"]
           } else {
            display_turn t[expr $tmp_tisch_id +1] "white"
            set play_state [lreplace $play_state $tmp_tisch_id $tmp_tisch_id "white"]
           }
           puts "Board: $board_puffer"
           display_board t[expr $tmp_tisch_id+1] $board_puffer
           if {$player1==[request_default_login] || $player2==[request_default_login]} {
              set kibitzer [lindex [lindex $full_tables $tmp_tisch_id] 3]
              for {set i 0} {$i<[llength $kibitzer]} {incr i} {
                 puts $socket_id "t [lindex $kibitzer $i] game_id $tmp_tisch $tmp_game_id"
                 flush $game_ids
               }
            }
         } elseif {[lindex  $eingabe 1]=="-"} {
           #did i lose?
           if {[lindex $eingabe 2]=="match"} {
           set killed_gid [lindex $eingabe 2]
           set index [lsearch $game_ids $killed_gid]
           set play_state [lreplace $play_state $index $index neutral]
           set game_ids [lreplace $game_ids $index $index ""]
           set wert [lindex $full_tables $index]
           if {[lindex $wert 1]==[request_default_login] || [lindex $wert 2]==[request_default_login]} {
             set kibitzer [lindex $wert 3]
               for {set i 0} {$i<[llength $kibitzer]} {incr i} {
                 puts $socket_id "t [lindex $kibitzer $i] game_id $index {}"
                 flush $game_ids
              }
         }
          puts [set winstate [lindex $eingabe [expr [llength $eingabe] -1]]]
          set blackscore 32
          set whitescore 32
          set kind draw
          if {$winstate!="left"} {
          if {[expr $winstate]<0} {
           set kind lost
           set blackscore ["expr 32+$winstate/2"]
           set whitescore ["expr 32-$winstate/2"]
         } elseif {$winstate>0} {
           set kind win
           set blackscore ["expr 32-$winstate/2"]
           set whitescore ["expr 32+$winstate/2"]
         } else {
           set kind draw
           set blackscore [expr 32]
           set whitescore [expr 32]
         }
         } else {
            set lost_reasons [lreplace $lost_reasons $index $index "draw"]
         }

         display_game_end t[expr $index +1] $kind [lindex $wert 1] [lindex $wert 2] $blackscore $whitescore [lindex $lost_reasons $index]
         set lost_reasons [lreplace $lost_reasons $index $index "left"]
         }
       } else {
       switch {[lindex  $eingabe 1]} {
        #is there somebody who wants to play with me?
        "+" {set request_id [lindex $eingabe 2]
             set player1 [lindex $eingabe 4]
             set player2 [lindex $eingabe 9]
             if {[lsearch $who_list $player1]!=-1 && [lsearch $who_list $player2]!=-1} {
             set index [get_table_id player1 player2]
             set $reqids [lreplace $reqids $index $index $request_id]
             }
            }
       "fatal-timeout" {#timout, game over
                        set game_id [lindex $eingabe 2]
                        set index [lsearch $game_ids $game_id]
                        set lost_reasons [lreplace $lost_reasons $index $index "timeout"]
                       }
       "undo" {  #do again?
                 set game_id [lindex $eingabe 2]
                 set index [lsearch $game_ids $game_id]
                 set answer [display_question_undo t[expr $index +1] [request_default_login]]
                  if {($answer == "yes") | ($answer == "always")} then {
                  display_undo t[expr $index +1]
                  puts $socket_id "ts undo $game_id"
                  flush $socket_id
                  }\
                  else {
                  display_undo_denied t[expr $index +1]
                  }
              }
       }
       }
       while {$eingabe!="READY"} {
            puts "von prot: $eingabe"
            while {[set eingabe [gets $socket_id]]==""} {}
            }
            puts "von prot: $eingabe"
       #oder rückgabe vom server
     } else {
         if {$eingabe=="READY"} {
         puts $eingabe
         } elseif {[lindex $eingabe 0]=="semiserv:"} {
            #what is the semiserver telling to me?
            puts "semiserv"
            puts $eingabe
            switch [lindex $eingabe 1] {
             "show_config_tables" {set rid r[lindex [lindex $eingabe 2] 0]
                                   set tcount [lindex [lindex $eingabe 2] 1]
                                   set tables ""
                                   for {set i 2} {$i<=[expr $tcount + 1]} {incr i} {
                                     set temp_tables t[lindex [lindex $eingabe 2] $i]
                                     set temp_tables2 [lreplace $temp_tables 3 3]
                                     if {$tables==""} then {
                                     set tables "{$temp_tables2}"
                                     set full_tables "{$temp_tables}"
                                      } else {
                                     set tables "$tables {$temp_tables2}"
                                     set full_tables "$full_tables {$temp_tables}"
                                     }

                                     #set tables "{$tables}"
                                     puts "Tische: $tables"
                                     puts "vollst : $full_tables"
                                    puts "rid: $rid"
                                    puts "tcount: $tcount"
                                    display_tables $rid $tcount $tables                                     }
                                    for {set i 0} {$i<24} {incr i} {
                                      set this_entry [lindex $full_tables $i]
                                      set tmp_tid [lindex $this_entry 0]
                                      set blackname [lindex $this_entry 1]
                                      set whitename [lindex $this_entry 2]
                                      set kibitz [lindex $this_entry 3]
                                      set im [request_default_login]
                                      #puts "loginname: $im"
                                      if {$blackname==$im || $whitename==$im || [lsearch $kibitz $im]!=-1} {
                                         display_game_who $tmp_tid $blackname $whitename $kibitz
                                      }
                                    }
                                   }
             "get_participants" {set who_list [lindex $eingabe 2]
                                 display_who r1 $who_list
                                     }
             "stop_game" {refresh_room}
             "set_status_private" {}
             "set_status_public" {}

            }
            while {$eingabe!="READY"} {
            puts "von semiserv: $eingabe"
            set eingabe [gets $socket_id]
            }
            puts "von semiserv: $eingabe"
         } else {

           set firststring [lindex $eingabe 0]
           #puts "firststring: $firststring"
           if {$firststring==".kdp"} {
              set secondstr [lindex $eingabe 2]
              if {$secondstr=="mainchat"} {
                 display_main_message [lindex $eingabe 3] [string trim [lindex $eingabe 1] :] [lindex $eingabe 4]
              }
           } elseif {[lsearch $who_list [set firststring [string trim $firststring :]]]!=-1} {
                #if chat, which kind of and of which person
                set secondstr [lindex $eingabe 1]
                switch $secondstr {
                "gamechat" {display_game_message [lindex $eingabe 2] [string trim [lindex $eingabe 0] :] [lindex $eingabe 3] [lindex $eingabe 4]}
                "mainchatw" {display_main_message_whisper [lindex $eingabe 2] [string trim [lindex $eingabe 0] :] [lindex $eingabe 3] [lindex $eingabe 4]}
                "gamechatw" {display_game_message_whisper [lindex $eingabe 2] [lindex $eingabe 3] [string trim [lindex $eingabe 0] :] [lindex $eingabe 4] [lindex $eingabe 5]}
                "private" {display_private_message [string trim [lindex $eingabe 0] :] [lindex $eingabe 2]}
                "game_id" {for {set i 0} {i< 24} {incr i} {
                             if {[lsearch [lindex [lindex $fulltables $i]3]]!=-1} {
                                set game_ids [lreplace $game_ids [lindex $eingabe 1] [lindex $eingabe 1] [lindex $eingabe 2]]
                                if {[lindex $eingabe 2]!=""} {
                                puts $socket_id "ts watch + [lindex $eingabe 2]"
                                flush $socket_id
                               }
                             }
                          }
                }
               }
           }
           while {$eingabe!="READY"} {
            puts "chat: $eingabe"
            while {[set eingabe [gets $socket_id]]==""} {}
            }
            puts "chat: $eingabe"

         }
         #it can only be chat
     }
    set eingabe [gets $socket_id]
    #listen to the socket
  }

     #if there is nothing at the socket and the connection is still open ask later again
     if {$connection_state=="true"} {
    after $polling_clock {
               if {$connection_state=="true"} {
                  puts "$eingabe wird ausgewertet."
                  #flush $socket_id
                  puts [set eingabe [gets $socket_id]]
                  #recursive
                  polling
                  }
               }
          }
  #if no input, ask again in polling_clock milliseconds, soll dann auf 10ms reduziert werden, sobald da


}


proc request_default_login {} {
#gets the default user name
 global default_user
 return [lindex $default_user 0]
}

proc request_default_password {} {
#gets the default password
 global default_user
 return [lindex $default_user 1]
}

proc request_save_password {newlogin newpass} {
  #save this user name and password to the disc
  global default_user
  set default_user "$newlogin $newpass"
  save_config
  puts "password saved"
}

proc request_server_settings {} {
  #gets the IP and port of the ggs(generic game server)
  global default_socket
  set current [lindex $default_socket 0]
  set default [lindex $default_socket 1]
  #current und default sind listen, bestehend asu {ip-adresse port}
  return "{$current} {$default}"
}

proc request_save_server_settings {newcurrentserver} {
  #save the server settings to the disc
  global default_socket
  set default_socket newcurrentserver
  save_config {}
  puts "server saved"
}


proc request_connection {login pass} {
  #logging in and initialising the string administration
  global socket_id default_user default_socket connection_state eingabe
  set default_user "$login $pass"
  set socket_id [socket [lindex $default_socket 0] [lindex $default_socket 1]]
  #call socket connection
  set connection_state true
  #connected
  fconfigure $socket_id -blocking false
  #no blocking
  puts $socket_id $login
  puts $socket_id $pass
  flush $socket_id
  #send user name und password 
  set eingabe ""
  while {$eingabe!=": Enter login (yours, or one you'd like to use)."} {
   puts [set eingabe [gets $socket_id]]
  }
  while {[set eingabe [gets $socket_id]]==""} {}
  if {$eingabe!=": Enter your password."} {
     #false username?
     error_message Login [join [lreplace [split $eingabe " "] 0 0] " "]
     disconnect
     return
  }
  while {[set eingabe [gets $socket_id]]==""} {}
  if {$eingabe != "READY"} {
  #false password?
      error_message Login [join [lreplace [split $eingabe " "] 0 0] " "]
      disconnect
      return
  }
  #bye!
  set eingabe ""
  puts $socket_id ": chann + .kdp"
  flush $socket_id
  #register to channel .kdp
  puts $socket_id ": mso"
  flush $socket_id
  #want to play reversi
  puts $socket_id "ts open 24"
  flush $socket_id
  #available for 24 games
  puts $socket_id "ts rated -"
  flush $socket_id

  while {$eingabe != "READY"} {
               puts [set eingabe [gets $socket_id]]
              }
  while {[set eingabe [gets $socket_id]]==""} {}
  while {[lindex $eingabe 1] == "help"} {
        #read the whole useless stuff from the server
        while {$eingabe != "READY"} {
               puts [set eingabe [gets $socket_id]]
              }
      while {[set eingabe [gets $socket_id]]==""} {}
  }
  create_room r1
  #initialize the socket read procedure and timer
  if {$connection_state=="true"} {
     polling
     theclock
  }
  #calls the actual configuraion
  refresh_room
}


proc request_game_room {rid tid position} {
  #initialising a game room
  global socket_id full_tables
  if {$position=="black"} {
  puts $socket_id "t semiserv game_black <[string trim $tid t]>"
  flush $socket_id
  } elseif {$position=="white"} {
     puts $socket_id "t semiserv game_white <[string trim $tid t]>"
     flush $socket_id
  } else {
  set tmplist [lindex $full_tables [string trim $tid t]]
  set player1 [lindex $tmplist 1]
  set player2 [lindex $tmplist 2]
  if {$player1=="" && $player2==""} {
  package require math
   if {[::math::random 0 2]==0} {
     puts $socket_id "t semiserv game_black <[string trim $tid t]>"
     flush $socket_id
   } else {
     puts $socket_id "t semiserv game_white <[string trim $tid t]>"
     flush $socket_id
   }
  } elseif {$player1==""} {
      puts $socket_id "t semiserv game_black <[string trim $tid t]>"
      flush $socket_id
  } elseif {$player2==""} {
      puts $socket_id "t semiserv game_white <[string trim $tid t]>"
      flush $socket_id
  } else {
    puts $socket_id "t semiserv view_table <[string trim $tid t]>"
    flush $socket_id
   }
  }
  refresh_room
  puts "Game room requested with position $position"
  # positionen können sein "black" "white" oder "neutral"
  create_game $tid "Game Room [string trim $tid t]"
  global gui client
  set blackname [.$rid.l.t.main itemcget blackplayername$tid -text]
  set whitename [.$rid.l.t.main itemcget whiteplayername$tid -text]
  if {($position == "black") && ($blackname == "")} then {set blackname $gui(login)}
  if {($position == "white") && ($whitename == "")} then {set whitename $gui(login)}
  set client(blackname) $blackname
  set client(whitename) $whitename

  display_game_who $tid $blackname $whitename {}
  display_timer $tid 0:15:00 0:15:00
  if {$position != "neutral"} {display_turn $tid $position}
  display_disccount $tid "2" "2"
}

proc request_move {tid move} {
 #this is called to execute a move
 global socket_id game_ids
 set game_id [lindex $game_ids [expr [string trim $tid t]-1]]
 puts "game id: game_id"
 if {$game_id!=""} {
  puts "move: $move"
  puts $socket_id "tp $game_id $move"
  flush $socket_id
  puts "$tid abgeschickt"
 }
}

# folgende prozeduren sind noch nicht von mir benötigt, werden aber noch eingebaut
proc disconnect {} {
  #it's called to disconnect from the server
  global connection_state  socket_id game_ids
  puts "Disconnecting ..."
  set connection_state false
  for {set i 0} {$i<24} {incr i} {
   if {[lindex $game_ids $i]!=""} {
   set tmp_tid t[expr $i+1]
   if {[request_resign $tmp_tid]=="no"} {return}
     }
   }
   if {[catch {
    puts $socket_id ": quit"
    flush $socket_id
    close $socket_id
   } err] } {
    error_message "Socket" $err
   }
}

proc request_disconnect {command} {
#like disconnect, but add with commands for the gui
global connection_state  socket_id game_ids

  if {[catch {
   if {[eof $socket_id]=="l"} {
      set connection_state false
      $command
      return
   } } err]} {return}
  if {[display_question_disconnect]=="no"} {return}
  set connection_state false
  for {set i 0} {$i<24} {incr i} {
   if {[lindex $game_ids $i]!=""} {
    set tmp_tid t[expr $i+1]
   if {[request_resign $tmp_tid]=="no"} {return}
    }
  }
  $command
  if {[catch {
   puts $socket_id ": quit"
   flush $socket_id
   close $socket_id
  } err]} { }
}

proc request_status {tid status} {
 #tell the state of a table which can be private or public
 switch $status {
       "private" {puts $socket_id "t semiserv set_status_private [string trim $tid t]"}
       "public" {puts $socket_id "t semiserv set_status_public [string trim $tid t]"}
}
 flush $socket_id
}

proc request_game_start {tid} {
 #requests a game_start. that means the user clicks on start and if the other player answers,
 #the game can begin
 global socket_id full_tables game_ids timer_values reqids
 set buftime [lindex $timer_values [expr [string trim $tid t] -1]]
 set blacktime [lindex $buftime 0]
 set whitetime [lindex $buftime 1]
 set this_user [request_default_login]
 set tmplist [lindex $full_tables [expr [string trim $tid t]-1]]
 set player1 [lindex $tmplist 1]
 puts "player1: $player1"
 set player2 [lindex $tmplist 2]
 puts "player2: $player2"
 set reqid [lindex $reqids [expr [string trim $tid t] +1]]
 if {$reqid!=""} {
   puts $socket_id "ts accept $reqid"
   puts "reqid:$reqid"
   set reqids [lreplace $reqids [expr [string trim $tid t] +1] [expr [string trim $tid t] +1] ""]
 } elseif {$player1==[request_default_login]} {
   puts $socket_id "ts ask 8B $blacktime//00:00 $whitetime//00:00 $player2"
 } else {
   puts $socket_id "ts ask 8W $whitetime//00:00 $blacktime//00:00 $player1"
 }
 flush $socket_id
 display_game_start $tid
}

proc request_resign {tid} {
  #give up the game
  global client socket_id game_ids lost_reasons
  set frage [display_question_resign $tid]
  if {[lindex $frage 1]=="yes"} {
   if {[catch {
    puts $socket_id "ts resign .[lindex game_ids [expr [string trim $tid t] -1]]"
    flush $socket_id
   } err]} {
      error_message "Socket" $err
   }
   set tmp_tid [expr [string trim $tid t]-1]
   set game_ids [lreplace $game_ids  $tmp_tid $tmp_tid ""]
   set lost_reasons [lreplace $lost_reasons $tmp_tid $tmp_tid "resigned"]
   #display_game_end $tid win $client(blackname) $client(whitename) 33 31 resigned
   return yes
  }
  return no
}

proc request_undo {tid} {
  #to remove the own mistake
  global socket_id game_ids
  puts $socket_id "ts undo [lindex $game_ids [expr [string trim $tid t] -1]]"
  flush $socket_id
}

proc request_adjourn {tid} {
  #not used, it was planed to save and exit games, to continue late
  global client
  set answer [display_question_adjourn $tid $client(whitename)]
  if {$answer == "yes"} then {
    display_adjourn $tid
  }\
  else {
    display_adjourn_denied $tid
  }
}

proc request_switch_seats {tid} {
 #if the game did not start, you can swap the seats
 global $socket_id
 puts $socket_id "t semiserv switch_places <[string trim $tid t]>"
 flush $socket_id
}

proc request_leave_table {tid} {
   #go away, that can include to give up
   global socket_id full_tables
   set tmplist [lindex $full_tables [string trim $tid t]]
   set player1 [lindex $tmplist 1]
   set player2 [lindex $tmplist 2]
   set index [expr [string trim $tid t] -1]
   if {[lindex game_ids $index]!=""} {set yesno [request_resign $tid]
   if {$yesno=="no"} {return}
   if {[lindex request_ids $index]!=""} {
     catch {
      puts $socket_id "ts decline [lindex request_ids $index]"
      flush $socket_id
     } err
     }
   }
   catch {
   puts $socket_id "t semiserv stop_game <[string trim $tid t]>"
   flush $socket_id
   puts $socket_id "t semiserv stop_view <[string trim $tid t]>"
   flush $socket_id
   refresh_room
  }
}

proc request_main_message {rid message} {
# send the $message to the mainroom 
 global socket_id
 display_main_message $rid [request_default_login] $message
 puts $socket_id "t .kdp mainchat $rid {$message}"
 flush $socket_id
}

proc request_game_message {rid tid message} {
 #sends a message to the game room
 global socket_id tables full_tables
 display_game_message $rid $tid [request_default_login] $message
 set tmplist [lindex $full_tables [string trim $tid t]]
 set player1 [lindex $tmplist 1]
 set player2 [lindex $tmplist 2]
 if {$player1=="" || $player2==""} {return}
 set kibitz [lindex $tmplist 3]
 set allnames [join {$player1 $player2 [join $kibitz ,]} ,]
 puts $socket_id "t $allnames gamechat $rid $tid {$message}"
 flush $socket_id
}

proc request_main_message_whisper {rid nicknames message} {
  # sends message only to choosed people in the mainroom
  global socket_id
  puts $socket_id "t [join $nicknames ,] mainchatw $rid {$nicknames} {$message}"
  flush $socket_id

}

proc request_game_message_whisper {rid tid nicknames message} {
 #displays a message only to choosed people in the game room
 global socket_id
 puts socket_id "t [join $nicknames ,] gamechatw $rid $tid {$nicknames} {$message}"
 flush $socket_id
# sende $message an alle $nicknames und erwähne, dass dies im $tid geflüstert passiert
}

proc request_private_message {nicknames message} {
 #displays a private message
 global socket_id
 puts $socket_id "t [join $nicknames ,] private {$message}"
 flush $socket_id
}

proc request_current_timer {rid tid} {
 #initialize the timer of a special room and give it to the gui
 global timer_values
 puts "timervalues [lindex $timer_values [expr [string trim $tid t] -1 ]]"
 return [lindex $timer_values [expr [string trim $tid t] -1 ]]
}

proc request_set_timer {rid tid blacktime whitetime} {
 #timer values from the gui of a special table are include into the special list
 global timer_values count_values
 set listelm "{$blacktime $whitetime}"
 set index [expr [string trim $tid t] -1]
 set timer_values [lreplace timer_values $index $index $listelm]
 set count_values $timer_values
}

