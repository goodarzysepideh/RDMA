import sysv_ipc
import random

max_key = 10000
shmSize = 65536

shmid = random.randint(0, max_key)
try:
	sysv_ipc.SharedMemory(999, sysv_ipc.IPC_CREAT, 0666, shmSize, ' ') 
except Exception as e:
	print('Failed to upload to ftp: '+ str(e))
	
	
print(shmid)
