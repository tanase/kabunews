FILES="*.css.erb"
dir=`dirname $0`
for f in ${FILES}; do
	if [ -f $f ]; then
		org=${f%.erb}
		echo "-- $f"
		tmp='tmpfile'
		ruby ${dir}/smartcss.rb $f > ${tmp}
		if [ $? -eq 0 ]; then
			if [ "`diff ${org} ${tmp}`" != "" ]; then
				mv ${tmp} ${org}
				echo "written ${org}"
			fi
		else
			echo "SKIPPED $f !!!"
		fi
		rm -f ${tmp}
	fi
done
