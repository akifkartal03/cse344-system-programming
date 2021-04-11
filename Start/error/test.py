import numpy as np
# input
x = [6, 1, 5, 9, 8, 7,4]  # <- x's
y = [8, 12, 5, 7, 2, 4,6]  # <- y's
# calculating coefficients
M = [[_x**i*(-1)**(i*len(x)) for _x in x] for i in range(len(x))]
C = [np.linalg.det((M+[y]+M)[d:d+len(x)]) for d in range(len(x)+1)]
C = (C / C[0] * (-1)**(len(x)+1) )[1:]
# polynomial lambda-function
poly = lambda _x: sum([C[i] * _x**i for i in range(len(x))])
# output and tests
print("Coefficients:\n", C)
print("TESTING:")
for _x, _y in zip(x, y):
    result = "[OK]" if np.allclose(_y, poly(_x)) else "[ERROR]"
    print(_x, " mapped to: ", poly(_x), " ; expected: ", _y, result)