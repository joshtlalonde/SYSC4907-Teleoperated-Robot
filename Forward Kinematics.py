"""
Garrett Mason 
Kinematic equation
"""
# import statements
import math

#varible definitions
previous_y = 56.5685  # inital starting postion

""" 
calculate_end_effector_position:
 Calculate the end effector position based on the given parameters from
 the postion encoders on the shaft.

    Parameters:
    - theta1 from postion encoder
    - theta2 from postion encoder
    - previous_y from global varible

    Returns:
    - Tuple (float, float): A tuple containing the x and y coordinates of the end effector.
    -Starting postion is (22.5,56.5685)
"""

def forward_kinematics(theta1, theta2, previous_y):
    #Define linkage lengths and distance bewteen motors
    a= 90 # first link
    w = 45 # distance between shafts
    b = 70 # second link
    
    #Set the offsets
    yoff = math.sqrt(a**2 - b**2)
    
    # Calculate R1 and R2 using given equations
    Lx = -(a * math.sin(theta1)) 
    Ly = (a * math.cos(theta1)) 
    Rx = (a * math.sin(theta2)) + w
    Ry = (a * math.cos(theta2))  

    u1 = (Ry - Ly) / (Lx - Rx)
    u2 = (Lx**2 + Ly**2 - Rx**2 - Ry**2) / (2 * (Lx - Rx))
    u3 = u1**2 + 1
    u4 = (2 * u1 * u2) + (-2 * u1 * Lx) - (2 * Ly)
    u5 = (u2**2) - (2 * u2 * Lx) + Lx**2 + Ly**2 - b**2

    # Calculate yP using quadratic equation
    discriminant = abs((u4**2) - (4 * u3 * u5))
  
    yP1 = (-u4 + math.sqrt(discriminant)) / (2 * u3)
    yP2 = (-u4 - math.sqrt(discriminant)) / (2 * u3)

    # Choose the solution that is closer to the previous y value
    ypos_endeff = yP1 if abs(yP1 - previous_y) < abs(yP2 - previous_y) else yP2

    # Calculate x based on y
    xpos_endeff = (ypos_endeff * u1) + u2

    return (xpos_endeff, ypos_endeff)

theta1 = math.pi/4
theta2 = math.pi/4
y = forward_kinematics(theta1, theta2, previous_y)
print(y)