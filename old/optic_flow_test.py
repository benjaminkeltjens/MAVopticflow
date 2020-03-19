import cv2
import numpy as np
import os
import time
import matplotlib.pyplot as plt
import natsort
import extract_information_flow_field as extr

path = '/home/benjamin/Documents/UNI/3rd Quarter/MAV/AE4317_2019_datasets/cyberzoo_poles/20190121-135009/'
images = os.listdir(path)
original_images = natsort.natsorted(images)

# points_old, points_new, flow_vectors = extr.show_flow(path+original_images[40], path+original_images[41])
# prev_bgr = cv2.imread(path+original_images[1])

# plt.ion()
time_overall_st = time.time()

for i in range(33,len(original_images)):
    current_img = path+original_images[i]
    prev_img = path+original_images[i-1]
    time_total_st = time.time()
    extr.show_flow(prev_img, current_img)
    print('TIME TOTAL: ' + str(time.time() - time_total_st) + '\n'+ '\n')
    plt.pause(0.01)

print('OVERALL TIME: ' + str(time.time() - time_overall_st)) 
print(len(original_images))

#Total flight time of cyberzoo poles is about 55 seconds and total computational time was 6.6 seconds in python (including printing)
#Look into Horn Schunk
plt.ioff() 
# plt.show()
