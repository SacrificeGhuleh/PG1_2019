import plotly.graph_objs as go
import numpy as np
from random import seed
import random

# seed random number generator
seed(1)
coef = 1
normalVector = [0, 2, 0]


def inversesqrt(num):
  return 1 / np.sqrt(num)


def normalize(vec):
  inv = inversesqrt(dot(vec, vec))

  return [vec[0] * inv, vec[1] * inv, vec[2] * inv]


def dot(vec1, vec2):
  return vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2]


def hemisphereSampling(normal):
  randomU = random.uniform(0, 1)
  randomV = random.uniform(0, 1)

  M_2PI = (2.0 * np.pi)

  u = np.cos(M_2PI * randomU) * np.sqrt(randomV * (1.0 - randomV))
  v = np.sin(M_2PI * randomU) * np.sqrt(randomV * (1.0 - randomV))
  w = 1.0 - randomV

  normalized = normalize([u, v, w])

  if (dot(normalize(normal), normalized) < 0):
    normalized[0] *= -1.0
    normalized[1] *= -1.0
    normalized[2] *= -1.0

  return [0, 0, 0, normalized[0], normalized[1], normalized[2]]


if __name__ == "__main__":
  cones = [[], [], [], [], [], []]

  cones[0].append(0)
  cones[1].append(0)
  cones[2].append(0)
  cones[3].append(normalVector[0])
  cones[4].append(normalVector[1])
  cones[5].append(normalVector[2])

  for i in range(100):
    out = hemisphereSampling(normalVector)
    cones[0].append(out[0])
    cones[1].append(out[1])
    cones[2].append(out[2])
    cones[3].append(out[3])
    cones[4].append(out[4])
    cones[5].append(out[5])

  fig = go.Figure(data=go.Cone(
    x=cones[0],
    y=cones[1],
    z=cones[2],
    u=cones[3],
    v=cones[4],
    w=cones[5],
    sizemode="absolute",
    sizeref=1,
    anchor="tail"))

  fig.update_layout(
    scene=dict(domain_x=[0, 1],
               camera_eye=dict(x=-1.57, y=1.36, z=0.58)))

  fig.show()
