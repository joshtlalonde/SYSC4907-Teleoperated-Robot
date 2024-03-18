import math
import numpy as np

ARM_LENGTH_A = 90 # First Link Length (mm)
ARM_LENGTH_B = 70 # Second Link Length (mm)
ARM_WIDTH = 45 # Distance Between Shafts (mm)
ARM_LENGTH_A_Meters = 0.09 # First Link Length (m)

def get_transpose_jacobian(theta1, theta2, current_y):
    """
    Transpose Jacobian Matrix Calculator:
    Calculates the transposed Jacobian matrix for a two-joint robotic arm based on given joint angles and end effector position.

    Parameters:
    - theta1: Shaft angle from the first position encoder.
    - theta2: Shaft angle from the second position encoder.
    - current_y: The current y-position of the end effector.

    Returns:
    - numpy.ndarray: A 2x2 matrix representing the transposed Jacobian matrix.

    This function computes the transposed Jacobian matrix (J_T) for a two-joint robotic arm, which relates changes in end effector 
    position to changes in joint angles. The input parameters include the current joint angles (theta1 and theta2) and the 
    current y-position of the end effector.

    The matrix J_T is constructed using trigonometric expressions derived from the 5-linkage bars arm's geometry. The resulting transposed 
    Jacobian matrix is then returned as a numpy array.

    Note: Ensure the input joint angles and end effector position are compatible for accurate calculations.
    """

    # Define the matrix J_T with the given expressions
    Ly =(ARM_LENGTH_A * math.sin(theta1))
    print("Height of the Left joint is: ", Ly)
    Ry = (ARM_LENGTH_A * math.sin(theta2))
    print("Height of the Right joint is: ",Ry)
    theta3 = math.pi/2 - math.acos((current_y-Ly)/ARM_LENGTH_B)
    if theta3 > -0.003 and theta3<0:
        theta3 = 0
    theta4 = math.pi/2 + math.acos((current_y-Ry)/ARM_LENGTH_B)
    print("The value for theta3 is: ", theta3)
    print("The value for theta4 is: ",theta4)
    J_T = np.array([
        [ARM_LENGTH_A_Meters * ((np.sin(theta1 - theta3) * np.sin(theta4))/np.sin(theta3 - theta4)), -ARM_LENGTH_A_Meters * ((np.sin(theta1 - theta3) * np.cos(theta4))/np.sin(theta3 - theta4))],
        [ARM_LENGTH_A_Meters * ((np.sin(theta4 - theta2) * np.sin(theta3))/np.sin(theta3 - theta4)), -ARM_LENGTH_A_Meters * ((np.sin(theta4 - theta2) * np.cos(theta3))/np.sin(theta3 - theta4))]
    ])

    return J_T

def test_transpose_jacobian():
    """
    Test function for the transpose_jacobian function.

    This function tests the transpose_jacobian function with different sets of input parameters. 
    It prints the output of the transpose_jacobian function for each test case.

    Test cases:
    - Test case 1: Same angles, different y
    - Test case 2: Different angles, same y
    - Test case 3: Different angles, different y
    - Test case 4: Test with high y value

    Note: This function does not return any value. It prints the output of each test case.
    """
    # Test case 1: Same angles, different y
    result = get_transpose_jacobian(2.1268, 2.1268, 76.3)
    print(f"Test case 1: {result}")
    
    # Test case 2: Different angles, same y
    result = get_transpose_jacobian(2.1268, 1.0148, 130)
    print(f"Test case 2: {result}")
    
    # Test case 3: Different angles, different y
    result = get_transpose_jacobian(1.8, 1.6, 130)
    print(f"Test case 3: {result}")
    
    # Test case 4: Test with high y value
    result = get_transpose_jacobian(1.8, 1.6,150)
    print(f"Test case 4: {result}")

test_transpose_jacobian()

def get_armature_current(theta1, theta2, current_y):
    """
    This function calculates the current for the left and right motors of an armature.

    Parameters:
    theta1 (float): The first angle parameter (in radians).
    theta2 (float): The second angle parameter (in radians).
    current_y (float): The current y-coordinate.

    Returns:
    dict: A dictionary with the current for the left and right motors.
    """
def get_armature_current(theta1, theta2, current_y):
    KT = np.array([1/232.65,1/232.65 ]) #Motor velocity constant
    JT = get_transpose_jacobian(theta1, theta2, current_y)
    F = np.array([0.01,0.01]) # from get_force(Fx,Fy)
    torque = np.dot(JT, F) # Matrix multiplication 
    Ial, Iar = np.multiply(torque,KT)
    return {'left motor current': Ial, 'right motor current': Iar}

def test_get_armature_current():
    """
    Test function for the get_armature_current function.

    This function tests the get_armature_current function with different sets of input parameters. 
    It prints the output of the get_armature_current function for each test case.

    Test cases:
    - Test case 1: Same angles, different y
    - Test case 2: Different angles, same y
    - Test case 3: Different angles, different y
    - Test case 4: Test with high y value

    Note: This function does not return any value. It prints the output of each test case.
    """
    # Test case 1: Same angles, different y
    result = get_armature_current(2.1268, 2.1268, 76.3)
    print(f"Test case 1: {result}")
    
    # Test case 2: Different angles, same y
    result = get_armature_current(2.1268, 1.0148, 130)
    print(f"Test case 2: {result}")
    
    # Test case 3: Different angles, different y
    result = get_armature_current(1.8, 1.6, 130)
    print(f"Test case 3: {result}")
    
    # Test case 4: Test with high y value
    result = get_armature_current(1.8, 1.6,150)
    print(f"Test case 4: {result}")

test_get_armature_current()
