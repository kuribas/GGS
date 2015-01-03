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

setup = [
   %w(OC BDg SC FGs VW CK VSg FC SO CM LH),
   %w(0 FFa 0 0 0 SW 0 0 0 CE 0),
   %w(SP SP SP TFo SP SP SP RR SP SP SP),
   %w(0 0 0 SP 0 0 0 SP 0 0 0) ]

names = $pieces.collect{ |p| p.tag }
setup = setup.collect{ |r| r.reverse! }
setup.each do |row|
   row.each do |p|
      if(names.index(p) == nil)
         printf("{0, 2}, ")
      else
         printf("{%i, 1}, ", names.index(p))
      end
   end
   puts
end

setup = setup.collect{ |r| r.reverse! }.reverse

setup.each do |row|
   row.each do |p|
      if(names.index(p) == nil)
         printf("{0, 2}, ")
      else
         printf("{%i, 0}, ", names.index(p))
      end
   end
   puts
end

