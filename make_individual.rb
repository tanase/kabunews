# -*- coding: utf-8 -*-
$:.unshift(File.dirname(__FILE__))
require '../kabudata/quotedatabase'
require 'erb'
require ''

def usage()
  puts <<-EOS
Usage:
  ruby make_individual.rb DEPLOYDIR
EOS
  exit(1)
end

DBNAME = '../kabudata/data.db'

def render(company, dir)
  File::open(dir + "/" + company['code'] + ".html", 'w') do |f|
    symbol = company['code']
    name = company['short_name']
    exchange = 't'
    case company['exchange']
    when /大証/
      exchange = 'O'
    when /ＪＱ/
    when /ＨＣ/
      exchange = 'Q'
    when /札証/
      exchange = 'S'
    when /福証/
      exchange = 'F'
    end
    erb = ERB.new(File.read('quote.html.erb'))
    f.puts erb.result(binding)

    similars = [1,2,3]
  end
end

def main()
  if ARGV.length < 1
    usage()
  end
  dir = ARGV[0]
  db = QuoteDatabase.new(DBNAME)
  companies = db.get_company_list()
  companies.each do |company|
    render(company, dir)
  end
end

main()
