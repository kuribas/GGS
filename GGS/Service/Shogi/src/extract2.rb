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

$moves.each_with_index do |m, i|
   m.sort!.uniq!
   m.each do |mv|
      printf( "extern %sMove %s;\n", Moves[i].capitalize, mv)
   end
   puts
end
