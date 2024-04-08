"""
Add noise to images
Author: Chengyang Yan
Date: March 2024
"""

import cv2
import glob
import numpy as np
import os
import math

def add_noise(path, saved_path, current_pixel):
    # FLIR (IMX172) Noise Characteristics
    sig_p = [0.3451, 0.1162, 0.3470]
    sig_t = [0.48, 1.21, 0.48]
    pixel_size = 2.4
    gain0 = 15

    # # OAK-Pro Wide (IMX378) Noise Characteristics
    # sig_p = [0.18, 0.22, 0.18]
    # sig_t = [0.44, 0.32, 0.45]
    # pixel_size = 2.4
    # gain0 = 15

    # # Basler DaA1280 Noise Characteristics
    # sig_p = [0.11, 0.04, 0.17]
    # sig_t = [0.79, 0.38, 1.67]
    # pixel_size = 3.75 ** 2
    # gain0 = 15

    # # Day
    # gain1 = 5
    # Night
    gain1 = gain0

    pixel_ratio = (current_pixel ** 2)/pixel_size
    gain = (1/pixel_ratio) * (10 ** (gain1/20))
    gain = 20 * (math.log(gain, 10))
    gain_ratio = 10 ** ((gain-gain0)/20)

    filenames = glob.glob(path)

    for name in filenames:
        img = cv2.imread(name)

        im_noisy = np.zeros((img.shape[0], img.shape[1], img.shape[2]))

        for c in range(3):
            img_channel = img[:, :, c]

            im_std = np.sqrt(gain_ratio * sig_p[c] * img_channel + (gain_ratio ** 2) * sig_t[c])
            generated_noisy = np.random.normal(img_channel, im_std)

            generated_noisy[generated_noisy[:] > 255] = 255
            generated_noisy[generated_noisy[:] < 0] = 0

            im_noisy[:, :, c] = generated_noisy

        im_noisy = im_noisy.astype(np.uint8)
        cv2.imwrite(os.path.join(saved_path, os.path.basename(name).replace('.tif', '.tif')), im_noisy)



