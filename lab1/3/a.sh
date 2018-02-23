A=$1
if [ $A -le 5 ]; then
	echo "LVL"$A": "$$
	ITER=$((A + 1))
	./a.sh $ITER &
	./a.sh $ITER &
fi
