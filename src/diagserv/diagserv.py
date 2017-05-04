import psutil
from conf import *
from io import *
import json
import pycurl

def main():
#    try:
    url = HOOK_URL
    res = BytesIO()
    req = pycurl.Curl()
    req.setopt(req.URL, url)
    req.setopt(req.POST, 1)
    header = ['Content-Type: application/json', 'Accept: application/json']
    req.setopt(req.HTTPHEADER, header)
    req.setopt(req.WRITEFUNCTION, res.write)
    while True:
        try:
            res.truncate()
            res.seek(0)
            cpu_usage = psutil.cpu_percent()
            mem_stats = psutil.virtual_memory()
            total_mem = mem_stats[0]/1000000 # Megabytes
            available_mem = mem_stats[1]/1000000 # Megabytes
            mem_percent_used = mem_stats[2]
        
            data = { 'cpu_percent' : cpu_usage, 'memory' : { 'total' : total_mem , 'available' : available_mem, 'percent_used' : mem_percent_used }}
            req.setopt(req.POSTFIELDS, json.dumps(data))
            req.perform()
            if res:
                try:
                    response = json.loads(res.getvalue())
                    if response == '':
                        continue
                    if response['ack'] != 'ok':
                        print '[-] ERROR: The server could not parse the update!'
                except KeyError as e:
                    print '[-] ERROR: Malformed JSON response!'
                    continue
                except ValueError as e:
                    print '[-] ERROR: Malformed JSON response!'
        except pycurl.error as e:
            continue
if __name__=='__main__':
    main()
