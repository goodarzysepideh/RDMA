import sysv_ipc
msgSize = 16 #100*1024
shmSize = 1024 #1024*1024*1024*6.4

shm = sysv_ipc.SharedMemory(9106)
shm.attach()
offset = 0;
data = 1
while True:
        shm.write(str(data), offset)
        offset = (offset + msgSize) % shmSize
        data = data+1

