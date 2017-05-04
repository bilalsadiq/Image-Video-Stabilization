import subprocess
import conf
def main():
    path = conf.VID_PATH.strip(".avi")
    cmd = "avconv -i %s.avi -c:v libx264 -c:a copy %s.mp4" % (path, path)
    subprocess.call(cmd.split())

if __name__=='__main__':
    main()
