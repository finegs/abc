import time
from threading import *

class MyThread(Thread):
	def __init__(self):
		super().__init__()

	def run(self):
		for el in range(100000):
			print('{}=>{}\n'.format(self.getName(), el), end = ' ')
			time.sleep(0.1)

if __name__ == "__main__":
	t1 = MyThread()
	t2 = MyThread()
	t3 = MyThread()
	t1.start()
	t2.start()
	t3.start()

