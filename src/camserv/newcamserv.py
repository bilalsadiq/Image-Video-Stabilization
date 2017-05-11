from picamera import *
from os import *
from time import *
from io import *
import pycurl
import base64
import json
import multiprocessing
from conf import *
DEBUG = True

def helper_thread(url='127.0.0.1'):
    # req : Request object
    # res : Response buffer / object
    
    cam = PiCamera()
    cam.resolution = CAM_RESOLUTION
    cam.color_effects = CAM_BLACK_AND_WHITE

    try:
        req = pycurl.Curl()
        req.setopt(req.URL, url)
        req.setopt(req.POST, 1)
        header = ['Content-Type: application/json','Accept: application/json']
        req.setopt(req.HTTPHEADER, header)
        stream = BytesIO()
        for fr in cam.capture_continuous(stream, format='jpeg'):
            try:
                stream.truncate()
                stream.seek(0)
            
                payload = '{ "ack" : "?", "img" : "' + base64.b64encode(stream.getvalue()) + '" }'

                res = BytesIO()
                req.setopt(req.POSTFIELDS, payload)
                req.setopt(req.WRITEFUNCTION, res.write)
                req.perform()
                if res:
                    try:
                        response = json.loads(res.getvalue())
                        if response['ack'] == 'bad': # Bad formatting?
                            if DEBUG:
                                print '[-] ERROR: The image POST failed!'
                                DEBUG = False
                    except Exception as e:
                        continue
                else:
                    if DEBUG:
                        print '[-] ERROR: The server could not be reached!'
                        DEBUG = False
            except pycurl.error as e:
                continue
            finally:
                res.close()
    except Exception as e:
        print '[-] ERROR: ' + e.message
    finally:
        cam.close()
 
def main():
    print '[?] Attempting to initiate daemon..'
    try:
        p = multiprocessing.Process( target=helper_thread, args=(HOOK_URL,))
        p.daemon = True
        p.start()
        print '[^] Server creation successful!'
        p.join()
    except Exception as e:
        print '[-] ERROR: The server could not be started!'
        

if __name__=='__main__':
    main()
