#This script is to test the speed for diffent message size and different 
#number of processesors.

#bytes = {1 1000 1000000 1000000000}
#processors = {2 12 13 24 48}

for j in 2 12 13 24 48
do
	for i in 1 1000 1000000 1000000000
	do
	echo "####### $j processors - $i bytes ######"	
	srun -n $j ./ring $i
	echo "#######################################"	
	echo
	done
done