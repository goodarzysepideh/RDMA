import sysv_ipc
msgSize =16
shmSize =1024

shm = sysv_ipc.SharedMemory(5516)
shm.attach()
offset = 0;
data = 1
while True:
        shm.write(str(data), offset)
        offset = (offset + msgSize) % shmSize
        data = data+1

