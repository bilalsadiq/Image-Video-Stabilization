import subprocess
from conf import *

def main():
    com = ['/usr/bin/python', '%snewcamserv.py' % (PATH_TO_EXECUTABLE)]
    subprocess.Popen(com)

if __name__=='__main__':
    main()
