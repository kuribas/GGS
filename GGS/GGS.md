This is a short introduction to help getting started
with playing Shogi Variants at the Generic Game Server
quickly. For more info, look at the homepage for GGS:

http://external.nj.nec.com/homepages/igord/usrman.htm


Introduction:
=============

The Generic Games Server (GGS) is a server for many different games.
Each gametype is handled by a different Service.  For the shogi games
this is /shs (Shogi Service).  For any commands related to the game
(i.e. doing moves) you should send information to the Service using
the tell command (i.e.  tell /shs play P-1e).

To save you from typing many commands, GGS provides aliases (and
variables).  When you type an alias it will be expanded to it's full
command (together with any variables).  GGS provides many useful
aliases to make playing on GGS more pleasant.

Logging in:
===========

Connect to the GGS using telnet.  For example:

    telnet skatgame.net 5000

When logging in you will be asked for a name and a password.
If you aren't registered you can choose one freely.

Playing games:
==============

Make the Shogi Variant Service your current Game Service for
all future commands, by typing:

    ms /shs            (Make /shs current Service)

Now you can use the command "ts" to give commands to `/shs`
(`tell /shs`).

Tell it that you are open to play one game at a time:

      ts open 1          (tell /shs open 1)

Find an oponent who is ready to play:

      ts who 0

where 0 is the variant type (Regular shogi in this case)
type "ts help types" to get a list of supported variants.

**note:** *The help files were lost, so `ts help <topic>`
currently doesn't work.  Look at `/GGS/Service/Shogi/src/Board.C`
for a list of supported variants.
*

Sending match request:
======================

      ts ask <variant> <player> 

A match request will be send to <player> if that player is
open for games.

If you don't specify a player, a global match
request will be send to all players that are open to global requests.

To tell the Service that you like to receive global match requests,
type:

     ts request +

To accept a match request, type:

     ts accept <.request>

Playing Moves:
==============

     tp <move>         (ts play <move>)

Make move <move>.  Valid moves are:

     P-3f
     3g-3f
     P*5f

For a complete description type

     ts help moves

Please note that the notation 'x' for captures isn't used.
Use '-' for both moving and capturing.

      ts resign

Resign the game

Talking to People
=================

     t <player> <mesg>

t is an alias for tell

send the text <mesg> to player.  

With the '.' command you can talk to the person you where talking
to previously.

Talking to Channels
===================

You can also talk to channels using the tell command:

    t .chat Hi, everybody.
    t .shogi A question about Shogi...

Channels are created when they are used the first time.
To join a channel, type:
  
    chann + <channel>

To send a message to everyone in the .chat channel, you
can also use yell:

    y Hi, all

Registering
===========

To register, you must ask a sysadmin to register you.
You can find a sysadmin by typing:

    g _admin

You will see

    : group _admin     2 : zardos otheradmin

Tell you want to register:

    t zardos I would like to be registered please

More Information
================

For more info, contact me at 
<kristof at resonata point org>
My handle at GGS is kuribas.
