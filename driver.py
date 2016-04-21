import sys
import vturtle  # https://possiblywrong.wordpress.com/2010/11/27/a-robot-simulator-using-vpython/
from AVR_Serial import AvrSerial
import serial


def main(argv):
    mcu = AvrSerial() # create object
    mcu.connect(_port='/dev/ttyACM0',_baud=9600,_byte_size=8,_stopbits=1,_parity=serial.PARITY_NONE) #connect to arduino
    robot = vturtle.Robot(obstacles=vturtle.maze(rows=4, columns=4, cell_size=50))
    while True:
        mcu.send(command='r')  # reads analogue joystick controller
        reading = mcu.receive() # recieves reading
        data_list = reading.strip('\r\n').split(" ") # remove \r\n from string and split with space.
        # example printf("%d %d %d %d %d", X_analog, Y_analog, button_1, button_2, button_3)
        X_analogue = int(data_list[0]) # map data
        Y_analogue = int(data_list[1])
        B = int(data_list[2])
        A = int(data_list[3])
        F = int(data_list[4])
        G = int(data_list[5])
        E = int(data_list[6])
        D = int(data_list[7])
        C = int(data_list[8])
        color_flag = 0 #flag for coloring
        visibility_flag = 0 #flag for robot visibility
        camera_flag= 0 #camera view flag
        # after reading data from analogue move the robot
        #512 is middle position. Need Extensive hardware testing to improve
        #Joystick Controlling (Robot Moving)
        if Y_analogue>512:
            robot.forward(20)
            if robot.sensor(1):
                robot.forward(20)

        elif Y_analogue<512:
            robot.backward(20)
        
        if X_analogue>512:
            robot.right(90)
            if robot.sensor(2):
                robot.right(0)
            
        elif X_analogue<512:
            robot.left(90)
            if(robot.sensor(0):
                robot.left(0)

        #Color Controlling (joystick button c)
        if C==1  && color_flag==0 :
            robot.pen_up()
            robot.color(color.red)
            color_flag = 1
        elif C==1 && robot.color()==color.red:
            robot.color(color.green)
        elif C==1 && color_flag==1:
            robot.pen_down()
            color_flag=0

        #Visibility controlling(Small push button E)
        if E==1 && visibility_flag==0:
            robot.hide()
            visibility_flag=1
        elif E==1 && visibility_flag==1:
            robot.show()
            visibility_flag=0

        #Speed Controlling (Large Push button B)
        if B==1 && robot.speed()<30:
            robot.Speed(30)

        elif B==1 && robot.speed()<100:
            robot.speed(100)
        
        elif B==1 && robot.speed()>100:
            robot.speed(inf)
        
        elif B==1 && robot.speed()==inf:
            robot.speed(30)
        
        #fast_forward controlling (Large push button A)
        if A==1 && robot.fast_forward()<2:
            robot.fast_forward(2)

        elif A==1 && robot.fast_forward()>=2:
            robot.fast_forward(inf)
        }
        elif A==1 && robot.fast_forward()==inf:
            robot.fast_forward(1)
        #Camera View (Large push button F)
        if F==1 && camera_flag ==0:
            robot.save_camera()
            camera_flag = 1;
        elif F==1 && camera_flag ==1:
            robot.restore_camera()    
`           camera_flag =0;
        #look changing  (didnt got any method to change lookt to view)(Large Push Button G)
        if G==1:
            robot.look() 

        #stall
        if robot.stalled():
            break;
            
    pass


if __name__ == '__main__':
    main(sys.argv[1:])