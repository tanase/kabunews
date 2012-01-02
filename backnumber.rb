# -*- coding: utf-8 -*-

def usage
  puts <<-EOS
Usage:
  ruby backnumber.rb COUNTRY LANG
EOS
  exit(1)
end

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

def default_lang(country)
  case country
  when "jp"
    return "jp"
  when "us"
    return "en"
  end
  return "jp"
end

def render_file(file)
  open(file + ".tmpl") do |f|
    f.each_line do |line|
      puts line
    end
  end
end

def render_html(&middle)
  puts <<-EOS
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"> 
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
EOS
  middle.call()
  puts '</html>'
end

def render_head(title, &middle)
  puts '<head>'
  puts '<meta http-equiv="content-type" content="text/html;charset=UTF-8" />'
  puts "<title>#{title}</title>"
  puts '</head>'
  middle.call()
  puts '</head>'
end

def render_body(title, &middle)
  puts '<body>'
  puts "<h1>#{title}</h1>"
  middle.call()
  puts '</body>'
end

usage if ARGV.length < 1
country = ARGV[0]
lang = ARGV[1] || default_lang(country)
dir = country + (lang == default_lang(country) ? "" : "/" + lang)
title = make_title(country, lang)

render_html() do
  render_head(title) do
    render_file("analytics_#{country}#{lang}")
  end
  render_body(title) do
    puts "<ul>"
    Dir::entries(dir).select {|x| x =~ /[0-9]{8}.html/}.sort.reverse.each do |file|
      year  = file[0,4]
      month = file[4,2]
      day   = file[6,2]
      puts "<li><a href=\"#{file}\">#{year}年#{month}月#{day}日</a></li>"
    end
    puts "</ul>"
  end
end
