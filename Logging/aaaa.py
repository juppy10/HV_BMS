import subprocess

def ping(host):
    command = ['ping', '-n', '10' , host]
    return subprocess.call(command)

host = 'advancednavigation.com'
print(ping(host))
