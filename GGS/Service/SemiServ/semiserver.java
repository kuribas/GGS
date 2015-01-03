// written by Sebastian Preetz, 706324
// (c) 15 July 2002
// the Semiserver is a part of the Game Server Project
// for the lecture "Concepts of Programming II"
// at Hasso-Plattner-Institut for Software Systems Engineering
// at the University of Potsdam, Germany

import java.io.*;
import java.net.*;
import java.util.*;

/******************************************************************************/
class members // for the members
{
  LinkedList liste; // members list

  // constructor
  public members()
  {
    liste=new LinkedList();
  }

  // it saves the members on hard disc
  public void write_members()
  {
    try
    {
    File f = new File("members.txt");
    PrintWriter out = new PrintWriter(new FileWriter(f));
    while (liste.isEmpty()==false)
    {
      out.println(liste.removeFirst());
    }
    out.close();
    }
    catch(IOException e)
    {
      System.out.println("It wasn't possible to save the members.");
    }
  }

  // it loads the members from hard disc
  public void read_members()
  {
    try
    {
      BufferedReader in = new BufferedReader(new FileReader("members.txt"));
      String line;
      while((line=in.readLine())!=null)
      {
        if (liste.contains(line)==false)
        {
          liste.addLast(line);
        }
      }
      in.close();
    }
      catch(IOException e)
    {
      System.out.println("It wasn't possible to load the members.");
    }
  }

  // it insert a new member
  public boolean add_member(String name)
  {
    if (liste.add(name)==true)
      return true;
    else
      return false;
  }

  // it deletes the member "name"
  public boolean delete_member(String name)
  {
    if (liste.remove(name)==true)
      return true;
    else
      return false;
  }

  // it checks whether name is a member
  public boolean contain_member(String name)
  {
    if (liste.contains(name)==true)
      return true;
    else
      return false;
  }

}
/******************************************************************************/
class participants // for the participants
{
  LinkedList liste; // participants list
  int anzahl; // number of participants

  // constructor
  public participants()
  {
    liste=new LinkedList();
    anzahl=0;
  }

  // it returns a pointer of a copie of the participants list
  public LinkedList get_participant_list()
  {
    LinkedList newlist = new LinkedList();
    for(int i=0;i<anzahl;i++)
    {
      newlist.add(liste.get(i));
    }
    return newlist;
  }

  // it returns the number of participants
  public int get_anzahl()
  {
      return anzahl;
  }

  // it inserts a new participant
  public boolean add_participant(String name)
  {
    if (liste.add(name)==true)
    {
      anzahl=anzahl+1;
      return true;
    } else
    {
      return false;
    }
  }

  // it deletes the participant "name"
  public boolean delete_participant(String name)
  {
    if (liste.remove(name)==true)
    {
      anzahl=anzahl+1;
      return true;
    } else
    {
      return false;
    }
  }

  // it deletes all participants
  public void delete_all_participants()
  {
    while(anzahl>0)
    {
      liste.removeFirst();
      anzahl=anzahl-1;
    }
  }

  // it checks whether the the name is in the participants list
  public boolean contain_participant(String name)
  {
    if (liste.contains(name)==true)
      return true;
    else
      return false;
  }

  // it returns the participants list
  public String get_participants()
  {
    StringBuffer s = new StringBuffer("get_participants {");
    for(int i=0;i<anzahl;i++)
    {
      s.append(liste.get(i));
      if (i<(anzahl-1)) s.append(" ");
    }
    s.append("}");
    return s.toString();
  }
}
/******************************************************************************/
class table // the tables
{
  String tid; // table id
  String black_player; // black player
  String white_player; // white player
  LinkedList kibitzer; // kibitzer list
  String status; // status {public,private}
  int anz_kibitzer; // number of kibitzer
  String game_id; // game id

  // constructor
  public table(String id)
  {
    tid=id;
    black_player = "{}";
    white_player = "{}";
    kibitzer = new LinkedList();
    status = "public";
    anz_kibitzer=0;
    game_id = "{}";
  }

  // it returns the table id
  public String get_tid()
  {
    return tid;
  }

  // it sets the game id
  public void set_game_id(String name)
  {
    game_id=name;
  }

  // it returns the game id
  public String get_game_id()
  {
    return game_id;
  }

  // it lets the participant be black player
  public boolean set_black_player(String name)
  {
    if (black_player.equals("{}"))
    {
      black_player=name;
      return true;
    } else
    {
      return false;
    }
  }

  // it resets black player
  public boolean release_black_player(String name)
  {
    if (black_player.equals(name))
    {
        black_player="{}";
        return true;
    } else
    {
      return false;
    }
  }

  // it returns black player
  public String get_black_player()
  {
    return black_player;
  }

  // it lets the participant be white player
  public boolean set_white_player(String name)
  {
    if (white_player.equals("{}"))
    {
      white_player=name;
      return true;
    } else
    {
      return false;
    }
  }

  // it resets white player
  public boolean release_white_player(String name)
  {
    if (white_player.equals(name))
    {
      white_player="{}";
      return true;
    } else
    {
      return false;
    }
  }

  // it returns white player
  public String get_white_player()
  {
    return white_player;
  }

  // it changes the places of white and black player
  public void switch_places()
  {
    String help_player;
    help_player=white_player;
    white_player=black_player;
    black_player=help_player;
  }

  // it inserts a participant into the kibitzer list
  public boolean insert_kibitzer(String name)
  {
    if ((status=="public")&&(!(kibitzer.contains(name))))
    {
      kibitzer.addLast(name);
      anz_kibitzer=anz_kibitzer+1;
      return true;
    }
    else
    {
      return false;
    }
  }

  // it deletes a paricipant from the kibitzer list
  public boolean delete_kibitzer(String name)
  {
    if (anz_kibitzer>0)
    {
      if (kibitzer.remove(name))
      {
        anz_kibitzer=anz_kibitzer-1;
        return true;
      } else
      {
        return false;
      }
    } else
    {
      return false;
    }
  }

  // it returns the list of kibitzer as a String
  public String get_kibitzer()
  {
    StringBuffer s = new StringBuffer("");
    for(int i=0;i<anz_kibitzer;i++)
    {
      s.append((String)kibitzer.get(i));
      if (i<(anz_kibitzer-1)) s.append(" ");
    }
    return s.toString();
  }

  // it returns the status
  public String get_status()
  {
    return status;
  }

  // it sets the status
  public boolean set_status(String s)
  {
    if (s.equals("public"))
    {
      status=s;
      return true;
    } else
    {
      if (s.equals("private"))
      {
        status=s;
        return true;
      }
      else
      {
        return false;
      }
    }
  }
}
/******************************************************************************/
class room // for the rooms
{
  LinkedList tische; // table list
  int anzahl; // number of tables
  int rid; // room id

  // constructor
  public room(int id,int n)
  {
    tische = new LinkedList();
    anzahl=0;
    rid=id;
    add_tables(n);
  }

  // it gives the number of tables in this room
  public int get_anzahl()
  {
    return anzahl;
  }

  // it adds one new table
  public void add_table()
  {
    table t = new table(String.valueOf(anzahl+1));
    tische.addLast(t);
    anzahl=anzahl+1;
  }

  // it creates n new tables in the room
  public void add_tables(int n)
  {
    for(int i=1;i<=n;i++)
    {
      add_table();
    }
  }

  // it deletes all tables
  public void delete_all_tables()
  {
    for(int i=1;i<=anzahl;i++)
    {
      tische.removeLast();
    }
    anzahl=0;
  }

  // it returns the config of the tables
  public String show_config_tables()
  {
    table t;
    StringBuffer s = new StringBuffer("show_config_tables {");
    s.append(rid);
    s.append(" ");
    s.append(anzahl);
    s.append(" ");
    for(int i=0;i<anzahl;i++)
    {
      s.append("{");
      t=(table)tische.get(i);
      // table id
      s.append(t.get_tid());
      s.append(" ");
      // player black
      s.append(t.get_black_player());
      s.append(" ");
      // player white
      s.append(t.get_white_player());
      // kibitzer
      s.append(" {");
      s.append(t.get_kibitzer());
      s.append("} ");
      // status
      s.append(t.get_status());
      if (i<(anzahl-1))
        s.append("} ");
      else
        s.append("}");
    }
    s.append("}");
    return s.toString();
  }
}
/******************************************************************************/
public class semiserver // the main class
{
  public static void main(String[] args)
  {
    members mitglieder = new members(); // members list
    mitglieder.read_members(); // read members from harddisk
    // create room 1 with 24 tables
    room r1 = new room(1,24);
    // create empty participants list
    participants p = new participants();
    try
    {
      /************************************************************************/
      // address of the GGS
      String host="ftp.nj.nec.com";
      // port of the GGS
      int port=5000;
      Socket s=new Socket(host,port);
      // from Server
      InputStream sin = s.getInputStream();
      BufferedReader fromServer = new BufferedReader(new InputStreamReader(sin));
      // to Server
      OutputStream sout = s.getOutputStream();
      PrintWriter toServer = new PrintWriter(new OutputStreamWriter(sout));
      // send the login to GGS
      for (int i=0;i<8;i++ )
      System.out.println(fromServer.readLine());
      toServer.print("semiserv\n");
      toServer.flush();
      // send the password to GGS
      for (int i=0;i<1;i++ )
      System.out.println(fromServer.readLine());
      toServer.print("steel\n");
      toServer.flush();
      /************************************************************************/

      int request_number=0; //number of requests since last update request
      // add semiserv to the channel, if nessesary create the channel
      toServer.print("c + .kdp\n");
      toServer.flush();
      String line = "";

      // "smp" is able to shut down the semiserver extern,
      // so you don't need Control-C in all cases
      while(!line.equals("smp: Semiserver exit")) //extern beenden
      // you can change "smp" into your GGS login
      {
        // read request line
        line = fromServer.readLine();
        // is the request in real the update info for the channel .kdp
        if (line.startsWith(": chann .kdp"))
        {
          // display info for the admin
          System.out.println("update participants list");
          while (p.get_anzahl()!=0)
          {
            // clear the participants list
            p.delete_all_participants();
          }
          //we start whith the secound letter, because the first ist ":" too
          int m=1;
          while ((m<line.length())&&(line.charAt(m)!=':'))
          {
            m=m+1;
          }
          // the char contains now ":" at the position m
          m=m+2; // set m to the first letter of the first name
          while (m<line.length())
          {
            StringBuffer p_name = new StringBuffer("");
            // we are now at the first letter of a name
            // get the name
            while ((m<line.length())&&(line.charAt(m)!=' '))
            {
              p_name.append(line.charAt(m));
              m=m+1;
            }
            // check the name
            if (mitglieder.contain_member(p_name.toString()))
            {
              // if she/he is a member add she/he to the participants
              p.add_participant(p_name.toString());
              // display the name on the screen for the admin
              System.out.println(p_name.toString());
            }
            // if there is the char at m is black
            if ((m<line.length())&&(line.charAt(m)==' '))
            {
              m=m+1; //we switch to the next start position
            }
          }
          // we check now the tables 1 to 24 in the first room
          for (int u=0;u<(r1.get_anzahl());u++)
          {
            // if the black player is away reset black_player
            if (!(p.contain_participant(((table)r1.tische.get(u)).get_black_player())))
            {
                ((table)r1.tische.get(u)).black_player="{}";
            }
            // if the white player is away reset white_player
            if (!(p.contain_participant(((table)r1.tische.get(u)).get_white_player())))
            {
                ((table)r1.tische.get(u)).white_player="{}";
            }
            // if at least one of the player is away delete game id
            if ((((table)r1.tische.get(u)).get_white_player().equals("{}"))||(((table)r1.tische.get(u)).get_black_player().equals("{}")))
            {
                ((table)r1.tische.get(u)).game_id="{}";
            }
            // if black and white player are away then change status to public
            if ((((table)r1.tische.get(u)).get_white_player().equals("{}"))&&(((table)r1.tische.get(u)).get_black_player().equals("{}")))
            {
                ((table)r1.tische.get(u)).status="public";
            }
            // now we check the kibitzer
            int a=0;
            while (a<((table)r1.tische.get(u)).anz_kibitzer)
            {
              if (p.contain_participant((String)((table)r1.tische.get(u)).kibitzer.get(a)))
              {
                a=a+1;
              } else
              {
                ((table)r1.tische.get(u)).kibitzer.remove(a);
                ((table)r1.tische.get(u)).anz_kibitzer=((table)r1.tische.get(u)).anz_kibitzer-1;
              }
            }
          }
        }
        // count the requests to the semiserver
        if (request_number<10)
        {
          request_number=request_number+1;
        } else
        {
          request_number=0;
          // if reach the request limit, ask the GGS "Who is in channel .kdp?"
          toServer.print("c .kdp\n");
          toServer.flush();
        }

        // nickname
        StringBuffer nick_name = new StringBuffer("");
        int i=0;
        // get nickname
        while ((i<line.length())&&(line.charAt(i)!=':'))
        {
          nick_name.append(line.charAt(i));
          i=i+1;
        }
        // check the nickname whether he/she is a membername
        if (mitglieder.contain_member(nick_name.toString()))
        {
          // if the sender isn't in the participants list sign him/her in
          if (!p.contain_participant(nick_name.toString()))
          {
            p.add_participant(nick_name.toString());
          }
          i=i+2;
          StringBuffer befehl = new StringBuffer("");
          // Befehl einlesen, es steht schon fest das es einer der Clients ist der sendet
          while((i<line.length())&&(line.charAt(i)!=' '))
          {
            befehl.append(line.charAt(i));
            i=i+1;
          }

          // if the command is "get_participants"
          if (befehl.toString().equals("get_participants"))
          {
            toServer.print("t ");
            toServer.print(nick_name.toString());
            toServer.print(" ");
            toServer.print(p.get_participants());
            toServer.print("\n");
            toServer.flush();
          }

          // if the command is "show_config_tables"
          if (befehl.toString().equals("show_config_tables"))
          {
            toServer.print("t ");
            toServer.print(nick_name.toString());
            toServer.print(" ");
            toServer.print(r1.show_config_tables());
            toServer.print("\n");
            toServer.flush();
          }

          // for some commands we need the table id
          int tisch_nr = -1; // the table id as an integer
          StringBuffer tisch_id = new StringBuffer(""); // the table id as a StringBuffer

          // if the command is "game_black"
          if (befehl.toString().equals("game_black"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            {
              // now the table id is in their range
              if ((((table)r1.tische.get(tisch_nr)).set_black_player(nick_name.toString())))
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("game_black true");
                toServer.print("\n");
                toServer.flush();
              } else
              {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("game_black false");
              toServer.print("\n");
              toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("game_black false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command is "game_white" ist
          if (befehl.toString().equals("game_white"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            {
              // now the table id is in their range
              if ((((table)r1.tische.get(tisch_nr)).set_white_player(nick_name.toString())))
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("game_white true");
                toServer.print("\n");
                toServer.flush();
              } else
              {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("game_white false");
              toServer.print("\n");
              toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("game_white false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command is "view_table"
          if (befehl.toString().equals("view_table"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            {
              // now the table id is in their range
              if (((table)r1.tische.get(tisch_nr)).insert_kibitzer(nick_name.toString()))
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("view_table true");
                toServer.print("\n");
                toServer.flush();
              } else
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("view_table false");
                toServer.print("\n");
                toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("view_table false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command is "stop_game"
          if (befehl.toString().equals("stop_game"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            {
              if (((table)r1.tische.get(tisch_nr)).release_black_player(nick_name.toString()))
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("stop game true");
                toServer.print("\n");
                toServer.flush();
              } else
              {
                if (((table)r1.tische.get(tisch_nr)).release_white_player(nick_name.toString()))
                {
                  toServer.print("t ");
                  toServer.print(nick_name.toString());
                  toServer.print(" ");
                  toServer.print("stop game true");
                  toServer.print("\n");
                  toServer.flush();
                } else
                {
                  toServer.print("t ");
                  toServer.print(nick_name.toString());
                  toServer.print(" ");
                  toServer.print("stop game false");
                  toServer.print("\n");
                  toServer.flush();
                }
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("stop_game false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command is "stop_view"
          if (befehl.toString().equals("stop_view"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            {
              // now the table id is in their range
              if (((table)r1.tische.get(tisch_nr)).delete_kibitzer(nick_name.toString()))
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("stop_view true");
                toServer.print("\n");
                toServer.flush();
              } else
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("stop_view false");
                toServer.print("\n");
                toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("stop_view false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command is "set_status_public"
          if (befehl.toString().equals("set_status_public"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            // now the table id is in their range
            {
              if (((table)r1.tische.get(tisch_nr)).get_white_player().equals(nick_name.toString())
              ||(((table)r1.tische.get(tisch_nr)).get_black_player().equals(nick_name.toString())))
              {
                if (((table)r1.tische.get(tisch_nr)).set_status("public"))
                {
                  toServer.print("t ");
                  toServer.print(nick_name.toString());
                  toServer.print(" ");
                  toServer.print("set_status_public true");
                  toServer.print("\n");
                  toServer.flush();
                } else
                {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("set_status_public false");
                toServer.print("\n");
                toServer.flush();
                }
              } else
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("set_status_public false");
                toServer.print("\n");
                toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("set_status_public false");
              toServer.print("\n");
              toServer.flush();
            }
          }


          // if the command is "set_status_private"
          if (befehl.toString().equals("set_status_private"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            // now the table id is in their range
            {
              if (((table)r1.tische.get(tisch_nr)).get_white_player().equals(nick_name.toString())
              ||(((table)r1.tische.get(tisch_nr)).get_black_player().equals(nick_name.toString())))
              {
                if (((table)r1.tische.get(tisch_nr)).set_status("private"))
                {
                  toServer.print("t ");
                  toServer.print(nick_name.toString());
                  toServer.print(" ");
                  toServer.print("set_status_private true");
                  toServer.print("\n");
                  toServer.flush();
                } else
                {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("set_status_private false");
                toServer.print("\n");
                toServer.flush();
                }

              } else
              {
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("set_status_private false");
                toServer.print("\n");
                toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("set_status_private false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command is "get_game_ids"
          if (befehl.toString().equals("get_game_ids"))
          {
            toServer.print("t ");
            toServer.print(nick_name.toString());
            toServer.print(" get_game_ids {");
            for(int l=0;l<(r1.get_anzahl());l++)
            {
              toServer.print(((table)r1.tische.get(l)).get_game_id());
              if (l<(r1.get_anzahl()-1))
              {
                toServer.print(" ");
              }
            }
            toServer.print("}\n");
            toServer.flush();
          }

          // if the command is "set_game_id"
          if (befehl.toString().equals("set_game_id"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // get the game id from line
            tisch_id=new StringBuffer("");
            while (((i+5)<(line.length()))&&(line.charAt(i+5)!='>'))
            {
              tisch_id.append(line.charAt(i+5));
              // I use the StringBuffer tisch_id here because was saved in tisch_nr,
              // so I don't need a new variable
              i=i+1;
            }
            // sent output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            // now the table id is in their range
            {
              if ((((table)r1.tische.get(tisch_nr)).get_white_player().equals(nick_name.toString())
                ||((table)r1.tische.get(tisch_nr)).get_black_player().equals(nick_name.toString()))
                && (!((table)r1.tische.get(tisch_nr)).get_white_player().equals("{}"))
                && (!((table)r1.tische.get(tisch_nr)).get_black_player().equals("{}")))

              // now the sender is white or black player and there are to player on the table
              {
                ((table)r1.tische.get(tisch_nr)).set_game_id(tisch_id.toString());
                // I use tisch_id here as a variable for the game id
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("set_game_id true");
                toServer.print("\n");
                toServer.flush();
              } else
              {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("set_game_id false");
              toServer.print("\n");
              toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("set_game_id false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command is "switch_places"
          if (befehl.toString().equals("switch_places"))
          {
            // get the table id from line
            while (((i+2)<(line.length()))&&(line.charAt(i+2)!='>'))
            {
              tisch_id.append(line.charAt(i+2));
              i=i+1;
            }
            // convert the table id from a string into an integer
            tisch_nr = Integer.parseInt(tisch_id.toString());
            tisch_nr=tisch_nr-1;
            // send output
            if ((tisch_nr<r1.anzahl)&&(tisch_nr>=0))
            // now the table id is in their range
            {
              if ((((table)r1.tische.get(tisch_nr)).get_game_id().equals("{}"))
              &&((((table)r1.tische.get(tisch_nr)).get_white_player().equals(nick_name.toString())
              ||(((table)r1.tische.get(tisch_nr)).get_black_player().equals(nick_name.toString())))))
              // the sender is either black or white player and there is no running game
              {
                ((table)r1.tische.get(tisch_nr)).switch_places();
                toServer.print("t ");
                toServer.print(nick_name.toString());
                toServer.print(" ");
                toServer.print("switch_places true");
                toServer.print("\n");
                toServer.flush();
              } else
              {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("switch_places false");
              toServer.print("\n");
              toServer.flush();
              }
            } else
            {
              toServer.print("t ");
              toServer.print(nick_name.toString());
              toServer.print(" ");
              toServer.print("switch_places false");
              toServer.print("\n");
              toServer.flush();
            }
          }

          // if the command has the ability to change something
          // send an update to all participants
          if (!((befehl.toString().equals("show_config_tables")
            ||befehl.toString().equals("get_participants"))
            ||befehl.toString().equals("get_game_ids")))
          {
            LinkedList hilfe=p.get_participant_list();
            for(int j=0;j<p.get_anzahl();j++)
            {
              // show_config_table
              toServer.print("t ");
              toServer.print((String)hilfe.get(j));
              toServer.print(" ");
              toServer.print(r1.show_config_tables());
              toServer.print("\n");
              toServer.flush();
              // participant_list
              toServer.print("t ");
              toServer.print((String)hilfe.get(j));
              toServer.print(" ");
              toServer.print(p.get_participants());
              toServer.print("\n");
              toServer.flush();
              // get_game_ids
              toServer.print("t ");
              toServer.print((String)hilfe.get(j));
              toServer.print(" get_game_ids {");
              for(int k=0;k<(r1.get_anzahl());k++)
              {
                toServer.print(((table)r1.tische.get(k)).get_game_id());
                if (k<(r1.get_anzahl()-1))
                {
                  toServer.print(" ");
                }
              }
              toServer.print("}\n");
              toServer.flush();
            }
          }
        }
      }
      /************************************************************************/
      fromServer.close(); // close the input stream
      toServer.close(); // close the output stream
      s.close(); // close the socket
      /************************************************************************/
    }
    catch(IOException e)
    {
      System.out.println("There is something wrong!!!");
      System.out.println("The exception happened while running the main part.");
    }
    r1.delete_all_tables(); // delete the tables
    mitglieder.write_members(); // save the member list persitent
  }
}
/******************************************************************************/
