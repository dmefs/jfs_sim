import pandas as pd
txt_size = 'bin/size.log'
txt_time = 'bin/time.log'
dsize = pd.read_csv(txt_size, sep=',')
dtime = pd.read_csv(txt_time, sep=',')
dsize.to_excel('bin/output.xlsx', index=False, sheet_name='size')
with pd.ExcelWriter('bin/output.xlsx', engine='openpyxl', mode='a') as writer:
    dtime.to_excel(writer, sheet_name='time')
    writer.save()
