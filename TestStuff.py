import math
import pandas as pd
import numpy as np

def forward_kinematics(theta1, theta2):
    #Define linkage lengths and distance bewteen motors
    a1 = a2 = 0.09
    w = 0.045 # distance between shafts
    b1 = b2 = 0.07
   
    # Calculate R1 and R2 using given equations
    xR1 = a1 * math.cos(theta1)
    yR1 = a1 * math.sin(theta1)
    xR2 = a2 * math.cos(theta2) + w
    yR2 = a2 * math.sin(theta2)

    # Calculate v1 and v2
    v1 = (yR1 - yR2) / (xR2 - xR1)
    v2 = (b1**2 - b2**2 - a1**2 + xR2**2 + yR2**2) / (2 * (xR2 - xR1))

    # Calculate v3, v4, and v5
    v3 = 1 + v1**2
    v4 = 2 * (v1 * v2 - v1 * xR1 - yR1)
    v5 = (a1**2 / (b1**2)) - 2 * v2 * xR1 + ((v2**2) / (v3))

    # Calculate yP using quadratic equation
    discriminant = v4**2 - 4 * v3 * v5
    if discriminant < 0:
        # No real solutions, return None
        return None
    else:
        yP1 = (-v4 + math.sqrt(discriminant)) / (2 * v3)
        yP2 = (-v4 - math.sqrt(discriminant)) / (2 * v3)

        # Calculate xP using v1 and v2
        xP1 = v1 * yP1 + v2
        xP2 = v1 * yP2 + v2

        # Return both solutions (xP1, yP1) and (xP2, yP2)
        return [(xP1, yP1), (xP2, yP2)]
        

def inverse_kinematics(xp, yp, working_mode):

    a1 = a2 = 0.09
    b1 = b2 = 0.07
    w = 0.045 # distance between shafts

    # Calculate c1 and c2 using Equations 19 and 20
    c1 = math.sqrt(xp**2 + yp**2)
    c2 = math.sqrt(c1**2 - (2 * xp * w) + w**2 )

    # Calculate alpha1 and alpha2 using Equations 21 and 22
    alpha1 = math.acos(xp / c1)
    alpha2 = math.acos((-xp + w) / c2)

    # Calculate beta1 and beta2 using Equation 18
    beta1 = math.acos((b1**2 - a1**2 - c1**2) / (-2 * a1 * c1))
    beta2 = math.acos((b2**2 - a2**2 - c2**2) / (-2 * a2 * c2))

    # Define a working mode (for example, 1 or 2) based on your table

    if working_mode == 1:
        # Mode ++: Combine alpha and beta directly
        theta1 = alpha1 + beta1
        theta2 = alpha2 + beta2
    elif working_mode == 2:
        # Mode +−: Combine alpha1 and beta1, and alpha2 and -beta2
        theta1 = alpha1 + beta1
        theta2 = alpha2 - beta2
    elif working_mode == 3:
        # Mode -+: Combine alpha1 and -beta1, and alpha2 and beta2
        theta1 = alpha1 - beta1
        theta2 = alpha2 + beta2
    elif working_mode == 4:
        # Mode --: Combine alpha and beta with negative signs
        theta1 = alpha1 - beta1
        theta2 = alpha2 - beta2
    else:
        # Handle other working modes
        raise ValueError("Invalid working mode")

    # Convert angles to degrees
    theta1_degrees = math.degrees(theta1)
    theta2_degrees = math.degrees(theta2)

    return theta1_degrees, theta2_degrees



#Main

# Load XY points from a CSV file using Pandas
csv_file = 'xy_points.csv'  # Replace with your CSV file path
df = pd.read_csv(csv_file)

# Extract x and y columns from the DataFrame
x_points = df['x'].values
y_points = df['y'].values

# Define your characteristic link lengths and working mode
a1 = 3.0
a2 = 2.0
w = 1.0
working_mode = 1  # Replace with the desired working mode (1, 2, 3, or 4)

# Initialize lists to store motor angles
theta1_list = []
theta2_list = []

# Iterate through the XY points and solve inverse kinematics for each point
for xp, yp in zip(x_points, y_points):
    theta1, theta2 = inverse_kinematics(xp, yp, a1, a2, w, working_mode)
    theta1_list.append(theta1)
    theta2_list.append(theta2)

# Convert the motor angles lists to NumPy arrays
theta1_array = np.array(theta1_list)
theta2_array = np.array(theta2_list)
