from io import DEFAULT_BUFFER_SIZE
import pandas as pd
import csv
size_col = ["Read actual size","Read virtual size","Write actual size",
            "Write virtual size","Delete write actual size",
            "Delete write virtual size","Delete read virtual size"]
time_col = ["Access time", "Write time", "Read time", "Read virtual time", "Delete write time", "Delete read time"]
feature_name = ["Native", "Zalloc","VG"]
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

(df_size, df_time) = getdf("bin/size_TPCC_100w.log", "bin/time_TPCC_100w.log")
dsize.append(df_size)
dtime.append(df_time)
(df_size, df_time) = getdf("bin/size_TPCE_100w.log", "bin/time_TPCE_100w.log")
dsize.append(df_size)
dtime.append(df_time)
sheet_name = ["TPCC", "TPCE"]
with pd.ExcelWriter('bin/reportTPCX.xlsx') as writer:
    for i in range(2):
        dsize[i].to_excel(writer, sheet_name="{0}size".format(sheet_name[i]))
        dtime[i].to_excel(writer, sheet_name="{0}time".format(sheet_name[i]))
