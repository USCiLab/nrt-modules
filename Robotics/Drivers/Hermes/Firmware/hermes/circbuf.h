/* Circular buffer object */
template<int size_>
class CircularBuffer{
  public:
    CircularBuffer() :
      size(size_+1),
      start(0),
      end(0)
  { }

    int isFull() 
    {
      return (this->end + 1) % this->size == this->start;
    }

    int isEmpty() 
    {
      return this->end == this->start;
    }

    /* Write an element, overwriting oldest element if buffer is full. */
    void write(uint8_t elem) 
    {
      this->elems[this->end] = elem;
      this->end = (this->end + 1) % this->size;
      if (this->end == this->start)
        this->start = (this->start + 1) % this->size; /* full, overwrite */
    }

    /* Read oldest element. */
    uint8_t read() 
    {
      uint8_t elem = this->elems[this->start];
      this->start = (this->start + 1) % this->size;
      return elem;
    }

    /* Peek at an element */
    uint8_t peek(int index)
    {
      return this->elems[(this->start + index) % this->size];
    }

    /* Get the capacity of the buffer */
    int capacity()
    {
      return size - 1;
    }
  private:

    int         size;            /* maximum number of elements           */
    int         start;           /* index of oldest element              */
    int         end;             /* index at which to write new element  */
    uint8_t     elems[size_+1];  /* vector of elements                   */
};
