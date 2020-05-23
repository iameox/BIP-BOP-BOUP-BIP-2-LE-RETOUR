#define RESPONSE_200 "HTTP/1.1 200 OK\r\n\r\n"
#define RESPONSE_400 "HTTP/1.1 400 Bad Request\r\n\r\n"
#define RESPONSE_404 "HTTP/1.1 404 Not Found\r\n\r\n"
#define RESPONSE_501 "HTTP/1.1 501 Not Implemented\r\n\r\n"
#define RESPONSE_505 "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n"
#define RESPONSE_200_SIZE 19
#define RESPONSE_400_SIZE 28
#define RESPONSE_404_SIZE 26
#define RESPONSE_501_SIZE 32
#define RESPONSE_505_SIZE 43

#define RESPONSE_400_BODY "<!DOCTYPE html><html><head><title>400 - Bad Request</title></head><body><h1>400 - Bad Request</h1><p>Sorry but your request wasn't accepted by the server.</p></body></html>"
#define RESPONSE_404_BODY "<!DOCTYPE html><html><head><title>404 - Not Found</title></head><body><h1>404 - Not Found</h1><p>Sorry but the page you are requesting wasn't found.</p><p>haha t tro nul</p></body></html>"
#define RESPONSE_501_BODY "<!DOCTYPE html><html><head><title>501 - Not Implemented</title></head><body><h1>501 - Not Implemented</h1><p>Sorry but this method is either not implemented by the server or malformed.</p></body></html>"
#define RESPONSE_505_BODY "<!DOCTYPE html><html><head><title>505 - HTTP Version Not Supported</title></head><body><h1>505 - HTTP Version Not Supported</h1><p>Sorry but this HTTP version isn't supported by the server. Its current version is 1.1.</p></body></html>"
#define RESPONSE_400_BODY_SIZE 172
#define RESPONSE_404_BODY_SIZE 187
#define RESPONSE_501_BODY_SIZE 202
#define RESPONSE_505_BODY_SIZE 235