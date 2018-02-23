while :
do
    if [ -s data.txt ]
    then
        RESULT=$(<data.txt)
        echo $(($RESULT*10)) > result.txt
        echo -n > data.txt
    fi
done
