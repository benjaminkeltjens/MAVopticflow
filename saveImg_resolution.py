import os
import natsort
import imageio
from PIL import Image

path = '/home/marina/Dropbox/TUDelft/Subjects/Q3/MAVs/ourProject/codeClassic/jpg2png/'

images = os.listdir(path)
original_images = natsort.natsorted(images)

# Select the two images to compare:

for i in original_images:
    im1_str = path+i    

    # Select images resolution:
    res_x = 28
    res_y = 60
    
    # Resizing the images to lower resolution:
    im = Image.open(im1_str)
    im2 = im.resize((res_x,res_y), Image.ANTIALIAS)

    # Saving the low resolution images to a new path
    saveName = 'lowRes2/' + i   # Select new path here
    im2.save(saveName,dpi=(res_x,res_y))
    