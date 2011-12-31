def usage
  $stderr.print "ruby smartcss.rb FILE\n"
  exit 1
end

def collect_commands( target )
  res = ""
  open( target ).each do |line|
    commands = line.scan( /<%([^=].*?)%>/ )
    if commands.length > 0
      res += ";" if res.length > 0
      res += commands.flatten.join( ";" )
    end
  end
  return res
end

def toline target
  res = ""
  open( target ).each do |line|
    res += line.chomp
  end
  return res
end

def doit target
  commands = collect_commands( target )
  line = toline( target )
  # remove comments (overly complicated trying to leave css-hacks)
  while line.gsub!( /\/\*[\*\s]*?[^\*\/\\¥]+[^\*\/\\\s]*[\*\s]*?\*\//, '' )
    next
  end
  # remove spaces
  while line.gsub!( /([,;:{}\/])\s/, '\1' )
    next
  end
  while line.gsub!( /\s([,;:{}\/])/, '\1' )
    next
  end
  # remove blank defs  ex) .section {}
  while line.gsub!( /\}[^{}]*?\{\}/, '}' )
    next
  end
  # remove nested ids  ex) #A #B #C => #C
  while line.gsub!( /#[\d\w]+\s(#[\d\w]+)/, '\1' )
    next
  end
  line.gsub!( /<%[^=].*?%>/, "" )
  while ( line =~ /<%=(.*?)%>/ )
    eval( commands )
    # remove c-style comments inside <% .. %>
    expr = $1
    expr.gsub!( /\/\*.*?\*\//, "" )
    line.sub!( /<%=.*?%>/, eval( expr ).to_s )
  end
  print line
end

target = ""
for arg in ARGV
  target = arg if not arg.start_with?( '-' )
end

usage if target == ""
begin
  doit target
rescue
  $stderr.print "ERROR in #{target} !!\n"
  exit 1
end
