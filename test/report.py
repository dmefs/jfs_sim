from io import DEFAULT_BUFFER_SIZE
import pandas as pd
import csv
size_col = ["Read actual size","Read virtual size","Write actual size",
            "Write virtual size","Delete write actual size",
            "Delete write virtual size","Delete read virtual size"]
time_col = ["Access time", "Write time", "Read time", "Read virtual time", "Delete write time", "Delete read time"]
feature_name = ["Native", "Zalloc", "TopBuffer", "VG"]
txt_size = 'bin/size.log'
txt_time = 'bin/time.log'
dsize = []
dtime = []
def getdf(sizeName, timeName):
    size_list = []
    time_list = []
    with open(sizeName, 'r') as fd:
        reader = csv.reader(fd)
        for row in reader:
            size_list.append(row)
    with open(timeName, 'r') as fd:
        reader = csv.reader(fd)
        for row in reader:
            time_list.append(row)
    dsize = pd.DataFrame(size_list, index=feature_name, columns=size_col)
    dtime = pd.DataFrame(time_list, index=feature_name, columns=time_col)
    return (dsize, dtime)

(df_size, df_time) = getdf("bin/50%size.log", "bin/50%time.log")
dsize.append(df_size)
dtime.append(df_time)
(df_size, df_time) = getdf("bin/75%size.log", "bin/75%time.log")
dsize.append(df_size)
dtime.append(df_time)
(df_size, df_time) = getdf("bin/90%size.log", "bin/90%time.log")
dsize.append(df_size)
dtime.append(df_time)
percent = [50, 75, 90]
with pd.ExcelWriter('bin/output.xlsx') as writer:
    for i in range(3):
        dsize[i].to_excel(writer, sheet_name="{0}%size".format(percent[i]))
        dtime[i].to_excel(writer, sheet_name="{0}%time".format(percent[i]))
