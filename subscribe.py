import sysv_ipc
msgSize =16
shmSize =1024

shm = sysv_ipc.SharedMemory(9472)
shm.attach()
offset = 0;
i = 0
total = 1000
prev_count = -1
while True:
        mem_data = shm.read(msgSize, offset)
        find_data=mem_data.find('\0')
        data=mem_data[:find_data]
        if (data != "" and int(data) > prev_count):
                prev_count = int(data)
                #print data
                i += 1
        if i > total:
                break
        #print offset
        offset = (offset + msgSize) % shmSize
