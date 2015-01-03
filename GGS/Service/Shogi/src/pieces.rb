d = IO.read("wa.dat")

Piece = Struct.new(:name, :descr, :tag, :prom_to, :prom_from, :moves)
Move = Struct.new(:name, :args)

$pieces = d.select{ |l| l !~ /^\s+$/ }.collect do |l|
   p = l.split(":")
   if p.size != 6
      $stderr << "wrong length: " << p.size << p.inspect
      exit
   end
   p = Piece.new(*p)
   p.moves = p.moves.split(";").collect do |m|
      m = m.split
      Move.new(m[0], m[1..-1])
   end
   p
end

def ptr(string)
   (string == "" ? "NULL" : string)
end

def name2pos(arr)
   sorted = arr.sort
   puts %Q(char *names[] = { "#{sorted.join('", "')}" };)
   puts
   pos = sorted.collect { |w| arr.index(w) }
   puts %Q(int name2pos[] = {#{pos.join(", ")}};)
end

$pieces.each_with_index do |p, i|
   moves = p.moves.collect { |m| "&#{m.name}move_#{m.args.join('_')}"}.join(", ");
   puts("MoveWithCheck *#{p.name}moves[] = {#{moves}, NULL};")
   printf("Piece %s = { \"%s\", &%s, %s, %i, 0, 0, %s};\n\n", p.name, p.tag,
        ptr(p.prom_to), ptr(p.prom_from), i, p.name + "moves")
end

name2pos($pieces.collect{ |p| p.tag })

puts
puts "const Piece *pieces[] = {\n#{$pieces.collect{ |p| '&' + p.name }.join(', ')}};\n"

