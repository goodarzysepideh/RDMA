import sysv_ipc

context = ""
for i in range(1024):
	context = context + "A"
shm = sysv_ipc.SharedMemory(9961)
shm.attach()
shm.write(context, 0)
