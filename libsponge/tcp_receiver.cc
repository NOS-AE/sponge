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

    if (_abs_ack_seqno == 0 and !header.syn) {
        return;
    }

    if (header.syn) {
        _isn = header.seqno;
    }
    size_t buffer_size = _reassembler.stream_out().buffer_size();
    uint64_t stream_index = unwrap(header.seqno, _isn, _abs_ack_seqno) + header.syn - 1;
    _reassembler.push_substring(seg.payload().copy(), stream_index, header.fin);
    _abs_ack_seqno +=
        _reassembler.stream_out().buffer_size() - buffer_size + header.syn + _reassembler.stream_out().input_ended();
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_abs_ack_seqno == 0) {
        return {};
    }

    return wrap(_abs_ack_seqno, _isn);
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
