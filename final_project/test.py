header ="CSE237A"
def recv(data):
    return data
def socket_receive(connection,remain,remain2,remain3):
    # global remain
    global header
    header_len= len(header)
    mode_size = 2 # bytes
    data_size = 7
    pack_size = 16384
    if(len(remain)>=0):
        print(remain[0:10])

    message = str()
    data   = str(remain)
    print( "remain size:"+str(len(remain)))
    while(True):
        if(len(data)<=(header_len+data_size+mode_size)):
            curr_data =recv(remain2)
            data += curr_data
        if(data[0:header_len]==header):
            data = data[header_len:]
            print("a"+data)
            break
        else:
            if(len(data) > 16000 or len(data) < 100):
                print("data len:"+str(len(data)))
            data = data[1:]
            print(data)

    print("out of loop:"+data)
    mode     = int(data[0:mode_size])
    img_size = int(data[mode_size:data_size+mode_size])
    data     = data[mode_size+data_size:]
    # print(data)
    count=0
    print("resuming")
    while(img_size > 0):
        print("receiving packet: "+str(count)+"  data size: "+str(len(data)))
        if(len(data) > img_size):
            message+=data[0:img_size]
            remain = data[img_size:]
            print("We are here")
            break
        else:
    	    message+=data
            img_size-=len(data)
            if(img_size<=0):
                remain = str()
                break
    	data  = recv(remain3)
        count+=1
    print(remain)
    print(message)
    return remain

# test_string = "asdsCSE23"
# test = "7A000000005A"
# test2 = "BCDEADDITIONAL DATA"
# connection=0
# print(test_string+test+test2)
# socket_receive(connection,test_string,test,test2)
# file=open("log.txt",'r')
# lines= file.readlines()
# print(lines[])
print(5/3)