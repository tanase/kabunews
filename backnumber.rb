# -*- coding: utf-8 -*-
$:.unshift(File.dirname(__FILE__))
require 'kabunews'
require '../kabudata/quotedatabase'
require 'erb'
require 'optparse'

def make_title(country, lang)
  case country
  when "jp"
    if lang == "jp"
      return "株色新聞 バックナンバー"
    else
      return "Japanese Stock News Back Issues"
    end
  when "us"
    if lang == "jp"
      return "アメリカ株新聞 バックナンバー"
    else
      return "US Stock News Back Issues"
    end
  end
  return "back issue"
end

def read_tmpl(name)
  str = ""
  open(name + ".tmpl") do |f|
    f.each_line do |line|
      str += line
    end
  end
  return str
end

def main
  opt = OptionParser.new
  country = "jp"
  lang = "jp"
  deploydir = nil
  dir = nil
  
  opt.on('-c', '--country COUNTRY') {|v| country = v}
  opt.on('-l', '--lang LANG') {|v| lang = v}
  opt.on('--deploydir DIR') {|v| deploydir = v}
  opt.on('--dir DIR') {|v| dir = v}
  opt.version = 1.0
  opt.parse!(ARGV)
  
  if !dir
    puts opt
    exit(1)
  end

  title = make_title(country, lang)

  f = deploydir ? open(File.join(deploydir, "back.html"), 'w') : STDOUT
  erb = ERB.new(File.read("backnumber.html.erb"))
  files = Dir::entries(dir).select {|x| x =~ /[0-9]{8}.html/}.sort.reverse
  f.puts erb.result(binding)
end

main()
