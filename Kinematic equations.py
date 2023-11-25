"""
Garrett Mason 
Kinematic equation
"""
# improt statements
import math
import numpy as np

#varible definitions

#Define the arm lengths in mm
a = 0.09; # 90mm
b = 0.07; #70mm
c = 0.045; #45mm

#Calculate the theta values based on the intial starting postion
theta2 = math.acos((2 * b - c) / (2 * a))
theta1 = math.pi - theta2

#set the offset for the virtual envirnment
xoffset = 0
yoffset = 0

#Calculate the hindge postions of the arm
Lx = xoffset + a * math.cos(theta1)
Ly = yoffset - a * math.sin(theta1)
Rx = xoffset + c + a * math.cos(theta2)
Ry = yoffset - a * math.sin(theta2)


u1 = (Ry - Ly) / (Lx - Rx)
u2 = (Lx**2 + Ly**2 - Rx**2 - Ry**2) / (2 * (Lx - Rx))
u3 = u1**2 + 1
u4 = 2 * u1 * u2 - 2 * u1 * Lx - 2 * Ly
u5 = u2**2 - 2 * u2 * Lx + Lx**2 + Ly**2 - b**2

kt = 232.65

#Function definitions

""" 
calculate_end_effector_position:
 Calculate the end effector position based on the given parameters from
 the postion encoders on the shaft.

    Parameters:
    - theta1 from postion encoder
    - theta2 from postion encoder

    Returns:
    - Tuple (float, float): A tuple containing the x and y coordinates of the end effector.
"""
def calculate_end_effector_position(theta_1, theta2_1):

    #Define the arm lengths in mm
    a = 0.09; # 90mm
    b = 0.07; #70mm
    c = 0.045; #45mm

    #Calculate the theta values based on the intial starting postion
    theta2 = -(math.acos((2 * b - c) / (2 * a))) + theta2_1
    theta1 = -(math.pi - (math.acos((2 * b - c) / (2 * a)))) + theta_1

    #set the offset for the virtual envirnment
    xoffset = 0
    yoffset = 0

    #Calculate the hindge postions of the arm
    Lx = xoffset + a * math.cos(theta1)
    Ly = yoffset - a * math.sin(theta1)
    Rx = xoffset + c + a * math.cos(theta2)
    Ry = yoffset - a * math.sin(theta2)


    u1 = (Ry - Ly) / (Lx - Rx)
    u2 = (Lx**2 + Ly**2 - Rx**2 - Ry**2) / (2 * (Lx - Rx))
    u3 = u1**2 + 1
    u4 = 2 * u1 * u2 - 2 * u1 * Lx - 2 * Ly
    u5 = u2**2 - 2 * u2 * Lx + Lx**2 + Ly**2 - b**2

    # Calculate the determinant
    determinant = (u4 ** 2) - 4 * u3 * u5
    
    if determinant < 0:
        # Solution is impossible if the determinant is negative
        return None
    
    # Calculate two possible values for y
    y1 = (-u4 + math.sqrt(determinant)) / (2 * u3)
    y2 = (-u4 - math.sqrt(determinant)) / (2 * u3)
    
    # Choose the solution with the lower y-value
    ypos_endeff = min(y1, y2)

    # Calculate x based on y
    xpos_endeff = (ypos_endeff*u1) + u2
    
    return (xpos_endeff,ypos_endeff)

#not correct

def calculate_inverse_kinematics(xpos, ypos):
    # Define the arm lengths in mm
    a = 0.09  # 90mm
    b = 0.07  # 70mm
    c = 0.045  # 45mm

    # Calculate intermediate variables
    Lx = xpos
    Ly = ypos

    # Calculate u1, u2, u3, and u4
    u1 = (Ry - Ly) / (Lx - Rx)
    u2 = (Lx**2 + Ly**2 - Rx**2 - Ry**2) / (2 * (Lx - Rx))
    u3 = u1**2 + 1
    u4 = 2 * u1 * u2 - 2 * u1 * Lx - 2 * Ly

    # Calculate u5
    u5 = u2**2 - 2 * u2 * Lx + Lx**2 + Ly**2 - b**2

    # Calculate the determinant
    determinant = (u4 ** 2) - 4 * u3 * u5

    if determinant < 0:
        # No solution if the determinant is negative
        return None

    # Calculate two possible values for y
    y1 = (-u4 + math.sqrt(determinant)) / (2 * u3)
    y2 = (-u4 - math.sqrt(determinant)) / (2 * u3)

    # Choose the solution with the lower y-value
    ypos_endeff = min(y1, y2)

    # Calculate x based on y
    xpos_endeff = (ypos_endeff * u1) + u2

    # Calculate theta2 based on ypos_endeff
    theta2 = math.acos((2 * b - c) / (2 * a)) + math.acos((Lx - xoffset - c) / a)

    # Calculate theta1 based on ypos_endeff and theta2
    theta1 = ypos_endeff - a * math.sin(theta2)

    return (theta1, theta2)






def jacobian(a, theta1, theta2, theta3, theta4):
    # Define the matrix J with the given expressions
    J = np.array([
        [a * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), a * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))],
        [-a * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), -a * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))]
    ])

    return J

def transpose_jacobian(a, theta1, theta2, theta3, theta4):
    # Define the matrix J_T with the given expressions
    J_T = np.array([
        [a * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), -a * ((np.sin(theta1 - theta3) * np.cos(theta4))/np.sin(theta3 - theta4))],
        [a * ((np.sin(theta4 - theta2) * np.sin(theta3))/np.sin(theta3 - theta4)), -a * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))]
    ])

    return J_T



#main

# Find theta 2&3 using xpos_endeff

#theta3 = math.acos((xpos_endeff-Lx)/b)
#theta4 = math.pi - math.acos((xpos_endeff-Rx)/b) 

x = calculate_end_effector_position(1, 0)
print(x)

xpos,ypos = x
print(calculate_inverse_kinematics(xpos, ypos))
