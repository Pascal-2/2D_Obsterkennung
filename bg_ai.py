import cv2
import numpy as np
import time
import os

# --- Configuration ---
IMAGE_PATH = "images/Tomaten-Cocktail/PICT_20250604_154015.JPG"
# Create a dummy image if the path doesn't exist, so the script is runnable
if not os.path.exists(IMAGE_PATH):
    print(f"Warning: Image path not found. Creating a dummy test image.")
    dummy_img = np.zeros((1080, 1920, 3), dtype=np.uint8)
    # Add some dark gray patches (like the ones to be removed)
    dummy_img[100:200, 100:200] = [30, 32, 31]
    # Add some other colored objects
    cv2.circle(dummy_img, (500, 500), 150, (0, 0, 255), -1)  # large red circle
    # Add some small "dots" to be removed
    cv2.circle(dummy_img, (800, 200), 5, (0, 255, 0), -1)  # small green dot
    cv2.circle(dummy_img, (900, 300), 8, (255, 0, 0), -1)  # small blue dot
    test_img = dummy_img
else:
    # Use cv2.IMREAD_COLOR for clarity, it's the default.
    # OpenCV reads images in BGR order by default.
    test_img = cv2.imread(IMAGE_PATH, cv2.IMREAD_COLOR)

if test_img is None:
    raise FileNotFoundError(f"Could not read image from {IMAGE_PATH}")


# --- Optimized Functions ---

def remove_bg_vectorized(img):
    """
    Removes the background using NumPy boolean masking. This is orders of magnitude
    faster than iterating with Python loops.
    """
    # Important: Work with a wider data type to prevent overflow when summing.
    # OpenCV loads images as BGR (Blue, Green, Red).
    img_int = img.astype(np.int16)
    b, g, r = img_int[:, :, 0], img_int[:, :, 1], img_int[:, :, 2]

    # Create a boolean "mask" for all pixels that meet the criteria.
    # This performs the calculation on the entire image at once.
    is_dark_gray_mask = ((r + g + b) < 120) & \
                        (np.abs(r - g) < 10) & \
                        (np.abs(g - b) < 10)

    # Use the mask to set all matching pixels to white.
    # This is also a single, highly optimized operation.
    img[is_dark_gray_mask] = [255, 255, 255]
    return img


def remove_dots_optimized(img, min_size_threshold=1000):
    """
    Removes small noise/dots using OpenCV's built-in and highly optimized
    connected components labeling algorithm.
    """
    # 1. Create a binary image: 0 for background (white), 255 for foreground (everything else)
    # This is the required input for connectedComponentsWithStats.
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Threshold the image. Anything not pure white (255) is considered part of an object.
    _, binary_mask = cv2.threshold(gray, 254, 255, cv2.THRESH_BINARY_INV)

    # 2. Find all connected components and get their stats (including area)
    # This one function replaces your entire BFS/DFS search and is implemented in C++.
    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(binary_mask, 8, cv2.CV_32S)

    # 3. Create a mask to identify all the small objects we want to remove.
    # We start with an empty mask of zeros.
    small_objects_mask = np.zeros(binary_mask.shape, dtype="uint8")

    # Iterate from 1 to num_labels (0 is the background label)
    for i in range(1, num_labels):
        # Get the area of the component from the stats matrix
        area = stats[i, cv2.CC_STAT_AREA]

        # If the area is smaller than our threshold, add it to our mask
        if area < min_size_threshold:
            # The 'labels' image contains the ID of the component for each pixel.
            # We find all pixels that belong to the current small component 'i'.
            small_objects_mask[labels == i] = 255

    # 4. Use the final mask to set the small objects to white in the original image.
    # The mask is non-zero (255) for all small dots.
    img[small_objects_mask > 0] = [255, 255, 255]
    return img


# --- Benchmarking ---
img_copy_1 = test_img.copy()
img_copy_2 = test_img.copy()

print(f"Image Dimensions: {test_img.shape}")

# --- New Optimized Method ---
print("\n--- Running Optimized Version ---")
start_opt = time.time_ns()
processed_img = remove_bg_vectorized(img_copy_1)
end_bg_opt = time.time_ns()
print(f"Optimized remove_bg:    {(end_bg_opt - start_opt) / 1_000_000:.4f} ms")

processed_img = remove_dots_optimized(processed_img)
end_dots_opt = time.time_ns()
print(f"Optimized remove_dots:  {(end_dots_opt - end_bg_opt) / 1_000_000:.4f} ms")
print(f"TOTAL OPTIMIZED TIME:   {(end_dots_opt - start_opt) / 1_000_000:.4f} ms")

cv2.imwrite("test/img4_optimized.jpeg", processed_img)
print("\nSaved optimized image to 'test/img4_optimized.jpeg'")
