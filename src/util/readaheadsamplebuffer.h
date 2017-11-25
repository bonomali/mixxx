#pragma once


#include "util/indexrange.h"
#include "util/samplebuffer.h"


namespace mixxx {

// A FIFO sample buffer with fixed capacity and range checking.
//
// Samples are written at the tail and read from the head (FIFO).
// It is intended to consume all buffered samples before writing
// any new samples. A full featured ring buffer is not needed for
// this purpose.
//
// The API is not designed for concurrent readers and writers!
// Samples reserved for writing are immediately available for
// reading, even if the writer has not not yet written any samples.
// With this in mind the implementation does not make any attempts
// to be thread-safe!
class ReadAheadSampleBuffer final {
  public:
    explicit ReadAheadSampleBuffer(
            SINT capacity = 0);
    ReadAheadSampleBuffer(
            const ReadAheadSampleBuffer& that)
        : ReadAheadSampleBuffer(that, that.capacity()) {
    }
    ReadAheadSampleBuffer(
            ReadAheadSampleBuffer&&) = default;

    ReadAheadSampleBuffer& operator=(
            const ReadAheadSampleBuffer& that) {
        *this = ReadAheadSampleBuffer(that); // copy ctor + move assignment
        return *this;
    }
    ReadAheadSampleBuffer& operator=(
            ReadAheadSampleBuffer&& that) {
        swap(that);
        return *this;
    }
    
    void swap(
            ReadAheadSampleBuffer& that);

    // The maximum capacity of the buffer.
    SINT capacity() const {
        return m_sampleBuffer.size();
    }

    // Tries to adjust the capacity taking into account the
    // current contents of the buffer. The resulting capacity
    // may therefore be higher than requested when shrinking
    // the buffer.
    void adjustCapacity(SINT capacity);

    // Discards all buffered samples.
    void clear();

    bool empty() const {
        return m_readableRange.empty();
    }

    // The number of samples that could be written instantly without
    // internal reorganization, i.e. the remaining capacity of the
    // buffer.
    // Only the space between the end of the slice occupied by
    // written (= readable) samples and the end of the allocated
    // buffer is available for writing!
    SINT writableLength() const {
        return capacity() - m_readableRange.end();
    }

    // Reserves space at the buffer's tail for writing samples.
    //
    // Returns a pointer to the continuous memory region and the
    // actual number of samples that have been reserved. The maximum
    // length is limited by writableLength().
    //
    // The returned pointer is valid until the next writeToTail() operation.
    SampleBuffer::WritableSlice writeToTail(SINT maxWriteLength);

    // The number of readable samples.
    SINT readableLength() const {
        return m_readableRange.length();
    }

    // Consumes buffered samples from the head of the buffer.
    //
    // Returns a pointer to the continuous memory region and the actual
    // number of readable samples. The maximum length is limited by
    // readableLength().
    //
    // The returned pointer is valid until the next writeToTail() operation.
    SampleBuffer::ReadableSlice readFromHead(SINT maxReadLength);

    // Discards the last samples that have been written at the tail of
    // the buffer
    //
    // Returns the number of samples that have actually been dropped. The
    // number of samples that can be dropped is limited by readableLength().
    SINT dropFromTail(SINT maxDropLength);

  private:
    ReadAheadSampleBuffer(
            const ReadAheadSampleBuffer& that,
            SINT capacity);

    SampleBuffer m_sampleBuffer;
    IndexRange m_readableRange;
};

} // namespace mixxx

namespace std {
    
// Template specialization of std::swap() for ReadAheadSampleBuffer
template<>
inline void swap(::mixxx::ReadAheadSampleBuffer& lhs, ::mixxx::ReadAheadSampleBuffer& rhs) {
    lhs.swap(rhs);
}

}  // namespace std
