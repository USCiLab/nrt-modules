#ifndef MEDIAN_FILTER
#define MEDIAN_FILTER

#define MEDIAN_FILTER_SIZE 10

class MedianFilter
{
public:
  MedianFilter();
  MedianFilter(int aInitialValue);
  int median();
  void push(int newVal);
  
private:
  int history[MEDIAN_FILTER_SIZE]; // Circular buffer
  int history_index; // Current index
  
  int ordered[MEDIAN_FILTER_SIZE];
};

#endif