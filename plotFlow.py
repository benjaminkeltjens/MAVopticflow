import numpy as np
import os
import errno
from PIL import Image
import io
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import imageio

# Do not touch code up to line 55
# read_flow is a function that reads the flow.flo file generated from the executable horn_schunck_classic

TAG_FLOAT = 202021.25
flags = {
    'debug': False
}

def read_flow(path):
    if not isinstance(path, io.BufferedReader):
        if not isinstance(path, str):
            raise AssertionError(
                "Input [{p}] is not a string".format(p=path))
        if not os.path.isfile(path):
            raise AssertionError(
                "Path [{p}] does not exist".format(p=path))
        if not path.split('.')[-1] == 'flo':
            raise AssertionError(
                "File extension [flo] required, [{f}] given".format(f=path.split('.')[-1]))

        flo = open(path, 'rb')
    else:
        flo = path

    tag = np.frombuffer(flo.read(4), np.float32, count=1)[0]
    if not TAG_FLOAT == tag:
        raise AssertionError("Wrong Tag [{t}]".format(t=tag))

    width = np.frombuffer(flo.read(4), np.int32, count=1)[0]
    if not (width > 0 and width < 100000):
        raise AssertionError("Illegal width [{w}]".format(w=width))

    height = np.frombuffer(flo.read(4), np.int32, count=1)[0]
    if not (width > 0 and width < 100000):
        raise AssertionError("Illegal height [{h}]".format(h=height))

    nbands = 2
    tmp = np.frombuffer(flo.read(nbands * width * height * 4),
                        np.float32, count=nbands * width * height)
    flow = np.resize(tmp, (int(height), int(width), int(nbands)))
    flo.close()

    return flow

# Transforming variable flow into the actual U and V array flows that we like (:
path = '/home/marina/Dropbox/TUDelft/Subjects/Q3/MAVs/ourProject/codeClassic/flow.flo'
flow = read_flow(path)
U = flow[:,:,0]
V = flow[:,:,1]
  
#Getting the original low resolution images (they have to be selected manually)
path = '/home/marina/Dropbox/TUDelft/Subjects/Q3/MAVs/ourProject/codeClassic/lowRes2/'

im1_str = path+'80211420.png'      
im2_str = path+'80311426.png'


# For graph: reading images with matplotlib and plotting ->
im_gr_1 = mpimg.imread(im1_str)
im_gr_2 = mpimg.imread(im2_str)

pil_image1 = Image.open(im1_str)
pil_image1 = np.array(pil_image1)

pil_image2 = Image.open(im2_str)
pil_image2 = np.array(pil_image2)

im = (0.5*pil_image1.copy().astype(float) + 0.5 * pil_image2.copy().astype(float)) / 255.0
flat_image=(im[:,:,0]+im[:,:,1]+im[:,:,2])/3.

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
plt.quiver(X, Y, U, -V, color='r')
plt.show()

plt.show()