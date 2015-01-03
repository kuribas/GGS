# extract all the "moves" from the variant files

require "strscan"

Moves = %w{short long jump num knight}

$moves = Array.new(Moves.size) { [] }
Dir["var*[CH]"].each do |f|
   data = IO.read(f)
   Moves.each_with_index do |m, i|
      pat = Regexp.new("#{m}move[0-9_]+")
      s = StringScanner.new(data)
      while s.scan_until(pat)
         $moves[i] << s[0]
      end
   end
end

$moves.each{ |m| m.sort!.uniq! }

File.open("moves_def.inc", "w") do |outfile|
   $moves.each_with_index do |m, i|
      m.each do |mv|
         arg1, arg2 = /_(\d+)(_(\d+))?/.match(mv).values_at(1, 3)
         if arg2
            outfile.printf("%sMove %s(0x%s, 0x%s);\n", Moves[i].capitalize, mv, arg1, arg2)
         else
            outfile.printf("%sMove %s(0x%s);\n", Moves[i].capitalize, mv, arg1)
         end
      end
      outfile.puts
   end
end

File.open("moves_decl.inc", "w") do |outfile|
   $moves.each_with_index do |m, i|
      m.each do |mv|
         outfile.printf( "extern %sMove %s;\n", Moves[i].capitalize, mv)
      end
      outfile.puts
   end
end
   
