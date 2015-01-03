def name2pos(arr)
   sorted = arr.sort
   puts %Q(static char *names[] = { "#{sorted.join('", "')}" };)
   pos = sorted.collect { |w| arr.index(w) }
   puts %Q(static int name2pos[] = {#{pos.join(", ")}};)
end

d = STDIN.read
d = d.split
name2pos(d)
