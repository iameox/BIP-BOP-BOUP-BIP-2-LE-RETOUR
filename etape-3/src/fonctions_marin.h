#define HEADERS { "Connection_header", "Content_Length_header", "Content_Type_header", "Cookie_header", "Transfer_Encoding_header", "Expect_header", "Host_header", "Accept_header", "Accept_Charset_header", "Accept_Encoding_header", "Accept_Language_header", "Referer_header", "User_Agent_header" }
#define NB_HEADERS 13

#define EXTENSIONS { "woff2", "xhtml", "docx", "epub", "html", "jpeg", "json", "midi", "mpeg", "mpkg", "pptx", "tiff", "weba", "webm", "webp", "woff", "xlsx", "3gp2", "aac", "abw", "arc", "avi", "azw", "bin", "bz2", "csh", "css", "csv", "doc", "eot", "gif", "htm", "ico", "ics", "jar", "jpg", "mid", "odp", "ods", "odt", "oga", "ogv", "ogx", "otf", "png", "pdf", "ppt", "rar", "rtf", "svg", "swf", "tar", "tif", "ttf", "vsd", "wav", "xls", "xml", "xul", "zip", "3gp", "bz", "js", "sh", "ts", "7z" }
#define EXTENSIONS_TYPES_MIME { "font/woff2", "application/xhtml+xml", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/epub+zip", "text/html", "image/jpeg", "application/json", "audio/midi", "video/mpeg", "application/vnd.apple.installer+xml", "application/vnd.openxmlformats-officedocument.presentationml.presentation", "image/tiff", "audio/webm", "video/webm", "image/webp", "font/woff", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "video/3gpp2", "audio/aac", "application/x-abiword", "application/octet-stream", "video/x-msvideo", "application/vnd.amazon.ebook", "application/octet-stream", "application/x-bzip2", "application/x-csh", "text/css", "text/csv", "application/msword", "application/vnd.ms-fontobject", "image/gif", "text/html", "image/x-icon", "text/calendar", "application/java-archive", "image/jpeg", "audio/midi", "application/vnd.oasis.opendocument.presentation", "application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.text", "audio/ogg", "video/ogg", "application/ogg", "font/otf", "image/png", "application/pdf", "application/vnd.ms-powerpoint", "application/x-rar-compressed", "application/rtf", "image/svg+xml", "application/x-shockwave-flash", "application/x-tar", "image/tiff", "font/ttf", "application/vnd.visio", "audio/x-wav", "application/vnd.ms-excel", "application/xml", "application/vnd.mozilla.xul+xml", "application/zip", "video/3gpp", "application/x-bzip", "application/javascript", "application/x-sh", "application/typescript", "application/x-7z-compressed" }
#define NB_EXTENSIONS 66

int are_unique_headers(void *root);

//int is_http_version_ok(void *root);
int is_http_version_ok(string *http_version, string *host_header);


string* get_mime_type(string *filename);


string* get_mime_type_with_magic(char *filename);