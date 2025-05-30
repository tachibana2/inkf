/******************************************************************************/
/*! @file WebSocket.cc
 *  @brief WebSocket class
 *  @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/select.h>
#include "fcgiapp.h"
#include "sha1.h"
#include "apolloron.h"

namespace apolloron {

/*! Constructor of WebSocket
   Initialize a new WebSocket instance.
   Sets up initial state with masking disabled (for server-to-client messages)
   and prepares the message buffer.
 */
WebSocket::WebSocket() {
    (*this).isWebSocket = false;
    (*this).maskingEnabled = false;  // Server-to-client messages are not masked
    (*this).parent = NULL;
    (*this).isCGI = false;
    (*this).isFragmented = false;
    (*this).fragmentedOpcode = TEXT;
    (*this).buffer.setFixedLength(1024); // set initial size of buffer 
    (*this).buffer.useAsBinary(0);
}

WebSocket::~WebSocket() {
}

/*! Set the parent CGI or FCGI instance for I/O operations.
   @param parent Pointer to the parent CGI or FCGI instance
   @param isCGI true if parent is CGI, false if FCGI
 */
void WebSocket::setParent(CGI* parent, bool isCGI) {
    (*this).parent = (CGI *)parent;
    (*this).isCGI = isCGI; // CGI == true, FCGI == false
}

/*! Write data to the underlying CGI/FCGI output stream.
   @param data Pointer to the data to write
   @param length Number of bytes to write
   @return true if write successful, false otherwise
 */
bool WebSocket::doWrite(const char* data, long length) {
    if ((*this).parent == NULL) return false;

    if ((*this).isCGI) {
        return fwrite(data, length, 1, stdout) == 1;
    } else {
        FCGI* fcgi = (FCGI*)(*this).parent;
        return FCGX_PutStr(data, (int)length, fcgi->request->out) == (int)length;
    }
}

/*! Read data from the underlying CGI/FCGI input stream.
   @param data Buffer to store read data
   @param length Maximum number of bytes to read
   @return Number of bytes read, or 0 on error
 */
bool WebSocket::doRead(char* data, long length) {
    if ((*this).parent == NULL) return 0;

    if ((*this).isCGI) {
        int fd = fileno(stdin);
        fd_set readfds;
        struct timeval tv = {0, 0}; // Non-blocking check

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        // Check if there is data in the input buffer
        if (select(fd + 1, &readfds, NULL, NULL, &tv) <= 0) {
            return 0; // No data available
        }

        // Read data if available
        return read(fd, data, length);
    } else {
        FCGI* fcgi = (FCGI*)(*this).parent;
        FCGX_Stream* stream = fcgi->request->in;

        // Check if stream is closed or at EOF
        if (stream->isClosed || FCGX_HasSeenEOF(stream)) {
            return 0;
        }

//        // Check if data is available using select
//        fd_set readfds;
//        struct timeval tv = {0, 0}; // Non-blocking check
//        int fd = fcgi->request->ipcFd;

//        FD_ZERO(&readfds);
//        FD_SET(fd, &readfds);

//        // Check if there is data in the input buffer
//        if (select(fd + 1, &readfds, NULL, NULL, &tv) <= 0) {
//            return 0; // No data available
//        }

        // Read data if available
        return FCGX_GetStr(data, (int)length, stream);
    }
}

/*! Perform WebSocket handshake with the given key.
   @param key Client's WebSocket key
   @return true if handshake successful, false otherwise
 */
bool WebSocket::handshake(const char* key) {
    if (!key) return false;

    String acceptKey = (*this).generateAcceptKey(key);
    if (acceptKey.empty()) return false;

    (*this).isWebSocket = true;
    return true;
}

/*! Generate WebSocket accept key from client key.
   Follows RFC 6455 specification for key generation.
   @param key Client's WebSocket key
   @return Base64 encoded accept key
 */
String WebSocket::generateAcceptKey(const char* key) {
    const char* magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char combined[1024];
    unsigned char hash[SHA1_DIGESTSIZE];
    SHA1_CTX context;
    String str;   
 
    snprintf(combined, sizeof(combined), "%s%s", key, magic);
    
    SHA1Init(&context);
    SHA1Update(&context, (unsigned char*)combined, strlen(combined));
    SHA1Final(hash, &context);

    str.setFixedLength(SHA1_DIGESTSIZE);
    str.useAsBinary(0);
    str.addBinary((const char *)hash, SHA1_DIGESTSIZE);

    return str.encodeBASE64(0, "", "", "");
}

/*! Send a WebSocket message.
   Creates a frame with the given message and opcode, then sends it.
   @param message Message to send
   @param opcode WebSocket operation code (TEXT, BINARY, etc.)
   @return true if send successful, false otherwise
 */
bool WebSocket::send(const String& message, OpCode opcode) {
    bool ret;
    if (!((*this).isWebSocket)) return false;

    String frame = createFrame(message, opcode);
    if (frame.binaryLength() == 0) return false;

    ret = doWrite(frame.c_str(), frame.binaryLength());
    if ((*this).isCGI) {
        fflush(stdout);
    } else {
        FCGI* fcgi = (FCGI*)(*this).parent;
        FCGX_FFlush(fcgi->request->out);
    }
    return ret;
}

/*! Receive a WebSocket message.
   Handles fragmented messages and maintains an internal buffer.
   @param message Reference to store received message
   @param opcode Reference to store message operation code
   @return true if complete message received, false otherwise
 */
bool WebSocket::receive(String& message, OpCode& opcode) {
    const long receive_loop_max_timeout_usec = 1000000;
    long receive_loop_timeout_usec;

    if (!((*this).isWebSocket)) return false;

    receive_loop_timeout_usec = 0;
    while (true) {
        const long max_timeout_usec = 100000;
        long timeout_usec;
        char temp[2];
        int bytesRead;
        timeout_usec = 0;

        do {
            bytesRead = doRead(temp, 1);
            if (bytesRead < 0) {
                return false; // Error in read
            }
            if (bytesRead == 0) {
                usleep(5000);
                timeout_usec += 5000;
                receive_loop_timeout_usec += 5000;
            }
        } while (bytesRead == 0 && timeout_usec < max_timeout_usec);

        if (0 < bytesRead) {
            (*this).buffer.addBinary(temp, bytesRead);
        }

        // Process frame from buffer
        size_t processed;
        bool isComplete = processFrame(
            (const unsigned char*)(*this).buffer.c_str(),
            (*this).buffer.binaryLength(),
            message,
            opcode,
            processed
        );

        if (0 < processed) {
            // Remove processed data from buffer
            String remaining;
            remaining.setFixedLength((*this).buffer.binaryLength() - processed);
            remaining.useAsBinary(0);
            remaining.addBinary(
                (*this).buffer.c_str() + processed,
                (*this).buffer.binaryLength() - processed
            );
            (*this).buffer = remaining;
        }

        if (isComplete) {
            if ((*this).isFragmented) {
                message = (*this).fragmentedMessage;
                opcode = (*this).fragmentedOpcode;
                (*this).isFragmented = false;
                (*this).fragmentedMessage.clear();
            }
            if (opcode == TEXT) {
                message.useAsText();
            }
            return true;
        }
        if (receive_loop_max_timeout_usec <= receive_loop_timeout_usec) {
            // Time out
            break;
        }
    }

    return false;
}

bool WebSocket::close() {
    if (!((*this).isWebSocket)) return false;

    String closeMessage;
    return (*this).send(closeMessage, CLOSE);
}

/*! Process a WebSocket frame.
   Handles frame parsing, unmasking, and message fragmentation.
   @param data Raw frame data
   @param length Length of frame data
   @param message Reference to store frame payload
   @param opcode Reference to store frame operation code
   @param processed Reference to store number of bytes processed
   @return true if frame represents a complete message, false if fragmented or error
 */
bool WebSocket::processFrame(const unsigned char* data, long length,
       	String& message, OpCode& opcode, size_t& processed) {
    processed = 0;

    if (length < 2) return false;

    // Parse header
    size_t header_length = 2;
    bool fin = (data[0] & 0x80) != 0;
    uint8_t rsv = (data[0] & 0x70) >> 4;  // RSV1,2,3 bits
    OpCode frameOpcode = (OpCode)(data[0] & 0x0F);

    // RSV bits must be 0
    if (rsv != 0) return false;

    // Parse mask and payload length
    bool masked = (data[1] & 0x80) != 0;
    uint64_t payload_length = data[1] & 0x7F;

    // Client-to-server messages must be masked
    if (!masked) return false;

    // Handle extended payload length
    if (payload_length == 126) {
        header_length += 2;
        if ((size_t)length < header_length) return false;
        payload_length = (((uint64_t)data[2]) << 8) | data[3];
    } else if (payload_length == 127) {
        header_length += 8;
        if ((size_t)length < header_length) return false;
        // Handle 64-bit length (big-endian)
        payload_length = 0;
        for (int i = 0; i < 8; ++i) {
            payload_length = (payload_length << 8) | data[2 + i];
        }
    }

    // Validate payload length and total frame size
    if (SIZE_MAX < payload_length || (size_t)length < header_length + payload_length) {
        processed = 0;
        return false;
    }

    if (masked) {
        header_length += 4;
    }

    if ((size_t)length < header_length + payload_length) {
        processed = 0;
        return false;
    }

    const unsigned char* payload = data + header_length;
    String frameData;
    frameData.setFixedLength(payload_length);
    frameData.useAsBinary(0);

    if (masked) {
        const unsigned char* mask = data + header_length - 4;
        char* unmasked = new char[payload_length];
        for (unsigned long i = 0; i < payload_length; i++) {
            unmasked[i] = payload[i] ^ mask[i % 4];
        }
        frameData.addBinary(unmasked, payload_length);
        delete[] unmasked;
    } else {
        frameData.addBinary((const char*)payload, payload_length);
    }

    processed = header_length + payload_length;

    // Handle message fragmentation
    if (!(*this).isFragmented) {
        if (frameOpcode == CONTINUATION) {
            // Unexpected continuation frame
            processed = 0;
            return false;
        }

        if (!fin) {
            // Start of a fragmented message
            (*this).isFragmented = true;
            (*this).fragmentedOpcode = frameOpcode;
            (*this).fragmentedMessage = frameData;
            opcode = frameOpcode;
            message = frameData;
            return false;  // Wait for more fragments
        } else {
            // Single frame message
            opcode = frameOpcode;
            message = frameData;
            if (opcode == TEXT) {
                message.useAsText();
            }
            return true;  // Complete message
        }
    } else {
        // Continuing a fragmented message
        if (frameOpcode != CONTINUATION) {
            // Expected a continuation frame
            processed = 0;
            (*this).isFragmented = false;  // Reset fragmentation state
            (*this).fragmentedMessage.clear();
            return false;
        }

        (*this).fragmentedMessage.add(frameData);

        if (fin) {
            // End of fragmented message
            opcode = (*this).fragmentedOpcode;
            message = (*this).fragmentedMessage;
            if (opcode == TEXT) {
                message.useAsText();
            }
            (*this).isFragmented = false;
            (*this).fragmentedMessage.clear();
            return true;  // Complete message
        }

        // More fragments expected
        return false;
    }
}

/*! Create a WebSocket frame from a message.
   Handles masking and proper frame formatting according to RFC 6455.
   @param message Message to frame
   @param opcode Operation code for the frame
   @return Formatted WebSocket frame as a String
 */
String WebSocket::createFrame(const String& message, OpCode opcode) {
    size_t length;
    String frame;
    if (message.isText()) {
        length = message.len();
    } else {
        length = message.binaryLength();
    }

    // Calculate frame size
    size_t frame_size = 2; // Basic header
    if (length <= 125) {
        frame_size += ((*this).maskingEnabled ? 4 : 0) + length;
    } else if (length <= 65535) {
        frame_size += 2 + ((*this).maskingEnabled ? 4 : 0) + length;
    } else {
        frame_size += 8 + ((*this).maskingEnabled ? 4 : 0) + length;
    }

    frame.setFixedLength(frame_size);
    frame.useAsBinary(0);

    // Set FIN and opcode
    frame.addBinary((char)((char)(0x80 | opcode))); // FIN + opcode

    // Set length and masking bit
    if (length <= 125) {
        frame.addBinary((char)((char)((*this).maskingEnabled ? (0x80 | length):length)));
    } else if (length <= 65535) {
        frame.addBinary((char)((char)((*this).maskingEnabled ? (0x80 | 126):126)));
        frame.addBinary((char)((char)((length >> 8) & 0xFF)));
        frame.addBinary((char)((char)(length & 0xFF)));
    } else {
        frame.addBinary((char)((*this).maskingEnabled ? (0x80 | 127) : 127));
        // send 64bit data as big endian
        size_t remaining = length;
        for (int i = 7; 0 <= i; i--) {
            frame.addBinary((char)((remaining >> (i * 8)) & 0xFF));
        }
    }

    // Add masking key and payload
    if ((*this).maskingEnabled) {
        unsigned char mask[4];
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        unsigned int seed = (unsigned int)(ts.tv_nsec ^ ts.tv_sec);
        for (int i = 0; i < 4; ++i) {
            seed = seed * 1103515245 + 12345;
            mask[i] = (seed >> 16) & 0xFF;
            frame.addBinary((char)(mask[i]));
        }

        for (size_t i = 0; i < length; ++i) {
            frame.addBinary((char)(message[i] ^ mask[i % 4]));
        }
    } else {
        frame.addBinary(message.c_str(), length);
    }

    return frame;
}

/*! Handle received WebSocket message.
   Default implementation echoes the message back.
   Can be overridden by derived classes for custom behavior.
   @param message Received message
   @param opcode Message operation code
 */
void WebSocket::onWebSocketMessage(const String& message, OpCode opcode) {
    // Default implementation - can be overridden by derived classes
    (*this).send(message, opcode);  // Echo back the message
}

} // namespace apolloron
