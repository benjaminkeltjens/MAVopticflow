import numpy as np
import os
import time
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import natsort
import cv2
print('need to pip install opencv-contrib-python')
# path = '/home/benjamin/Documents/UNI/3rd Quarter/MAV/AE4317_2019_datasets/cyberzoo_poles/20190121-135009/'
path = '/home/benjamin/Documents/UNI/3rd Quarter/MAV/AE4317_2019_datasets/cyberzoo_poles_panels/20190121-140205/'

images = os.listdir(path)
original_images = natsort.natsorted(images)

N = 220
im1_str = path+original_images[N]
im2_str = path+original_images[N+1]


mult = 1
resolution = (28*mult,60*mult)

im1_todisplay = cv2.imread(im1_str)
im2_todisplay = cv2.imread(im2_str)
im1_disp = cv2.resize(im1_todisplay, resolution, interpolation=1)
im2_disp = cv2.resize(im2_todisplay, resolution, interpolation=1)


im1 = cv2.imread(im1_str, cv2.IMREAD_GRAYSCALE)
im2 = cv2.imread(im2_str, cv2.IMREAD_GRAYSCALE)

im1_res = cv2.resize(im1, resolution, interpolation=1)
im2_res = cv2.resize(im2, resolution, interpolation=1)


#change method here
optflow_object = cv2.optflow.createOptFlow_Farneback()




start = time.time()

flow = optflow_object.calc(im1_res,im2_res,None)

print("time for calculation" + str(time.time()-start), "\n")

U = flow[:,:,0]
V = flow[:,:,1]

im = (0.5*im1_disp.copy().astype(float) + 0.5 * im2_disp.copy().astype(float)) / 255.0
flat_image=(im[:,:,0]+im[:,:,1]+im[:,:,2])/3.

r, c = np.shape(flat_image)
gd = 1
Y, X = np.mgrid[0:r:gd, 0:c:gd]

plt.figure()
plt.imshow(im)
plt.quiver(X, Y, U, V, color='r')
plt.show()

