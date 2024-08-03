#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

// About about ambiguous seqno:
// prevent ambiguous seqno in the future by window mechanism.
// prevent ambiguous seqno in the past by timestamp in the option field of header.
void TCPReceiver::segment_received(const TCPSegment &seg) {
    auto header = seg.header();

    if (!_is_syn and !header.syn) {
        return;
    }

    if (header.syn) {
        _isn = header.seqno;
        _is_syn = true;
    }
    uint64_t abs_ack_seqno = _reassembler.stream_out().bytes_written() + _reassembler.stream_out().input_ended() + 1;
    uint64_t stream_index = unwrap(header.seqno, _isn, abs_ack_seqno) + header.syn - 1;
    _reassembler.push_substring(seg.payload().copy(), stream_index, header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_is_syn) {
        return {};
    }

    return wrap(_reassembler.stream_out().bytes_written() + _reassembler.stream_out().input_ended() + 1, _isn);
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
