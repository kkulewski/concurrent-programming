echo "Provide a number:"
read INPUT
echo $INPUT > data.txt

while [ ! -s result.txt ]; do :
done
RESULT=$(<result.txt)
echo -n > result.txt

echo "Server response:"
echo $RESULT
