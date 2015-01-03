#################################
#  GUI for Game Server Project  #
#  Author: Matthias Berg 706145 #
#                               #
#################################
source [pwd]/core.tcl

# init_gui is started to initialize the program. all packages are loaded.
# gui opens with HPI logo and menu bar to connect
proc init_gui {} {
  package require BWidget
  package require Img
  package require Iwidgets

  wm title . "GGS Client"
  wm protocol . WM_DELETE_WINDOW "request_disconnect exit"

  . configure -menu .mbar
  menu .mbar

  .mbar add cascade -label "File" -menu .mbar.file
  menu .mbar.file -tearoff no
  .mbar.file add command -label "Exit" -command exit

  .mbar add cascade -label "Connect" -menu .mbar.connect -command {display_connect}

  .mbar add cascade -label "Settings" -menu .mbar.settings
  menu .mbar.settings -tearoff no
  .mbar.settings add command -label "Server" -command {display_server_settings}

 set mainlogo [image create photo -file [pwd]/gui/mainlogo.png]
 frame .main -width 300 -height 250 -background white
 label .main.logo -image $mainlogo
 pack .main .main.logo


}

# connection dialog with entries to enter login and password
proc display_connect {} {
   global gui
   set gui(defaultlogin) [request_default_login]
   set gui(defaultpass) [request_default_password]
   set gui(login) $gui(defaultlogin)
   set gui(pass) $gui(defaultpass)
   set login [PasswdDlg .pass -logintextvariable gui(login) \
                              -passwdtextvariable gui(pass) \
                              -type okcancel -command {display_save_pass}]

}

# if login or password has been changed, then user is asked if he wants to save changes
proc display_save_pass {} {
  global gui
  if {![string equal $gui(login) $gui(defaultlogin)] | ![string equal $gui(pass) $gui(defaultpass)]} then {
    set response [tk_messageBox -title "Login" -icon question -message  "Save Password?" -type yesno]
    if {$response} then {
     request_save_password $gui(login) $gui(pass)
    }
  }
  destroy .pass
  request_connection $gui(login) $gui(pass)
}

# progressbar for the connection
proc display_connection_progress {value} {
     if {$value == "end"} {
      destroy .progress
   }\
   else {
     global gui
     if {![winfo exists .progress]} then {
        toplevel .progress
        wm title .progress "Connecting game server..."
        set server [lindex [request_server_settings] 0]
        label .progress.label -text "Connecting to $server ..."
        pack .progress.label
        ProgressBar .progress.pbar -variable gui(progressvar) -width 300
        pack .progress.label .progress.pbar -padx 6 -pady 6
     }
     set gui(progressvar) $value
   }
}

# pop-up window for the user to view or change the server settings
proc display_server_settings {} {
   global gui
   set settings [request_server_settings]
   set gui(currentserver) [lindex $settings 0]
   set gui(defaultserver) [lindex $settings 1]

   toplevel .serverset
   wm title .serverset "Server Settings"
   frame .serverset.left
   frame .serverset.main
   frame .serverset.bottom
   pack .serverset.bottom -side bottom -expand yes -fill both
   pack .serverset.main -side right -expand yes -fill both
   pack .serverset.left

   set servericon [image create photo -file [pwd]/gui/servericon.png]
   label .serverset.left.icon -image $servericon
   pack .serverset.left.icon -padx 8 -pady 12 -expand yes

   label .serverset.main.label -text "Enter server and port:"
   pack .serverset.main.label -expand yes -padx 8 -pady 4
   entry .serverset.main.currentserv -textvariable gui(currentserver)
   pack .serverset.main.currentserv -expand yes -padx 8 -pady 8
   button .serverset.bottom.ok -text "OK" -underline 0 -command {
      destroy .serverset
      request_save_server_settings "$gui(currentserver)"
   }
   button .serverset.bottom.default -text "Default" -underline 0 -command {
     set gui(currentserver) $gui(defaultserver)
   }
   button .serverset.bottom.cancel -text "Cancel" -underline 0 -command {
     destroy .serverset
   }
   bind .serverset <Alt-KeyPress-o> {.serverset.bottom.ok invoke}
   bind .serverset <Alt-KeyPress-d> {.serverset.bottom.default invoke}
   bind .serverset <Alt-KeyPress-c> {.serverset.bottom.cancel invoke}

   focus .serverset.bottom.ok
   pack .serverset.bottom.ok .serverset.bottom.default .serverset.bottom.cancel -side left -padx 8 -pady 8
}

##### create room #####
# main room is created using the room id that is given as parameter

proc create_room {rid} {
  toplevel .$rid
  wm title .$rid "Othello Room [string trim $rid r]"
  wm protocol .$rid WM_DELETE_WINDOW "request_disconnect gui_close_room"

  # create and pack menu bar
  frame .$rid.mbar -relief sunken
  pack .$rid.mbar -fill x

  # create, pack and bind menubuttons
  foreach m {{file File} {view View} {options Options} {help Help}} {
      menubutton .$rid.mbar.[lindex $m 0] -text "[lindex $m 1]" -menu .$rid.mbar.[lindex $m 0].m -underline 0
      pack  .$rid.mbar.[lindex $m 0] -side left
      menu .$rid.mbar.[lindex $m 0].m -tearoff 0
  }

  # items in the file menu, not yet specified
  .$rid.mbar.file.m add command -label "Open" -underline 0
  .$rid.mbar.file.m add command -label "Save" -underline 0
  .$rid.mbar.file.m add command -label "Exit" -command {exit} -underline 0

  # just some items for the other menus
  foreach m {{view View} {options Options} {help Help}} {
      .$rid.mbar.[lindex $m 0].m add command -label "1st entry" -underline 0
      .$rid.mbar.[lindex $m 0].m add command -label "2nd entry" -underline 0
      .$rid.mbar.[lindex $m 0].m add command -label "3rd entry" -underline 0
  }

  # below menu bar there'll be a left and a right frame
  # both frames will be splitted into a top and a bottom frame
  frame .$rid.r
  frame .$rid.r.t
  frame .$rid.r.b
  pack .$rid.r -side right -fill both
  pack .$rid.r.t -fill both
  pack .$rid.r.b -fill both -expand yes

  # listbox of all nicknames in the room (with scrollbar)
  scrollbar .$rid.r.t.ysbar_nicks -command ".$rid.r.t.nicks yview"
  listbox .$rid.r.t.nicks -width 20 -height 15 -background white -selectmode extended \
                          -yscrollcommand ".$rid.r.t.ysbar_nicks set"

  bind .$rid.r.t.nicks <Double-ButtonPress> "gui_wholist_doubleclick %y .$rid.r.t.nicks"
  # info box (space for information. dont know what will be in there yet)
  canvas .$rid.r.b.info -width 20 -heigh 20 -background white
 # .$rid.r.b.info create text 30 25 -anchor nw -text "info box"
  pack .$rid.r.t.ysbar_nicks -side right -fill y
  pack .$rid.r.t.nicks -side left
  pack .$rid.r.b.info -side bottom -fill both -expand yes

  # here comes the left frame with its top and bottom frame
  frame .$rid.l
  frame .$rid.l.t
  frame .$rid.l.b
  pack .$rid.l -side left -fill both -expand yes
  pack .$rid.l.t -fill both -expand yes
  pack .$rid.l.b -fill both -expand yes

  # canvas for tables, chairs and avatars
  # entry and text area for the main chat
  scrollbar .$rid.l.t.sbar -command ".$rid.l.t.main yview"
  canvas .$rid.l.t.main -width 600 -heigh 300 -background skyblue \
        -yscrollcommand ".$rid.l.t.sbar set" -scrollregion {0 0 600 400} -yscrollincrement 100
  entry .$rid.l.b.imput
  scrollbar .$rid.l.b.sbar_chat -command ".$rid.l.b.chat yview"
  text .$rid.l.b.chat -yscrollcommand ".$rid.l.b.sbar_chat set" -state disabled -height 12
  gui_set_texttags .$rid.l.b.chat
  pack .$rid.l.b.imput -side bottom -fill x
  pack .$rid.l.b.sbar_chat -side right -fill y
  pack .$rid.l.b.chat -fill both -expand yes
  pack .$rid.l.t.sbar -side right -fill y
  pack .$rid.l.t.main -fill both -expand yes
  bind .r1.l.b.imput <KeyPress-Return>  "gui_send_main_message $rid"
  bind .r1.l.b.imput <Control-KeyPress-Return>  "gui_send_main_message_whisper $rid"

  set backgroundpattern [image create photo -file [pwd]/gui/backgroundpattern.png]
  for {set bgj 0} {$bgj <=2000} {set bgj [expr $bgj+100]} {
    for {set bgi 0} {$bgi <=2000} {set bgi [expr $bgi+100]} {
      .$rid.l.t.main create image $bgi $bgj -anchor nw -image $backgroundpattern -tags background
    }
  }

}
##### end of create room #####


##### display tables #####
# the table constallation for room $rid is displayed
# parameter tablelist consist of lists
# the elements of those lists are:
# tid blackplayername whiteplayername status
# status is private or public
proc display_tables {rid ammount tablelist} {
  puts "$rid $ammount $tablelist"


  set win .$rid.l.t.main
  set mainwidth [$win cget -width]
  $win delete all

  set tableicon [image create photo -file [pwd]/gui/tableicon.png]
  set emptytableicon [image create photo -file [pwd]/gui/emptytableicon.png]
  set blackchair [image create photo -file [pwd]/gui/blackchair.png]
  set whitechair [image create photo -file [pwd]/gui/whitechair.png]
  set blackavatar [image create photo -file [pwd]/gui/blackavatar.png]
  set whiteavatar [image create photo -file [pwd]/gui/whiteavatar.png]
  set keyicon [image create photo -file [pwd]/gui/key.png]
  set backgroundpattern [image create photo -file [pwd]/gui/backgroundpattern.png]


  for {set bgj 0} {$bgj <=2000} {set bgj [expr $bgj+100]} {
    for {set bgi 0} {$bgi <=2000} {set bgi [expr $bgi+100]} {
      .$rid.l.t.main create image $bgi $bgj -anchor nw -image $backgroundpattern -tags background
    }
  }


  set xindex 100
  set yindex 5
  set id 1

  for {set i 1} {$i <= $ammount} {incr i} {
    $win create text $xindex $yindex -tags anchorpointt$id
    $win create image $xindex $yindex -anchor ne -image $blackchair -tags blackchairt$id
    $win create image $xindex $yindex -anchor nw -image $whitechair -tags whitechairt$id
    $win create text $xindex [expr $yindex+25] -text $id -fill yellow -tags tablenumbert$id
    $win create image $xindex [expr $yindex+40] -anchor n -image $emptytableicon -tags tablet$id

    $win bind blackchairt$id <ButtonPress> "request_game_room $rid t$id black"
    $win bind whitechairt$id <ButtonPress> "request_game_room $rid t$id white"
    $win bind tablet$id      <ButtonPress> "request_game_room $rid t$id neutral"

    set xindex [expr $xindex +200]
    incr id
    if {$xindex > $mainwidth} {
      set xindex 100
      set yindex [expr $yindex +100]
    }
  }

  foreach table $tablelist {
    set tid [lindex $table 0]
    set blackplayer [lindex $table 1]
    set whiteplayer [lindex $table 2]
    set status [lindex $table 3]

    set xindex [lindex [$win coords anchorpoint$tid] 0]
    set yindex [lindex [$win coords anchorpoint$tid] 1]

    puts $table
    puts "xindex = $xindex"
    puts "yidnex = $yindex"
    puts "blackplayer = $blackplayer"
    puts "whiteplayer = $whiteplayer"

    if {$blackplayer != ""} then {
      $win create image $xindex $yindex -anchor ne -image $blackavatar -tags blackavatar$tid
      $win create text [expr $xindex-50] $yindex -text $blackplayer -fill yellow -tags blackplayername$tid
      $win bind blackavatar$tid     <ButtonPress> "request_game_room $rid t$id neutral"
      $win bind blackplayername$tid <ButtonPress> "request_game_room $rid t$id neutral"
    }
    if {$whiteplayer != ""} then {
      $win create image $xindex $yindex -anchor nw -image $whiteavatar -tags whiteavatar$tid
      $win create text [expr $xindex+50] $yindex -text $whiteplayer -fill yellow -tags whiteplayername$tid
      $win bind whiteavatar$tid     <ButtonPress> "request_game_room $rid t$id neutral"
      $win bind whiteplayername$tid <ButtonPress> "request_game_room $rid t$id neutral"
    }
    if {($blackplayer != "") | ($whiteplayer != "")} then {
      $win delete table$tid
      $win create image $xindex [expr $yindex+40] -anchor n -image $tableicon -tags table$tid
    }
    if {$status == "private"} then {
      $win create image $xindex [expr $yindex+10] -image $keyicon
    }
  }
  $win configure -scrollregion "0 0 $mainwidth [expr $yindex +100]"
}
##### end of display tables #####

# displays the nicknames of all the persons that are in room $rid
proc display_who {rid nicknames} {
  .$rid.r.t.nicks delete 0 end
  foreach nick $nicknames {.$rid.r.t.nicks insert 0 $nick}
}

                       
##### create game #####
# a game room is created for table $tid
# list of players/kibitzers won't be shown
# after create game, one has to call
# display_game_who and display_boaard
proc create_game {tid titel} {
  toplevel .$tid
  wm title .$tid $titel
  wm protocol .$tid WM_DELETE_WINDOW "
    request_leave_table $tid
    puts {leave table requested}
    destroy .$tid
   "

  set blackavatar [image create photo -file [pwd]/gui/blackavatar.png]
  set whiteavatar [image create photo -file [pwd]/gui/whiteavatar.png]
  set boardframe [image create photo -file [pwd]/gui/boardframe.png]

  frame .$tid.main -background white
  frame .$tid.chat
  pack .$tid.main -side top -fill both
  pack .$tid.chat -side bottom -expand yes -fill both

  canvas .$tid.main.board -background green
  frame .$tid.main.who -background white
  frame .$tid.main.buttons -background white
  frame .$tid.main.left

  pack .$tid.main.left -side left -expand yes -fill both
  pack .$tid.main.board -side left
  pack .$tid.main.buttons -side left -padx 8
  pack .$tid.main.who -side right -expand yes -fill both -padx 8

  entry .$tid.chat.imput
  scrollbar .$tid.chat.sbar -command ".$tid.chat.txt yview"
  text .$tid.chat.txt -yscrollcommand ".$tid.chat.sbar set" -height 9
  gui_set_texttags .$tid.chat.txt
  pack .$tid.chat.imput -side bottom -fill x
  pack .$tid.chat.sbar -side right -fill y
  pack .$tid.chat.txt -fill both -expand yes

  button .$tid.main.buttons.start -text Start -width 5  -relief groove -state disabled \
         -command "request_game_start $tid"
  button .$tid.main.buttons.resign -text Resign -width 5  -relief groov -state disabled \
         -command "request_resign $tid"
  button .$tid.main.buttons.undo -text Undo -width 5    -relief groove -state disabled \
         -command "request_undo $tid"
  button .$tid.main.buttons.timer -text Timer -width 5   -relief groove -state normal \
         -command "gui_display_set_timer r1 $tid"
  button .$tid.main.buttons.switch -text Switch -width 5 -relief groove

  pack .$tid.main.buttons.start
  pack .$tid.main.buttons.resign
  pack .$tid.main.buttons.undo
  pack .$tid.main.buttons.timer
  pack .$tid.main.buttons.switch

  canvas .$tid.main.left.black -width 150 -height 200 -background black
  canvas .$tid.main.left.white -width 150 -height 200 -background white
  grid .$tid.main.left.black -sticky nsew
  grid .$tid.main.left.white -sticky nsew
  grid columnconfigure .$tid.main.left 0 -weight 1
  grid rowconfigure .$tid.main.left 0 -weight 1
  grid rowconfigure .$tid.main.left 1 -weight 1

  .$tid.main.board create image 0 0 -image $boardframe -anchor nw
  set boardbbox [.$tid.main.board bbox all]
  .$tid.main.board configure -width [expr [lindex $boardbbox 2]-2] -height [expr [lindex $boardbbox 3]-2]
  display_board $tid ---------------------------O*------*O---------------------------

  bind .$tid <ButtonPress> ".$tid.main.board delete gameend"
  bind .$tid.chat.imput <KeyPress-Return>  "gui_send_game_message r1 $tid"
}

# displays the constellation of the board on table $tid
proc display_board {tid constellation} {
  set black [image create photo -file [pwd]/gui/black.png]
  set white [image create photo -file [pwd]/gui/white.png]
  set empty [image create photo -file [pwd]/gui/empty.png]

  for {set count 0; set i 1; set j 1} {$count < 64} {incr count} {
    if {[string equal [string index $constellation $count] "-"]} then {
       .$tid.main.board create image [expr ($i-1)*42+23+($i*2)] [expr ($j-1)*42+23+($j*2)] \
                              -image $empty -anchor nw -tags square$i$j
    .$tid.main.board bind square$i$j <ButtonPress> "gui_request_move $tid $i $j"
    }
    if {[string equal [string index $constellation $count] "*"]} then {
       .$tid.main.board create image [expr ($i-1)*42+23+($i*2)] [expr ($j-1)*42+23+($j*2)] \
                              -image $black -anchor nw -tags square$i$j
    }
    if {[string equal [string index $constellation $count] "O"]} then {
       .$tid.main.board create image [expr ($i-1)*42+23+($i*2)] [expr ($j-1)*42+23+($j*2)] \
                              -image $white -anchor nw -tags square$i$j
    }
    incr i
    if {$i>8} {set i 1; incr j}
    if {$j>8} {set j 1}
  }
}
# displays all kibizers and players on table $tid
proc display_game_who {tid blackplayer whiteplayer kibitzer} {
  set blackicon [image create photo -file [pwd]/gui/blackicon.png]
  set whiteicon [image create photo -file [pwd]/gui/whiteicon.png]
  set kibitzicon [image create photo -file [pwd]/gui/kibitzicon.png]
  set blackavatar [image create photo -file [pwd]/gui/blackavatar.png]
  set whiteavatar [image create photo -file [pwd]/gui/whiteavatar.png]
  set blackchair [image create photo -file [pwd]/gui/blackchair.png]
  set whitechair [image create photo -file [pwd]/gui/whitechair.png]

  ### destroy and new set up of who list
  destroy .$tid.main.who
  frame .$tid.main.who -background white
  pack .$tid.main.who -side right -expand yes -fill both -padx 8

  ### delete playernames and turn marks
  .$tid.main.left.black delete playername
  .$tid.main.left.white delete playername
  .$tid.main.left.black delete turn
  .$tid.main.left.white delete turn

  ### who list
  label .$tid.main.who.bicon -image $blackicon -background white
  label .$tid.main.who.bname -text $blackplayer -background white
  grid  .$tid.main.who.bicon .$tid.main.who.bname
  label .$tid.main.who.wicon -image $whiteicon -background white
  label .$tid.main.who.wname -text $whiteplayer -background white
  grid  .$tid.main.who.wicon .$tid.main.who.wname
  set id 0
  foreach kibitz $kibitzer {
    label .$tid.main.who.icon$id -image $kibitzicon -background white
    label .$tid.main.who.name$id -text $kibitz -background white
    grid  .$tid.main.who.icon$id .$tid.main.who.name$id
    incr id
  }

  ### players
  if {$blackplayer != ""} {
    .$tid.main.left.black create image 0 0 -image $blackchair -anchor nw
    .$tid.main.left.black create image 0 0 -image $blackavatar -anchor nw
    .$tid.main.left.black create text [expr [.$tid.main.left.black cget -width]/2] 105 \
                                -text $blackplayer -fill white -font {{MS Sans Serif Bold} 14} -tags playername
  }
  if {$whiteplayer != ""} {
    .$tid.main.left.white create image [.$tid.main.left.white cget -width] \
                                       [.$tid.main.left.white cget -height] -image $whitechair -anchor se
    .$tid.main.left.white create image [.$tid.main.left.white cget -width] \
                                       [.$tid.main.left.white cget -height] -image $whiteavatar -anchor se
    .$tid.main.left.white create text [expr [.$tid.main.left.white cget -width]/2]\
                                      [expr [.$tid.main.left.white cget -height]-105] \
                                -text $whiteplayer -font {{MS Sans Serif Bold} 14} -tags playername
  }
  if {$blackplayer == ""} {
    .$tid.main.left.black create image 0 0 -image $blackchair -anchor nw
  }
  if {$whiteplayer == ""} {
    .$tid.main.left.white create image [.$tid.main.left.white cget -width] \
                                       [.$tid.main.left.white cget -height] -image $whitechair -anchor se
  }
# enable Button "Start" if two players are there, else disable
  if {($blackplayer != "") && ($whiteplayer != "")} then {
    .$tid.main.buttons.start configure -state normal
  }
}

# displays the timer on table $rid
proc display_timer {tid blacktime whitetime} {
  .$tid.main.left.black delete timer
  .$tid.main.left.white delete timer
  .$tid.main.left.black create text [expr [.$tid.main.left.white cget -width]/2]\
                                   [expr [.$tid.main.left.white cget -height]-25]\
                             -text $blacktime -font {{MS Sans Serif Bold} 20} -fill white -tags timer
  .$tid.main.left.white create text [expr [.$tid.main.left.white cget -width]/2] 25 \
                              -text $whitetime -font {{MS Sans Serif Bold} 20} -tags timer
}

# displays the current "result", the disccount
proc display_disccount {tid blackcount whitecount} {
   .$tid.main.left.black delete count
   .$tid.main.left.white delete count

  .$tid.main.left.black create text [expr [.$tid.main.left.black cget -width]-5] 5 -text $blackcount \
                               -anchor ne -fill white -font {{MS Sans Serif Bold} 18} -tag count
  .$tid.main.left.white create text 35 [expr [.$tid.main.left.white cget -height]-5] -text $whitecount \
                               -anchor se  -font {{MS Sans Serif Bold} 18} -tag count

}
# mark the player who is at his turn i
proc display_turn {tid side} {
 set blackicon [image create photo -file [pwd]/gui/blackicon.png]
 set whiteicon [image create photo -file [pwd]/gui/whiteicon.png]
 .$tid.main.left.black delete turn
 .$tid.main.left.white delete turn
 if {$side == "black"} then {
   .$tid.main.left.black create rectangle [.$tid.main.left.black bbox playername]\
     -outline black -width 2 -tags turn
   .$tid.main.left.black create rectangle [.$tid.main.left.black bbox turn]\
     -outline RoyalBlue -width 2 -tags turn
 }
 if {$side == "white"} then {
     .$tid.main.left.white create rectangle [.$tid.main.left.white bbox playername]\
       -outline white -width 2 -tags turn
     .$tid.main.left.white create rectangle [.$tid.main.left.white bbox turn]\
       -outline RoyalBlue -width 2 -tags turn
 }

}

# help prozedure that converts column indexes into letters
proc gui_request_move {tid column row} {
  switch $column {
     1 {set col a}
     2 {set col b}
     3 {set col c}
     4 {set col d}
     5 {set col e}
     6 {set col f}
     7 {set col g}
     8 {set col h}
  }
   request_move $tid $col$row
}
# displays a gamestart and changes the buttons to playing status
proc display_game_start {tid} {
  .$tid.main.buttons.start configure -state disabled
  .$tid.main.buttons.resign configure -state normal
  .$tid.main.buttons.undo configure -state normal
  .$tid.main.buttons.timer configure -state disabled
  .$tid.main.buttons.switch configure -state disabled
  .$tid.chat.txt insert end "Systemmessage - Game started, good luck!\n"
}

# displays result of the game and changes button to not playing status
proc display_game_end {tid kind blackplayer whiteplayer blackscore whitescore reason} {
  if {[winfo exists .$tid]} {
    .$tid.main.buttons.start configure -state normal
    .$tid.main.buttons.resign configure -state disabled
    .$tid.main.buttons.undo configure -state disabled
    .$tid.main.buttons.timer configure -state normal
    .$tid.main.buttons.switch configure -state normal

    set gameendbackground [image create photo -file [pwd]/gui/gameendbackground.png]
    if {$kind == "win"} then {
      set winner $blackplayer
      set loser $whiteplayer}
    if {$kind == "loss"} then {
      set winner $whiteplayer
      set loser $blackplayer}

    .$tid.main.board delete gameend
    .$tid.main.board create image 200 200 -image $gameendbackground -tags gameend
    if {($kind == "win") ^ ($kind == "loss")} then {
      .$tid.main.board create text 200 200 -font {{MS Sans Serif Bold} 16} -justify center -fill DarkOrange3\
          -text "$winner\ndefeated\n$loser\n\n$blackscore - $whitescore" -tags gameend
      .$tid.chat.txt insert end "Systemmessage - $winner defeated $loser $blackscore - $whitescore"
      if {$reason == "resigned"} then {
        .$tid.chat.txt insert end " ($loser resigned)"
      }
      if {$reason == "timeout"} then {
        .$tid.chat.txt insert end " ($loser exceeded timelimit)"
      }
      .$tid.chat.txt insert end "\n"
    }\
    elseif {$kind == "draw"} then {
      .$tid.main.board create text 200 200 -font {{MS Sans Serif Bold} 16} -justify center -fill DarkOrange3\
          -text "$blackplayer\ndrew\n$whiteplayer\n\n$blackscore - $whitescore" -tags gameend
      .$tid.chat.txt insert end "Systemmessage - $blackplayer and $whiteplayer played a draw $blackscore - $whitescore\n"
    }
  }
}

# asks player if he accepts the undo-request of the opponent
proc display_question_undo {tid opponent} {
  global gui_undoConfirm
  raise .$tid

  set undoicon [image create photo -file [pwd]/gui/undoicon.png]
  toplevel .undo$tid
  wm title .undo$tid "Undo requested"
  frame .undo$tid.top
  label .undo$tid.top.icon -image $undoicon
  label .undo$tid.top.mesg -text "$opponent asked for undo. \nDo you want to permit?"
  pack .undo$tid.top.icon -side left
  pack .undo$tid.top.mesg -side right

  frame .undo$tid.controls
  button .undo$tid.controls.yes -text "Yes" -command "set gui_undoConfirm yes"
  button .undo$tid.controls.always -text "Always" -command "set gui_undoConfirm always"
  button .undo$tid.controls.no -text "No" -command "set gui_undoConfirm no"
  button .undo$tid.controls.never -text "Never" -command "set gui_undoConfirm never"
  pack .undo$tid.controls.yes -side left -padx 4
  pack .undo$tid.controls.always -side left -padx 4
  pack .undo$tid.controls.no -side left -padx 4
  pack .undo$tid.controls.never -side left -padx 4
  focus .undo$tid.controls.no

  pack .undo$tid.top -padx 8 -pady 8
  pack .undo$tid.controls -pady 4

  wm protocol .undo$tid WM_DELETE_WINDOW ".undo$tid.controls.no invoke"
  gui_dialog_wait .undo$tid gui_undoConfirm  $tid
  destroy .undo$tid
  return $gui_undoConfirm
}


# displays that undo was rejected with a message in the game chat
proc display_undo_denied {tid} {
  .$tid.chat.txt insert end "Systemmessage - Undo denied\n"
}

# displays that undo was rejected with a message in the game chat
proc display_undo {tid} {
  .$tid.chat.txt insert end "Systemmessage - one move taken back\n"
}

# displays that undo was rejected with a message in the game chat
proc display_undo_denied {tid} {
  .$tid.chat.txt insert end "Systemmessage - Undo denied\n" Systemmessage
}

# help procedure do allow modal dialogs
proc gui_dialog_wait {win varName tid} {
  gui_dialog_safeguard $win
  set x [expr [winfo rootx .$tid]+250]
  set y [expr [winfo rooty .$tid]+141]
  wm geometry $win "+$x+$y"
  wm deiconify $win
  grab set $win
  vwait $varName
  wm withdraw $win
}

bind modalDialog <ButtonPress> {
  wm deiconify %W
  raise %W
}

# another help procedure to allow modal dialogs
proc gui_dialog_safeguard {win} {
  if {[lsearch [bindtags $win] modalDialog] < 0} {
    bindtags $win [linsert [bindtags $win] 0 modalDialog]
  }
}

# help procedure to set the tags for the text fields
proc gui_set_texttags {win} {
  $win tag configure "systemmessage" -foreground green
  $win tag configure "nickname" -foreground blue
  $win tag configure "message" -foreground black
  $win tag configure "whisper" -foreground skyblue
}

# help procedure to get the concent of the entry in $room D
proc gui_send_main_message {rid} {
  set message [.$rid.l.b.imput get]
  request_main_message $rid $message
  .$rid.l.b.imput delete 0 end
}

# help procedure to get the content of the entry in the main room and send to all persons
proc gui_send_game_message {rid tid} {
  set message [.$tid.chat.imput get]
  request_game_message $rid $tid $message
  .$tid.chat.imput delete 0 end
}

# help procedure to get the content of the entry in the main room and to whisper to the selected person
proc gui_send_main_message_whisper {rid} {
  list nicknames
  foreach index [.$rid.r.t.nicks curselection] {
    lappend nicknames [.$rid.r.t.nicks get $index]
  }
  if {$nicknames != ""} {
      request_main_message_whisper $rid $nicknames [.$rid.l.b.imput get];
      .$rid.l.b.imput delete 0 end
  }
}

# help procedure to get the content of the entry in the game room and to whisper to the selected person
# selecting persons isnt possible for now. so it's not needed in this version. but maybe soon
proc gui_send_game_message_whisper {rid tid} {
  request_game_message_whisper $rid $tid $nicknames [.$tid.chat.imput get];
  .$tid.chat.imput delete 0 end
}

# displays the message that came in the main room
proc display_main_message {rid nickname message} {
  puts "$nickname wrote: $message"
  .$rid.l.b.chat configure -state normal
  .$rid.l.b.chat insert end "$nickname: " nickname
  .$rid.l.b.chat insert end "$message\n"  message
  .$rid.l.b.chat configure -state disabled
}

# displays the message that came in a gameroom
proc display_game_message {rid tid nickname message} {
  .$tid.chat.txt configure -state normal
  .$tid.chat.txt insert end "$nickname: " nickname
  .$tid.chat.txt insert end "$message\n"  message
  .$tid.chat.txt configure -state disabled
}

# displayed whispered message in main room
proc display_main_message_whisper {rid nickname nicknames message} {
  .$rid.l.b.chat configure -state normal
  .$rid.l.b.chat insert end "$nickname whispers to" whisper
  foreach name $nicknames {
  .$rid.l.b.chat insert end " $name" whisper
  }
  .$rid.l.b.chat insert end ": " whisper
  .$rid.l.b.chat insert end "$message\n"  whisper
  .$rid.l.b.chat configure -state disabled
}

# displays whispered message in game room
# because sending whispers isnt possbible yet, this feature isnt used neither
proc display_game_message_whisper {rid tid nickname nicknames message} {
  .$tid.chat.txt configure -state normal
  .$tid.chat.txt insert end "$nickname: " nickname
  .$tid.chat.txt insert end "$nickname whispers to" whisper
  foreach name $nicknames {
  .$tid.chat.txt insert end " $name" whisper
  }
  .$tid.chat.txt insert end ": " whisper
  .$tid.chat.txt insert end "$message\n"  whisper
  .$tid.chat.txt configure -state disabled
}

# help procede du catch the doubleclicks on the main windows to start writing a private message
proc gui_wholist_doubleclick {coord win} {
  set index [$win nearest $coord]
  set nickname [$win get $index]
  gui_display_write_private_message $nickname
}

# help porcedure to set timer of the table
proc gui_display_set_timer {rid tid} {
  set win .timer$rid$tid
  if {[winfo exists $win]} then {raise $win} \
  else {
    set blackicon [image create photo -file [pwd]/gui/blackicon.png]
    set whiteicon [image create photo -file [pwd]/gui/whiteicon.png]
    global gui

    toplevel $win
    wm title $win "Set timer for table [string trim $tid t]"

    frame $win.black
    frame $win.line1 -height 2 -borderwidth 1 -relief sunken
    frame $win.white
    frame $win.line2 -height 2 -borderwidth 1 -relief sunken
    frame $win.link
    frame $win.line3 -height 2 -borderwidth 1 -relief sunken
    frame $win.buttons
    pack $win.buttons $win.line3 $win.link $win.line2 $win.white $win.line1 $win.black -side bottom -fill both -pady 4

    pack $win.black -expand yes
    pack $win.white -expand yes

    set blacktime [lindex [request_current_timer $rid $tid] 0]
    set whitetime [lindex [request_current_timer $rid $tid] 1]
    if {$blacktime == $whitetime} then {set gui(linktime$rid$tid) 1} else {set gui(linktime$rid$tid) 0}


    button $win.buttons.ok -text "Ok" -command "gui_request_set_timer $rid $tid $win {destroy $win}" -width 5
    button $win.buttons.apply -text "Apply" -command "gui_request_set_timer $rid $tid $win {}" -width 5
    button $win.buttons.cancel -text "Cancel" -command "destroy $win" -width 5
    pack $win.buttons.ok $win.buttons.apply $win.buttons.cancel -side left -padx 8 -pady 4

    checkbutton $win.link.check -text "link times" -variable gui(linktime$rid$tid)
    pack $win.link.check

    label $win.black.icon -image $blackicon
    iwidgets::spintime $win.black.timer
    $win.black.timer show $blacktime
    pack $win.black.timer $win.black.icon -side right -fill both -expand yes

    label $win.white.icon -image $whiteicon
    iwidgets::spintime $win.white.timer
    $win.white.timer show $whitetime
    pack $win.white.timer $win.white.icon -side right -fill both -expand yes

    bind $win <ButtonRelease> "gui_link_timer $rid $tid %W $win"
  }
}

# help procedure to link the timers on the timersetting pop-up
proc gui_link_timer {rid tid path win} {
  global gui
  if {$gui(linktime$rid$tid)} {
    if {[string first white $path] != "-1"} then {
       set original $win.white.timer
       set copy $win.black.timer
    }\
    else {
       set original $win.black.timer
       set copy $win.white.timer
     }
    $copy show [$original get]
  }
}

# help procedure to make the last step to send the request to the core
proc gui_request_set_timer {rid tid win command} {
  set blacktime [$win.black.timer get]
  set whitetime [$win.white.timer get]
  request_set_timer $rid $tid $blacktime $whitetime
  eval $command
}

# help procedure to start writing the private message
proc gui_display_write_private_message {nickname} {
  set count 0
  set win ".privatemessage[incr count]"
  while {[winfo exists $win]} {
         set win ".privatemessage[incr count]"
  }

  toplevel $win
  wm title $win "Send private message to $nickname"

  frame $win.buttons -relief sunken
  frame $win.main
  pack $win.buttons -side bottom -fill both
  pack $win.main -expand yes -fill both

  button $win.buttons.send -text "Send" -width 6 -relief groove \
         -command "gui_send_private_message $win $nickname"
  button $win.buttons.cancel -text "Cancel" -command {destroy $win} -width 6 -relief groove
  pack $win.buttons.send $win.buttons.cancel -side left -padx 8 -pady 4

  scrollbar $win.main.sbar -command "$win.main.txt yview"
  text $win.main.txt -width 50 -height 10 -yscrollcommand "$win.main.sbar set"

  pack $win.main.sbar -side right -fill y
  pack $win.main.txt -expand yes -fill both
  focus $win.main.txt
  bind $win <Control-KeyPress-Return> "$win.buttons.send invoke"

}

# help prozedur to send the match request
proc gui_send_private_message {win nicknames} {
  set message [$win.main.txt get 1.0 "end -1 char"]
  request_private_message $nicknames $message
  destroy $win
}

# displays a private message and the nickame it came from
proc display_private_message {nickname message} {
  set count 0
  set win ".privatemessage[incr count]"
  while {[winfo exists $win]} {
         set win ".privatemessage[incr count]"
  }

  toplevel $win
  wm title $win "$nickname has sent a private message to you"

  frame $win.buttons -relief sunken
  frame $win.main
  pack $win.buttons -side bottom -fill both
  pack $win.main -expand yes -fill both

  button $win.buttons.reply -text "Reply" -width 6 -relief groove \
         -command "gui_display_write_private_message $nickname; destroy $win"
  button $win.buttons.ok -text "ok" -command "destroy $win" -width 6 -relief groove
  pack $win.buttons.reply $win.buttons.ok -side left -padx 8 -pady 4

  scrollbar $win.main.sbar -command "$win.main.txt yview"
  text $win.main.txt -width 50 -height 10 -yscrollcommand "$win.main.sbar set" -state disabled

  pack $win.main.sbar -side right -fill y
  pack $win.main.txt -expand yes -fill both
  $win.main.txt configure -state normal
  $win.main.txt insert 1.0 $message
  $win.main.txt configure -state disabled
  focus $win.buttons.reply
}

# asks the user if he really wants to disconnect
proc display_question_disconnect {} {
 set answer [tk_messageBox -icon question  -type yesno -title "Disconnect?" \
                            -message "Do you really want to disconnect?" ]
  return $answer
}

# asks user if he wants to resign
proc display_question_resign {tid} {
  set answer [tk_messageBox -icon question -title "Resign?" -type yesno \
                            -message "Do you want to resign your game at table $tid"]
  return "$tid $answer"
}

# help procedure to close room
proc gui_close_room {} {
  destroy .r1
}

init_gui
