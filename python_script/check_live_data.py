import sqlite3
conn = sqlite3.connect('/home/pi/Monitor/db.sqlite3')
c= conn.cursor()
data = c.execute('select * from login_live_data')
for each in data:
    print each
