from sympy import symbols, cos, sin, pi, Matrix, simplify

# Define the rotation angle (45 degrees counterclockwise)
theta_45_ccw = pi / 4  # Counterclockwise rotation

# Step 1: Translation to origin (move (2, 3) to (0, 0))
translate_to_origin = Matrix([
    [1, 0, -2],
    [0, 1, -3],
    [0, 0, 1]
])

# Step 2: Rotation matrix (45 degrees counterclockwise)
rotation_matrix_45 = Matrix([
    [cos(theta_45_ccw), -sin(theta_45_ccw), 0],
    [sin(theta_45_ccw), cos(theta_45_ccw), 0],
    [0, 0, 1]
])

# Step 3: Translation back to (2, 3)
translate_back = Matrix([
    [1, 0, 2],
    [0, 1, 3],
    [0, 0, 1]
])

# Combined transformation matrix
final_transformation_matrix = simplify(translate_back * rotation_matrix_45 * translate_to_origin)

# Display the matrices
print("Translation to Origin Matrix:")
print(translate_to_origin)
print("\nRotation Matrix (45° Counterclockwise):")
print(rotation_matrix_45)
print("\nTranslation Back Matrix:")
print(translate_back)
print("\nFinal Combined Transformation Matrix:")
print(final_transformation_matrix)
