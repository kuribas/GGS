regexp = /Piece (\w+)\s*=\s*\{.*?,\s+(\{.*?\})\};/
d = STDIN.read
d.gsub!(regexp) do |s|
   mvs = "MoveWithCheck *#{$1}moves[] = #{$2};"
   mvs + "\n" + s.gsub(/,\s+\{.*?\}/, ", &#{$1}moves") + "\n"
end
puts d
