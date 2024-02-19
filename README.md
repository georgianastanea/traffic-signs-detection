# Traffic Sign Detection Project

## Overview

This C++ OpenCV application aims to detect traffic signs based on color. The approach involves comparing histograms of templates, which are integral representations of the signs, with histograms of objects extracted from an input image.

## Project Structure

- **Templates Preparation**: The initial step involves preparing templates representing the signs to be detected. Templates are loaded, binarized, and their histograms are computed and stored in an array.

- **Application Execution**: When the application is started, users are provided with the option to select an image for traffic sign recognition. The application performs color space conversion from RGB to HSV and identifies regions of interest corresponding to red, yellow, and blue.

- **Thresholding**: Thresholds for red, yellow, and blue regions are predefined to segment the images effectively. Thresholding results in three images with different regions to be processed: red, yellow, and blue.

- **Object Labeling and Extraction**: Objects in each segmented image are labeled using BFS (Breadth-First Search) and extracted. Each object is enclosed within a bounding box, and its dimensions are analyzed. Objects that do not fulfill specific conditions are excluded to eliminate irrelevant background or unconventionally shaped objects.

- **Comparison and Selection**: Extracted objects are resized to match the templates and their histograms are computed. The histogram of each object is then compared to the ones saved initially. The object with the lowest score (fewest differences) is chosen as the result.

## Usage

1. **Preparing Templates**: Ensure that templates representing traffic signs are prepared and stored in the appropriate format.

2. **Starting the Application**: Execute the application and choose an input image for traffic sign recognition.

3. **Processing**: The application processes the input image by segmenting regions of interest based on predefined color thresholds.

4. **Object Detection**: Objects within the segmented regions are labeled, extracted, and compared against templates.

5. **Result**: The recognized traffic sign is displayed based on the comparison result.

## Color Thresholds

- Red:
  - Lower1: (0, 100, 20)
  - Upper1: (10, 255, 255)
  - Lower2: (160, 100, 20)
  - Upper2: (179, 255, 255)
  
- Blue:
  - Lower: (100, 100, 20)
  - Upper: (140, 255, 255)
  
- Yellow:
  - Lower: (10, 100, 100)
  - Upper: (50, 255, 255)

## Note

Ensure that the input image contains clearly visible traffic signs for accurate detection results. Adjustments to color thresholds may be necessary based on the lighting conditions and variations in traffic sign appearances.
