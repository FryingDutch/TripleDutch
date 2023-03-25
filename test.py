import requests

lockname = "hi"

link = "https://resourcelock.com/api/"
statusEndpoint = "status?auth=testkey"
getLock = "getlock?auth=testkey&lifetime=200000&lockname=milliond"


for i in range(1, 10000000):
    r = requests.post(link + getLock + str(i))
