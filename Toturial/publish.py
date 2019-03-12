import sysv_ipc

context = ""
for i in range(512):
	context = context + "A"
shm = sysv_ipc.SharedMemory(3491)
shm.attach()
shmem.write(3491, context)
