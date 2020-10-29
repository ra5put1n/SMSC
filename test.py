import os
os.system('ipcrm -a')
os.system("touch request.txt")
os.system("touch answer.txt")
os.system("gcc -o server server.c")
os.system("gcc -o client client.c")
os.system("gcc -o service1 service1.c")
os.system("gcc -o service2 service2.c")
os.system("gcc -o service3 service3.c")
