#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.
    class Segment {
      private:
        std::string _data;
        uint64_t _index;

      public:
        Segment(Segment &&seg) : _data(move(seg._data)), _index(seg._index) {}
        Segment(const std::string &data, uint64_t index) : _data(data), _index(index) {}
        Segment(const std::string &&data, uint64_t index) : _data(data), _index(index) {}
        Segment &operator=(Segment &&seg) {
            _data = std::move(seg._data);
            _index = seg._index;
            return *this;
        }
        uint64_t index() const { return _index; }
        uint64_t last_index() const { return _index + _data.size(); }
        const std::string &data() const { return _data; }
        // return if the Segment can be merge with another Segment
        bool is_mergable(const Segment &seg) const { return last_index() >= seg._index and seg.last_index() >= _index; }
        // merge two Segments, or raise an exception if they not overlap.
        Segment &operator+=(const Segment &seg);
    };
    std::vector<Segment> _segments{};
    size_t _first_unassembled_index{};
    size_t _eof{};
    size_t _eof_index{};

    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
