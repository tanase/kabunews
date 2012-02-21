# -*- coding: utf-8 -*-
$:.unshift(File.dirname(__FILE__))
require '../kabudata/quotedatabase'
require 'optparse'
require 'erb'
require 'util'

$url = "http://jp.sugyoku.com"

def render(company, dir, template, miscdir)
  basics = {}
  basics[:symbol] = company['code']
  basics[:name] = company['short_name']
  basics[:exchange] = 't'
  case company['exchange']
  when /大証/
    basics[:exchange] = 'O'
  when /ＪＱ/
  when /ＨＣ/
    basics[:exchange] = 'Q'
  when /札証/
    basics[:exchange] = 'S'
  when /福証/
    basics[:exchange] = 'F'
  end
  
  # 関連銘柄リスト
  correlated = []
  if miscdir[:cor]
    begin
      open(File.join(miscdir[:cor], basics[:symbol])) do |f|
        f.each_line do |line|
          ord, cor, symbol, name = line.split()
          ord = ord.to_i
          cor = cor.to_f
          correlated << {:ord => ord, :cor => cor, :symbol => symbol, :name => name}
          break if ord == 10
        end
      end
    rescue => e
      puts e
    end
  end

  # 予想購入価格平均
  estimated_ave = nil
  estimated_histogram = []
  if miscdir[:pe]
    begin
      open(File.join(miscdir[:pe], symbol)) do |f|
        sum = 0
        f.each_line do |line|
          if !estimated_ave
            estimated_ave = line.to_f
          else
            range, volume = line.split
            volume = volume.to_i
            sum += volume
            estimated_histogram << [range, volume]
          end
        end
        estimated_histogram.each do |a|
          a[1] = a[1].to_f / sum
        end
      end
    rescue => e
    end
  end

  #
  articles = $db.get_articles(basics[:symbol])

  # ERB!
  File::open(dir + "/" + company['code'] + ".html", 'w') do |f|
    erb = ERB.new(File.read(template))
    
    f.puts erb.result(binding)
  end
end

def main()
  dbname = '../kabudata/data.db'
  dir = '/tmp'
  prefix = nil
  miscdir = {}
  template = 'quote.html.erb'
  
  opt = OptionParser.new
  opt.on('--deploydir DIR') {|v| dir = v}
  opt.on('--prefix PREFIX') {|v| prefix = v}
  opt.on('--cordir CORDIR') {|v| miscdir[:cor] = v}
  opt.on('--pedir PEDIR') {|v| miscdir[:pe] = v}
  opt.on('--template TEMPLATE') {|v| template = v}
  opt.parse!(ARGV)

  $db = QuoteDatabase.new(dbname)
  
  companies = $db.get_company_list(prefix)
  companies.each do |company|
    render(company, dir, template, miscdir)
  end
end

main()
