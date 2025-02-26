import numpy as np

##########
# PROBLEM 1
##########

# Given values
ambient_strength = 0.2
light_color = np.array([0.9, 0.9, 0.5])  # White light
object_color = np.array([0.7, 0.2, 0.7])  # Some object color (Reddish-orange)

# Compute ambient lighting component
ambient = ambient_strength * light_color

# Compute final color result
result_color = ambient * object_color

# Display the results
print(result_color)

###########
# PROBLEM 2
###########

# Define a new set of non-collinear triangle vertices
a = np.array([-15, 18, -10])  # Vertex A at origin
b = np.array([-15, 18, -12])  # Vertex B along X-axis
c = np.array([-15, 22, -11])  # Vertex C along Y-axis

# Compute the edge vectors
E1 = b - a
E2 = c - a

# Compute the normal using the cross product
N = np.cross(E1, E2)

# Normalize the normal
N_normalized = N / np.linalg.norm(N) if np.linalg.norm(N) != 0 else N

# Display the normal vector
print(f"PROBLEM 2 - normalized result = {N_normalized}")

###########
# PROBLEM 3
###########

# Define the light source position in world space
light_pos = np.array([0.0, 1.0, -10.0])  # Example light position

# Define the fragment's world position (position of the surface point)
frag_pos = np.array([-15.0, 18.0, 11.0])  # Example fragment position

# Define the normal vector at the fragment (should be unit length)
normal = np.array([1.0, 0.0, 0.0])  # Example normal pointing along Z-axis

# Define the object's base color (RGB)
object_color = np.array([0.7, 0.2, 0.7])  # Example reddish-orange object

# Define the light color (RGB)
light_color = np.array([0.9, 0.9, 0.5])  # White light

# Compute the normalized normal vector
norm = normal / np.linalg.norm(normal)

# Compute the normalized light direction vector
light_dir = light_pos - frag_pos  # Compute direction from fragment to light
light_dir = light_dir / np.linalg.norm(light_dir)  # Normalize

# Compute the diffuse component using the dot product
diff = max(np.dot(norm, light_dir), 0.0)  # Ensure non-negative

# Compute the diffuse lighting contribution
diffuse = diff * light_color

# Compute an example ambient lighting contribution (soft base lighting)
ambient_strength = 0.1
ambient = ambient_strength * light_color

# Compute final color by combining ambient and diffuse effects
result_color = (ambient + diffuse) * object_color

# Display the results
result_color
print(f"PROBLEM 3 - result = {result_color}")

###########
# PROBLEM 4
###########

# set view position
view_pos = np.array([10.0, 30.0, -10.0])

# set specular strength
specular_strength = 0.5

# set new fragment position, which is the position of the surface point
frag_pos = np.array([-15.0, 18.0, 11.0])

# normalize the view direction (result should be a unit vector)
view_dir = (view_pos - frag_pos) / np.linalg.norm(view_pos - frag_pos)


# define reflect function 
"""
From the OpenGL Shading Language Specification - https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml:
For a given incident vector I and surface normal N reflect returns the reflection direction calculated as I - 2.0 * dot(N, I) * N.

N should be normalized in order to achieve the desired result.
"""
def reflect(I, N):
    return I - 2.0 * np.dot(N, I) * N
# set the incident vector
I = (light_pos - frag_pos) / np.linalg.norm(light_pos - frag_pos) # ensures light is coming from "correct direction"

# set the normal vector
sphere_center = np.array([0.0, 0.0, 0.0])  # Assuming planet center is at (0,0,0)
N = (frag_pos - sphere_center) / np.linalg.norm(frag_pos - sphere_center)

# compute the reflection vector (the reflection direction)
R = reflect(-I, N)

# calculate the specular component
spec = max(np.dot(view_dir, R), 0.0) ** 32 # this should be a float

specular = specular_strength * spec * light_color

# set object color, which is a bright red - RGB = (1.0, 0.1, 0.1)
object_color = np.array([1.0, 0.1, 0.1])

# compute the final color
result_color = (ambient + diffuse + specular) * object_color
# Display the results (specular component and final color)
print(f"PROBLEM 4 - specular = {specular}")
print(f"PROBLEM 5 - result = {result_color}")


