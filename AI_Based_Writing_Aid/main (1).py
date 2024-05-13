# All the imports go here
import cv2
import numpy as np
import mediapipe as mp
from collections import deque


# Giving different arrays to handle colour points of different colour
bpoints = [deque(maxlen=2048)]
gpoints = [deque(maxlen=2048)]
rpoints = [deque(maxlen=2048)]


# These indexes will be used to mark the points in particular arrays of specific colour
blue_index = 0
green_index = 0
red_index = 0

#The kernel to be used for dilation purpose
kernel = np.ones((5,5),np.uint8) 

colors = [(0, 0, 0), (255, 0, 0), (0, 0, 255)]
colorIndex = 0

# Here is code for Canvas setup
paintWindow = np.zeros((471,636,3)) + 255
paintWindow = cv2.rectangle(paintWindow, (40,1), (140,65), (255,255,255), -1) #clear
paintWindow = cv2.rectangle(paintWindow, (160,1), (255,65), (0,0,0), -1)     #black
paintWindow = cv2.rectangle(paintWindow, (275,1), (370,65), (255,0,0), -1)  #blue
paintWindow = cv2.rectangle(paintWindow, (390,1), (485,65), (0,0,255), -1)  #red

cv2.putText(paintWindow, "CLEAR", (49, 33), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2, cv2.LINE_AA)
cv2.namedWindow('Paint', cv2.WINDOW_AUTOSIZE)


# initialize mediapipe
mpHands = mp.solutions.hands
hands = mpHands.Hands(max_num_hands=1, min_detection_confidence=0.5)
mpDraw = mp.solutions.drawing_utils


# Initialize the webcam
cap = cv2.VideoCapture(0)
ret = True
while ret:
    # Read each frame from the webcam   
    ret, frame = cap.read()

    x, y, c = frame.shape

    # Flip the frame vertically
    frame = cv2.flip(frame, 1)
    #hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    framergb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    frame = cv2.rectangle(frame, (40,1), (140,65), (255,255,255), -1)
    frame = cv2.rectangle(frame, (160,1), (255,65), (0,0,0), -1)
    frame = cv2.rectangle(frame, (275,1), (370,65), (255,0,0), -1)
    frame = cv2.rectangle(frame, (390,1), (485,65), (0,0,255), -1)

    cv2.putText(frame, "CLEAR", (49, 33), cv2.FONT_HERSHEY_COMPLEX, 0.5, (0, 0, 0), 1, cv2.LINE_AA)

    # Get hand landmark prediction
    result = hands.process(framergb) 

    # post process the result
    if result.multi_hand_landmarks:
        landmarks = []
        for handslms in result.multi_hand_landmarks:
            for lm in handslms.landmark:
                lmx = int(lm.x * 640)
                lmy = int(lm.y * 480)

                landmarks.append([lmx, lmy])

            landmarks_array = np.array(landmarks, dtype=np.float32)
            landmarks_smoothed = cv2.GaussianBlur(landmarks_array, (3,3), 1.5)
            landmarks_smoothed = landmarks_smoothed.tolist()


            # Drawing landmarks on frames
            mpDraw.draw_landmarks(frame, handslms, mpHands.HAND_CONNECTIONS)
        center = (landmarks[8][0],landmarks[8][1])
        thumb = (landmarks[4][0],landmarks[4][1])
        middle = (landmarks[12][0], landmarks[12][1])
        cv2.circle(frame, center, 3, (0,255,0),-1)
        print(center[1]-thumb[1])
        # if (thumb[1]-center[1]<50):
        if (middle[1]-center[1]<30):
            bpoints.append(deque(maxlen=2048))
            blue_index += 1
            gpoints.append(deque(maxlen=2048))
            green_index += 1
            rpoints.append(deque(maxlen=2048))
            red_index += 1

        elif center[1] <= 65:
            if 40 <= center[0] <= 140: # Clear Button
                bpoints = [deque(maxlen=2048)]
                gpoints = [deque(maxlen=2048)]
                rpoints = [deque(maxlen=2048)]
            #giving index to corresponding colors
                blue_index = 0
                green_index = 0
                red_index = 0

                paintWindow[67:,:,:] = 255
            elif 160 <= center[0] <= 255:
                    colorIndex = 0 # Black
            elif 275 <= center[0] <= 370:
                    colorIndex = 1 # Blue
            elif 390 <= center[0] <= 485:
                    colorIndex = 2 # Red
        else :
            if colorIndex == 0:
                bpoints[blue_index].appendleft(center)
            elif colorIndex == 1:
                gpoints[green_index].appendleft(center)
            elif colorIndex == 2:
                rpoints[red_index].appendleft(center)
    # Append the next deques when nothing is detected to avois messing up
    else:
        bpoints.append(deque(maxlen=2048))
        blue_index += 1
        gpoints.append(deque(maxlen=2048))
        green_index += 1
        rpoints.append(deque(maxlen=2048))
        red_index += 1

    # Draw lines of all the colors on the canvas and frame
    points = [bpoints, gpoints, rpoints]
    for i in range(len(points)):
        for j in range(len(points[i])):
            for k in range(1, len(points[i][j])):
                if points[i][j][k - 1] is None or points[i][j][k] is None:
                    continue
                cv2.line(frame, points[i][j][k - 1], points[i][j][k], colors[i], 2)
                cv2.line(paintWindow, points[i][j][k - 1], points[i][j][k], colors[i], 2)

    cv2.imshow("Output", frame)
    cv2.imshow("Canvas", paintWindow)

    if cv2.waitKey(1) == ord('q'):
        break

# release the webcam and destroy all active windows
cap.release()
cv2.destroyAllWindows()