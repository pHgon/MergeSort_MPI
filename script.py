import os

source = "MergeSort_MPI.cpp"

n_elements = ['10', '100', '1000', '10000', '100000', '1000000', '10000000', '100000000', '1000000000']
n_procs    = ['1', '2', '3', '4', '5', '6', '7', '8']

cmd = "mpic++ -std=c++11 " + source + " -o prog"
os.system(cmd)

cmd = "echo \"n_elem proc copy_to copy_back total_copy process_time total_time exec_time\n\" > output.csv"
os.system(cmd)

for e in n_elements:
	for p in n_procs:
		cmd = "mpiexec -np " + p + " ./prog " + e + " >> output.csv"
		os.system(cmd)

cmd = "rm -R prog"
os.system(cmd)