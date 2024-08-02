#include "stream_reassembler.hh"

#include <algorithm>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t last_index = index + data.size();
    size_t first_unacceptable_index = _first_unassembled_index + (_capacity - _output.buffer_size());
    if (eof) {
        _eof = true;
        _eof_index = last_index;
    }
    if (data.empty()) {
        // empty substring should be ignore, but eof might be true, see it as a special case
        if (eof and _first_unassembled_index == index and _segments.empty()) {
            _output.end_input();
        }
        return;
    }
    size_t real_index = max(_first_unassembled_index, index);
    size_t real_last_index = min(first_unacceptable_index, last_index);
    if (_eof) {
        real_last_index = min(real_last_index, _eof_index);
    }
    if (real_index >= real_last_index) {
        return;
    }

    // form a Segment and merge if possible
    auto seg = Segment(data.substr(real_index - index, real_last_index - real_index), real_index);
    auto iter = _segments.begin();
    while (iter != _segments.end()) {
        if (seg.last_index() < iter->index()) {
            break;
        }
        if (not seg.is_mergable(*iter)) {
            ++iter;
            continue;
        }
        seg += *iter;
        _segments.erase(iter);
    }

    // assemble or keep as a segment
    if (seg.index() == _first_unassembled_index) {
        _output.write(seg.data());
        _first_unassembled_index = seg.last_index();
        if (_eof and seg.last_index() == _eof_index) {
            _output.end_input();
        }
    } else {
        _segments.insert(iter, std::move(seg));
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    size_t ret = 0;
    for (const auto &seg : _segments) {
        ret += seg.data().size();
    }
    return ret;
}

bool StreamReassembler::empty() const { return _segments.empty(); }

StreamReassembler::Segment &StreamReassembler::Segment::operator+=(const StreamReassembler::Segment &seg) {
    if (not is_mergable(seg)) {
        std::ostringstream out;
        out << "fail to merge Segment[" << _index << "," << _index + _data.size() << "] and Segment[" << seg._index
            << "," << seg._index + seg._data.size() << "]";
        throw std::runtime_error(out.str());
    }
    if (_index <= seg._index) {
        if (last_index() >= seg.last_index()) {
            return *this;
        }
        _data = _data.substr(0, seg._index - _index) + seg._data;
    } else {
        if (seg.last_index() >= last_index()) {
            _data = seg._data;
            _index = seg._index;
            return *this;
        }
        _data = seg._data.substr(0, _index - seg._index) + _data;
        _index = seg._index;
    }
    return *this;
}
