import sysv_ipc
import datetime
msgSize = 16 #100*1024
shmSize = 1024 #1024*1024*1024*6.4

shm = sysv_ipc.SharedMemory(8206)
shm.attach()
offset = 0;
i = 0
total = 1000
prev_count = -1

a = datetime.datetime.now()
while True:
        mem_data = shm.read(msgSize, offset)
        find_data=mem_data.find('\0')
        data=mem_data[:find_data]
        if (data != "" and int(data) > prev_count):
                prev_count = int(data)
                #print data
                i += 1
        if i > total:
                b = datetime.datetime.now()
                delta = b - a
                print delta.total_seconds() * 1000 # milliseconds
                break
        #print offset
        offset = (offset + msgSize) % shmSize

