
from datetime import datetime
from datetime import timedelta
from datetime import timezone

def get_time():
    SHA_TZ = timezone(timedelta(hours=8),name='Asia/Shanghai')
    utc_now = datetime.utcnow().replace(tzinfo=timezone.utc)
    beijing_now = utc_now.astimezone(SHA_TZ)

    h = beijing_now.strftime('%H')
    m = beijing_now.strftime('%M')
    s = beijing_now.strftime('%S')
    
    return (h,m,s)

def time_test():

    SHA_TZ = timezone(timedelta(hours=8),name='Asia/Shanghai')
    utc_now = datetime.utcnow().replace(tzinfo=timezone.utc)
    beijing_now = utc_now.astimezone(SHA_TZ)
    
    print("Beijing: " + beijing_now.strftime("%H:%M:%S"))
    

