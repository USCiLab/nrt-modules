#include "MedianFilter.h"

MedianFilter::MedianFilter() 
{
  MedianFilter(0);
}
MedianFilter::MedianFilter(int aInitialValue)
{
  history_index = 0;
  for(int i=0; i<MEDIAN_FILTER_SIZE; i++)
  {
    history[i] = aInitialValue;
  }
}

int MedianFilter::median()
{  
  // Copy history into ordered array
  for(int i=0; i<MEDIAN_FILTER_SIZE; i++)
  {
    ordered[i] = history[i];
  }
  
  // Sort ordered array
  for(int i=1; i<MEDIAN_FILTER_SIZE; i++)
  {
    int item = ordered[i];
    int hole = i;
    while(hole > 0 && ordered[hole-1] > item)
    {
      ordered[hole] = ordered[hole-1];
      hole--;
    }
    ordered[hole] = item;
  }
  
  return ordered[(int)(MEDIAN_FILTER_SIZE/2)];
}

void MedianFilter::push(int newVal)
{
  history[history_index++] = newVal;

  // Wrap around if needed
  if(history_index == MEDIAN_FILTER_SIZE)
    history_index = 0;
}