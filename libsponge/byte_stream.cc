#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity(capacity) {}

size_t ByteStream::write(const string &data) {
    size_t remaining = remaining_capacity();
    size_t count = min(remaining, data.size());
    for (size_t i = 0; i < count; i++) {
        _deq.emplace_back(data[i]);
    }
    _bytes_written += count;
    return count;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    return string(_deq.cbegin(), _deq.cbegin()+len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t l = min(len, _deq.size());
    for (size_t i = 0; i < l; i++) {
        _deq.pop_front();
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    // TODO use stringstream
    string ret = peek_output(len);
    pop_output(len);
    return ret;
}

void ByteStream::end_input() {
    _end_input = true;
}

bool ByteStream::input_ended() const {
    return _end_input;
}

size_t ByteStream::buffer_size() const {
    return _deq.size();
}

bool ByteStream::buffer_empty() const {
    return _deq.empty();
}

bool ByteStream::eof() const {
    return _end_input and _deq.empty();
}

size_t ByteStream::bytes_written() const {
    return _bytes_written;
}

size_t ByteStream::bytes_read() const {
    return _bytes_written - _deq.size();
}

size_t ByteStream::remaining_capacity() const {
    return _capacity - _deq.size();
}
