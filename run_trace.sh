exec_file1="cmr native blockswap blockswap_virtual"
exec_file2="vg vg_reserved vg_history"
trace_file="1GB_10GB.csv 1GB_50GB.csv 10MB_10GB.csv 10MB_50GB.csv"
for trace in $trace_file; do
for exec in $exec_file1; do
    ./bin/$exec -i instructions/$trace -s 100 
    # ./bin/$exec -i instructions/1GB_10GB.csv -s 100 
done
done
for trace in $trace_file; do
    for exec in $exec_file2; do
        for i in 16 32 64 128; do
            ./bin/$exec -i instructions/$trace -s 100 -g $i
        done
    done
done