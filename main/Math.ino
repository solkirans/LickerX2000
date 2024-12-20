#include <stddef.h>

// Function to calculate the average of an array
float calculateAverage(const float* array, size_t length) {
    if (array == nullptr || length == 0) {
        // Return an error code if the array is null or length is zero
        return -1.0f;
    }

    int sum = 0;
    for (size_t i = 0; i < length; i++) {
        sum += array[i];
    }

    return static_cast<float>(sum) / length;
}
