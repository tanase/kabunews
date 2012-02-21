def date2year(date)
  return date / 10000
end

def date2month(date)
  return date % 10000 / 100
end

def date2day(date)
  return date % 100
end

def parse_date(date)
  return date2year(date), date2month(date), date2day(date)
end
