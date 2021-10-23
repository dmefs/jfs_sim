# log 50% trace
make native zalloc top_buffer vg
JJFS=('native' 'zalloc' 'topbuffer' 'vg')
rm ./bin/*.log
for exe in "${JJFS[@]}"
do
    ./bin/$exe -i instructions/1m_1024_333 -s 2 -l
done
mv ./bin/size.log ./bin/50%size.log
mv ./bin/time.log ./bin/50%time.log
for exe in "${JJFS[@]}"
do
    ./bin/$exe -i instructions/1m_1536_333 -s 2 -l
done
mv ./bin/size.log ./bin/75%size.log
mv ./bin/time.log ./bin/75%time.log
for exe in "${JJFS[@]}"
do
    ./bin/$exe -i instructions/1m_1844_333 -s 2 -l
done
mv ./bin/size.log ./bin/90%size.log
mv ./bin/time.log ./bin/90%time.log

python3 ./test/report.py