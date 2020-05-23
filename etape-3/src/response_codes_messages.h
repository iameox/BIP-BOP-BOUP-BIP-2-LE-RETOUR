#define RESPONSE_400 "HTTP/1.1 400 Bad Request\r\n\r\n<head><title>400 - Bad Request</title></head><body><h1>400 - Bad Request</h1><p>Sorry but your request wasn't accepted by the server.</p></body>"
#define RESPONSE_400_SIZE 172
#define RESPONSE_404 "HTTP/1.1 404 Not Found\r\n\r\n<head><title>404 - Not Found</title></head><body><h1>404 - Not Found</h1><p>Sorry but the page you are requesting wasn't found.</p><p>haha t tro nul</p></body>"
#define RESPONSE_404_SIZE 185
#define RESPONSE_501 "HTTP/1.1 501 Not Implemented\r\n\r\n<head><title>501 - Not Implemented</title></head><body><h1>501 - Not Implemented</h1><p>Sorry but this method is either not implemented by the server or malformed.</p></body>"
#define RESPONSE_501_SIZE 206
#define RESPONSE_505 "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n<head><title>505 - HTTP Version Not Supported</title></head><body><h1>505 - HTTP Version Not Supported</h1><p>Sorry but this HTTP version isn't supported by the server. Its current version is 1.1.</p></body>"
#define RESPONSE_505_SIZE 250
