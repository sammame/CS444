#!/usr/bin/python
import os

# Create, read to, write to, and delete files ....
#1
file = open("testfile.txt","w") 
file.write("Hello World. ")
file.write("This is a test file.")
file.close()

#2
file = open("testfile2.txt","w") 
file.write("Hello World again. ")
file.write("This is another test file.")
file.close()

#3
file = open("testfile.txt","r") 
print file.read() 
file.close


#4
file = open("testfile3.txt","w") 
file.write("Hello World for the third time. ")
file.write("I AM THE LAST TEST FILE.")
file.close()

#4
file = open("testfile3.txt","r") 
print file.read() 
file.close

#5
file = open("testfile2.txt","r") 
print file.read() 
file.close


#Clean up all files created 
os.remove("testfile.txt")
os.remove("testfile2.txt")
os.remove("testfile3.txt")
