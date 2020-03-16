import numpy as np
import os
import time
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import natsort
import pyoptflow
import imageio
from scipy.ndimage.filters import convolve
from PIL import Image

path = '/home/benjamin/Documents/UNI/3rd Quarter/MAV/AE4317_2019_datasets/cyberzoo_poles/20190121-135009/'
# path = '/home/benjamin/Documents/UNI/3rd Quarter/MAV/AE4317_2019_datasets/image_sequence_pure_ver1/'
# path = '/home/benjamin/Documents/UNI/3rd Quarter/MAV/AE4317_2019_datasets/rotated_test/'
images = os.listdir(path)
original_images = natsort.natsorted(images)




# pyoptflow.HornSchunck
N = 34
im1_str = path+original_images[N]
im2_str = path+original_images[N+1]
im1 = imageio.imread(im1_str, as_gray=True)
im2 = imageio.imread(im2_str, as_gray = True)
print('imageio shape ' + str(np.shape(im1)))

resolution = 60
resolution = (resolution, resolution)
resolution_method = Image.ANTIALIAS

pil_image1 = Image.open(im1_str).convert('L')
pil_image1.thumbnail(resolution, resolution_method)
pil_image1 = np.array(pil_image1)
print(np.shape(pil_image1))
# pil_image1=((pil_image1[:,:,0]+pil_image1[:,:,1]+pil_image1[:,:,2])/3)

pil_image2 = Image.open(im2_str).convert('L')
pil_image2.thumbnail(resolution,  resolution_method)
pil_image2 = np.array(pil_image2)
# pil_image2=((pil_image2[:,:,0]+pil_image2[:,:,1]+pil_image2[:,:,2])/3)


print('pil image type: ' +  str(type(pil_image2)) + ' pil image shape: ' + str(np.shape(pil_image2)))



time_overall_st = time.time()
U, V = pyoptflow.HornSchunck(pil_image1, pil_image2, alpha = 5, Niter = 10, verbose=False)
time_overall_end = time.time()  

# for graph
im_gr_1 = mpimg.imread(im1_str)
im_gr_2 = mpimg.imread(im2_str)

pil_image1 = Image.open(im1_str)
pil_image1.thumbnail(resolution, resolution_method)
pil_image1 = np.array(pil_image1)

pil_image2 = Image.open(im2_str)
pil_image2.thumbnail(resolution, resolution_method)
pil_image2 = np.array(pil_image2)

im = (0.5*pil_image1.copy().astype(float) + 0.5 * pil_image2.copy().astype(float)) / 255.0
flat_image=(im[:,:,0]+im[:,:,1]+im[:,:,2])/3.
# flat_image = im

r, c = np.shape(flat_image)
gd = 1
test_slice = flat_image[::gd,::gd]
Y, X = np.mgrid[0:r:gd, 0:c:gd]
plt.figure()
plt.title('first image')
plt.imshow(pil_image1)

plt.figure()
plt.title('second image')
plt.imshow(pil_image2)


plt.figure()
plt.imshow(im)
plt.quiver(X, Y, U, V, color='r')
plt.show()




print('OVERALL TIME: ' + str(time_overall_end - time_overall_st)) 
print(len(original_images))
print('-----U-----')
print(type(U))
print(np.shape(U))
print('\n')

print('-----V------')
print(type(V))
print(np.shape(V))
print('\n')

print('-----image-----')
img = mpimg.imread(path+original_images[40])
print(type(img))
print(np.shape(img))
plt.show()

# plt.ioff() 
# plt.show()
