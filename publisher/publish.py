import sysv_ipc
msgSize = 1024
shmSize = 65536

shm = sysv_ipc.SharedMemory(999)
shm.attach()
offset = 0;
counter = 10 ** 9
helper = 1
while True:
        shm.write(str(counter) + ('a' * (msgSize - 10)), offset)
        offset = (offset + msgSize) % shmSize
        #print counter
        counter = counter+1
        helper = helper+1
