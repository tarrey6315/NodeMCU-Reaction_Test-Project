
# script  = "l1s1,l3s1,r1"
# script  = "l1s2,l3s2,l2s2,r1"
# userlog = "" \
#           "t3102l1s3/4/2,t4201l3s3/4/2,t4521l3s4/4/5,t7521l5s4/4/5,t8521l5s4/4/5," \
#           ""

# script  = "l1s1,l3s1,r1"
# userlog = "t3102l1s3/4/2,t4201l3s3/4/2,t4521l5s4/4/5,"
script = ""
userlog = ""
test = "22"
timer = 3
repeatNum = 0
userlog_format = []
user_ans = []
LED_list = []
stop_list_t = []
stop_list = []
ans_list_t = []
ans_list = []

if __name__ == '__main__':
    # getfile(script,userlog)
    f = open('ans.csv')
    str = f.readlines()
    script = str[-2]
    userlog = str[-1]
    print(f'script:{script}')
    print(f'userlog:{userlog}')
    f.close()

    userlog_list = userlog.split(',')[0:-1]
    ans_list_t = [i[0:2] for i in script.split(',')[0:-1]]
    script_list = script.split(',')

    for sub in script_list:
        if(sub[0] == 'r') : repeatNum = int(sub[1])
        else:
            stop_list_t.append(int(sub[3:]))

    # stop_list_t = stop_list
    # ans_list_t = ans_list
    while(repeatNum > 0):
        for stop in stop_list_t:
            stop_list.append(stop)
        for ans in ans_list_t:
            ans_list.append(ans)
        repeatNum -= 1

    for sub in userlog_list:
        l_pos = sub.find('L')
        str = "l" + sub[l_pos+1] + "s" + sub[1:l_pos-3]
        userlog_format.append(str)
    # index = 0

    for sub,stoptime in zip(userlog_format,stop_list ):
        usertime = int(sub[3:])
        # stoptime = int(stop_list[index])

        if(timer <= usertime & usertime < timer + stoptime):
            user_ans.append(sub[0:2])
            timer += stoptime

        elif(usertime > timer+ 3*stoptime):
            continue
        elif(usertime >= timer+stoptime):
            timer += stoptime
            while(usertime >= timer):
                user_ans.append("  ")
                timer += stoptime

            user_ans.append(sub[0:2])

    while(len(user_ans) != len(ans_list)):
        user_ans.append("  ")

    correct = 0
    fail = 0
    for user, ans in zip(user_ans,ans_list):
        if(user == ""):
            fail += 1
            continue
        elif(user[1] == ans[1]):
            correct += 1
        else: fail +=1



    # print(f'LED_list:{LED_list}')
    print(f'stop_list:{stop_list}')
    print(f'repeatNum:{repeatNum}')
    print(f'userlog_format:{userlog_format}')
    print(f'ans_list:{ans_list}')
    print(f'user_ans:{user_ans}')
    print(f'userlog_list:{userlog_list}')
    print(f'correct:{correct}, fail:{fail}')