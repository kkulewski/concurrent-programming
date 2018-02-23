echo "Provide a number:"
read INPUT

echo $INPUT > data.txt
sleep 1
RESULT=$(<result.txt)
echo -n > result.txt

echo "Server response:"
echo $RESULT
