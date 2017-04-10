import sqlite3
import time

import random
conn = sqlite3.connect('/home/naveen/Dropbox/Monitor/db.sqlite3')
c = conn.cursor()

#query = "select * from login_nodes"

# for row in c.execute(query):
# 	print(row[2])





import datetime

from random import randint
def strTimeProp(start, end, format, prop):
    """Get a time at a proportion of a range of two formatted times.

    start and end should be strings specifying times formated in the
    given format (strftime-style), giving an interval [start, end].
    prop specifies how a proportion of the interval to be taken after
    start.  The returned time will be in the specified format.
    """

    stime = time.mktime(time.strptime(start, format))
    etime = time.mktime(time.strptime(end, format))

    ptime = stime + prop * (etime - stime)

    return time.strftime(format, time.localtime(ptime))


def randomDate(start, end, prop):
    return strTimeProp(start, end, '%Y-%m-%d %I:%M %p', prop)

iter = 1000;
i =0;
while i < 100:
    strng = str(randomDate("2017-4-5 12:00 AM", "2017-4-5 11:59 PM", random.random()))
    datet = strng.split()[0];
    am = str(strng.split()[2]);
    timet = strng.split()[1];
    if(am == "PM"):
        hh = int(timet.split(":")[0]);
        mm = timet.split(":")[1];
        # print("----------------", timet, am)
        if hh == 12:
            timet = str(hh)+":"+str(mm);
        else:
            timet = str(12 + hh)+":"+str(mm);
    else:
        hh = int(timet.split(":")[0]);
        mm = timet.split(":")[1];

        if hh == 12:
            hh = 12 - int(hh);
            if hh >= 0 and hh < 10:
                hh = "0"+str(hh) 
            timet = str(hh)+":"+str(mm);
        else:
            if hh >= 0 and hh < 10:
                hh = "0"+str(hh) 
            timet = str(hh)+":"+str(mm)
    print (timet)
    temperature = random.uniform(18, 40)
    humidity = randint(20, 60)
    co = randint(0, 40);
    particulate_matter = randint(0, 200)
    node_id_id = randint(1,2)

    n = datetime.datetime.now();
     # you'll get the datetime you already have
    n.strftime("%d/%m/%y %H:%M") 
    n = datet +" "+timet;
    query = "INSERT INTO login_node_data (temperature, humidity, co, particulate_matter, node_id_id, created_date, created_time, timestamp) VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
    c.execute(query, (temperature, humidity, co, particulate_matter, node_id_id, datet, timet, n))
    conn.commit()
    i = i+1;

# startdate=datetime.date(2010,01,01)
# date=startdate+datetime.timedelta(randint(1,365))

# print(date)


# c.execute(query)
# conn.commit()

