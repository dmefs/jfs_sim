from os import close


size_str = "Read actual size,Read virtual size,Write actual size,Write virtual size,Delete write actual size,Delete write virtual size,Delete read virtual size"
time_str = "Access time, Write time, Read time, Read virtual time, Delete write time, Delete read time"
fsize = open("./bin/size.log", "w")
ftime = open("./bin/time.log", "w")

fsize.write(size_str)
ftime.write(time_str)
fsize.close()
ftime.close()
