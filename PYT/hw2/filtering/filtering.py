"""
This file defines a function that applies a filter (convolution) to an image.
It works for both grayscale and RGB images.
"""

import numpy as np


def apply_filter(image: np.array, kernel: np.array) -> np.array:
    """ Apply given filter on image """
    # A given image has to have either 2 (grayscale) or 3 (RGB) dimensions
    assert image.ndim in [2, 3]
    # A given filter has to be 2 dimensional and square
    assert kernel.ndim == 2
    assert kernel.shape[0] == kernel.shape[1]

    size=kernel.shape[0]
    pad=size//2

    if image.ndim == 2:
        padded=np.pad(image, ((pad, pad), (pad, pad)), 'constant')
        result = np.zeros((image.shape[0], image.shape[1]))
        for w in range(image.shape[0]):
            for h in range(image.shape[1]):
                cut=padded[w:w+size,h:h+size]
                result[w,h]=np.sum(cut*kernel)
    else:
        padded=np.pad(image, ((pad, pad), (pad, pad),(0,0)), 'constant')
        result = np.zeros((image.shape[0], image.shape[1], image.shape[2]))
        for w in range(image.shape[0]):
            for h in range(image.shape[1]):
                for c in range(image.shape[2]):
                    cut=padded[w:w+size,h:h+size,c]
                    result[w,h,c]=np.sum(cut*kernel)
    np.clip(result, 0, 255,out=result)
    return result.astype(np.uint8)
