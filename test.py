import os
os.system('ipcrm -a')
os.system("gcc -o server server.c")
os.system("gcc -o client client.c")