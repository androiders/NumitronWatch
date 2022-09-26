import sys, serial, getopt
from datetime import datetime

def setClock():
    print("setting clock")
    ser = serial.Serial('/dev/ttyUSB0', timeout=10)  # open serial port
    ser.flushOutput()
    print(ser.name)         # check which port was really used
    now = datetime.now()
    current_time = now.strftime("%H:%M:%S")
    t = 'set' + current_time + "\n"
#    ser.write(bytes(t,encoding='utf-8'))     # write a string
    ser.write(b'set ')
    ser.write(bytes(current_time,encoding='utf-8'))     # write a string
    line = ser.readline()
    print(line.decode())
    ser.close()
    f = open("timefile","w")
    f.write(current_time)
    f.close()
    
    return

def checkClock():
    print("checking clock")
    ser = serial.Serial('/dev/ttyUSB0')  # open serial port
    print(ser.name)         # check which port was really used
    localnow = datetime.now()
    ser.write(b'get')     # write a string
    line = ser.readline()
    ld = line.decode()
    ld.strip()
    clocknow = datetime.strptime(ld,"%H:%M:%S\r\n")
    
    hdiff = localnow.hour - clocknow.hour
    mdiff = localnow.minute - clocknow.minute
    sdiff = localnow.second - clocknow.second
    print("")
    print("difference in time is:")
    print("Hours: " , hdiff)
    print("Minutes: " , mdiff)
    print("seconds: " , sdiff)
    
    f = open("timefile","r")
    starttime = f.readline()
    startnow = datetime.strptime(starttime,"%H:%M:%S")

    diff = clocknow - startnow

    hdiff = clocknow.hour - startnow.hour
    mdiff = clocknow.minute - startnow.minute
    sdiff = clocknow.second - startnow.second

    print("")
    print("Time since last set " + str(diff))
      
    return

def readClock():
    print("reading clock")
    ser = serial.Serial('/dev/ttyUSB0')  # open serial port
    print(ser.name)         # check which port was really used
    ser.write(b'get')     # write a string
    line = ser.readline()
    print(line.decode())
    ser.close()
    return


def main():
    
    try:
      opts, args = getopt.getopt(sys.argv[1:],"scr",["set","check","read"])
    except getopt.GetoptError as err:
        print(err)
    for opt, arg in opts:
        if opt in ('-s','--set'):
            setClock()
        elif opt in ("-c", "--check"):
            checkClock()
        elif opt in ("-r", "--read"):
            readClock()
    
    return 0

if '__main__' == __name__:
    sys.exit(main())
