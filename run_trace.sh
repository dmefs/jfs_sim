#!/bin/bash
exec_file1="cmr native blockswap blockswap_virtual"
exec_file2="vg vg_reserved vg_history"
postmark_trace_file="1GB_10GB.csv 1GB_50GB.csv 1GB_80GB.csv 10MB_10GB.csv 10MB_50GB.csv 10MB_80GB.csv"
trace_file="TPCC.trace.csv TPCE.trace.csv"
vg_size="16 32 64 128"
empty=()

non_vg () {
    local -n traces=$1
    local -n execs=$2
    for trace in $traces; do
        for exec in $execs; do
            ./bin/$exec -i instructions/$trace -s 100 $3
        done
    done
}

vg () {
    local -n traces=$1
    local -n execs=$2
    for trace in $traces; do
        for exec in $execs; do
            for i in 16 32 64 128; do
                ./bin/$exec -i instructions/$trace -s 100 -g $i $3
            done
        done
    done
}

# postmark
non_vg postmark_trace_file exec_file1
vg postmark_trace_file exec_file2
#TPCC, TPCE
non_vg trace_file exec_file1 -c
vg trace_file exec_file2 -c
