import cv2
import face_recognition as fr
import time
import serial
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from datetime import datetime

arduino = serial.Serial(port = "COM11", baudrate = 9600)
videoCapture = cv2.VideoCapture(0) #Default camera is selected
ownerImage = fr.load_image_file("C:\\Users\\merty\\Desktop\\Project 2\\img\\mert.jpg") #Loads the authorized face image into the program
ownerEncode = [fr.face_encodings(ownerImage)[0]] 
print("Program Started")

while True:
#-------------------------------------------- EMAIL SENDER --------------------------------------------------#
    face_rec = 0
    smtp_port = 587
    smtp_server = "smtp.gmail.com"
    email_from = "mertirlanda@gmail.com"
    email_to = "mertyuceyurt58@gmail.com"
    pswd = "kelugbwjnkslaxme"
    sent_before = 0
    raw_message = str(arduino.readline()) # Takes the message sent by Arduino
    message = raw_message[raw_message.index("'")+1 : raw_message.index("r")-1] # Takes out the important part
    print(message)
    
    now = datetime.now()
    date_time = now.strftime("%d/%m/%Y %H:%M:%S")
    
    if message == "KITCHEN": # If arduino sends "KITCHEN" to Python
        subject = "Motion Detected: KITCHEN"
        body = f"""
        Motion detected in the kitchen!
        {date_time}
        """
        face_rec = 1 # Means that Face Recognition system must be initialized.
    
    elif message == "BEDROOM":
        subject = "Motion Detected: BATHROOM"
        body = f"""
        Motion detected in the bathroom!
        {date_time}
        """
        message = ""
        face_rec = 1

    elif message == "ENTRY_EXIT":
        subject = "Motion Detected: ENTRY EXIT ZONE"
        body = f"""
        Motion detected in the entry exit zone!
        {date_time}
        """
        message = ""
        face_rec = 1    

    msg = MIMEMultipart()
    msg["From"] = email_from
    msg["To"] = email_to
    msg["Subject"] = subject
    msg.attach(MIMEText(body, "plain"))
    
    text = msg.as_string() 
    try:
        print("Connecting to server...")
        TIE_server = smtplib.SMTP(smtp_server, smtp_port)
        TIE_server.starttls()
        TIE_server.login(email_from, pswd)
        print("Connected to SMTP server")
    
        TIE_server.sendmail(email_from, email_to, text)
        print("Email sent")

    except Exception as e:
        print(e)

#------------------------------------------- FACE RECOGNITION --------------------------------------------------#
    if face_rec == 1: print("Face Recognition System Initialized")
    while face_rec == 1:
        ownerImage = fr.load_image_file("C:\\Users\\merty\\Desktop\\Project 2\\img\\mert.jpg") #Loads the authorized face image into the program
        ownerEncode = [fr.face_encodings(ownerImage)[0]]
    
        res, frame = videoCapture.read()
        time.sleep(1) #Because my computer cannot process 24 frames per second. If yours is powerful enough, you can skip this part.
        RGBFrame = frame[:, :, ::-1] #Converting BGR to RGB (that's what face_recognition wants)
        frameEncodings = fr.face_encodings(RGBFrame)
        lastCheck = 0
        for frameEncode in frameEncodings:
            matchCheck = fr.compare_faces(ownerEncode,frameEncode)
            if matchCheck[0]:
                lastCheck = 1
                break
        if lastCheck:
            arduino.write(bytes("1","utf-8")) #Arduino recieves 1 if face has been found
            print("Face Recognized!")
            face_rec = 0
        else:
            arduino.write(bytes("0","utf-8")) #If not, arduino recieves 0
            print("No Registered Face Recognized")     

#------------------------------------------------------------------------------------------------------------#            