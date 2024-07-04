#define INFERNO_H
#ifdef INFERNO_H

#include <iostream>
#include <cmath>

template <typename T>
void merge(T array[], int const left, int const mid,
           int const right)
{
    int const left_size = mid - left + 1;
    int const right_size = right - mid;

    // Create temp arrays
    auto *leftArray = new int[left_size],
         *rightArray = new int[right_size];

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (auto i = 0; i < left_size; i++)
        leftArray[i] = array[left + i];
    for (auto j = 0; j < right_size; j++)
        rightArray[j] = array[mid + 1 + j];

    auto l = 0, r = 0;
    int k = left;

    // Merge the temp arrays back into array[left..right]
    while (l < left_size && r < right_size) {
        if (leftArray[l] <= rightArray[r]) {
          array[k] = leftArray[l];
          l++;
        }
        else {
          array[k] = rightArray[r];
          r++;
        }
        k++;
    }

    // Copy the remaining elements of
    // left[], if there are any
    while (l < left_size) {
      array[k] = leftArray[l];
      l++;
      k++;
    }

    // Copy the remaining elements of
    // right[], if there are any
    while (r < right_size) {
      array[k] = rightArray[r];
      r++;
      k++;
    }
    delete[] leftArray;
    delete[] rightArray;
}

// start is for left index and end is right index
// of the sub-array of arr to be sorted
template <typename T>
void mergeSort(T array[], int const start, int const end)
{
  if (start >= end)
    return;

  int mid = start + (end - start) / 2;
  mergeSort(array, start, mid);
  mergeSort(array, mid + 1, end);
  merge(array, start, mid, end);
}

//Parameters (arr, pivot, lowerbound, upperbound)
//arr = array to iterate over
//pivot = search value
//lowerbound = start index to search
//upperbound = last index to search
template <typename T>
int binarySearch(T arr[], T pivot, int lowerbound, int upperbound){
  int mid = 0;

  while(lowerbound <= upperbound){
    //do until the pointers lowerbound and upperbound meet each other.
    mid = lowerbound + (upperbound - lowerbound)/2; //move mid point up
    if (pivot == arr[mid]){
        return mid;
    }
    else if (pivot > arr[mid]){ // x is on the right side
        lowerbound = mid + 1;
    }
    else{                       // x is on the left side
        upperbound = mid - 1;
    }
  }
  return -1;
}

#endif