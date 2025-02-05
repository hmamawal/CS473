import numpy as np
import matplotlib.pyplot as plt

# problem 1
id_matrix_3x3 = np.array([[1,0,0],[0,1,0],[0,0,1]])

# translate by 10 in x axis
translate_10x = np.array([[1,0,10],[0,1,0],[0,0,1]])

# rotate by 45 degrees clockwise using cosine and sine
theta = np.radians(-45)  # convert degrees to radians
cos_theta = np.cos(theta)
sin_theta = np.sin(theta)

rotate_45_clockwise = np.array([
    [cos_theta, -sin_theta, 0],
    [sin_theta, cos_theta, 0],
    [0, 0, 1]
])

# Calculate the final transformation matrix (rotation dot translation)
final_transformation_matrix = np.dot(rotate_45_clockwise, translate_10x)
print("Final Transformation Matrix (Rotation dot Translation):")
print(final_transformation_matrix)

# Apply translation to the identity matrix
translated_matrix = np.dot(translate_10x, id_matrix_3x3)
print("Translated Matrix:")
print(translated_matrix)

# Apply rotation to the translated matrix
result_matrix = np.dot(rotate_45_clockwise, translated_matrix)
print("Resulting Matrix after Translation and Rotation:")
print(result_matrix)

# Extract the resulting point
resulting_point = result_matrix[:, 2]

# Plot the resulting point
plt.figure()
plt.plot(resulting_point[0], resulting_point[1], 'ro')  # 'ro' means red color, circle marker
plt.xlim(-15, 15)
plt.ylim(-15, 15)
plt.axhline(0, color='black',linewidth=0.5)
plt.axvline(0, color='black',linewidth=0.5)
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)
plt.title('Resulting Point after Translation and Rotation')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.show()