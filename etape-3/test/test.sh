#Usage : ./test.sh
#Port 8080 par défaut

# ========================= TESTS POUR LA SYNTAXE DE LA METHODE =========================
#printf 'GET / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'GET / HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'HEAD / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'POST / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'SUPERMETHODEDU47QUIEXISTEPAS / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080

# ========================= TESTS POUR POST ET CONTENT-LENGTH =========================
printf 'POST / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080
printf 'POST / HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 13\r\n\r\nsay=Hi&to=Mom' | netcat 127.0.0.1 8080
printf 'POST / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 123456789123\r\n\r\nsay=Hi&to=Mom' | netcat 127.0.0.1 8080
printf 'POST / HTTP/1.1\r\nHost: www.example.com\r\nContent-Length: 164643844\r\n\r\nsay=Hi&to=Mom' | netcat 127.0.0.1 8080

# ========================= TESTS POUR LE MULTISITE =========================
#printf 'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'GET /fichierquiexistepas.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'GET / HTTP/1.1\r\nHost: www.monsitedepeche.haha\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'GET /particles.html HTTP/1.1\r\nHost: www.supersitedu47.wow\r\n\r\n' | netcat 127.0.0.1 8080
#printf 'GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:45.0) Gecko/20100101 Firefox/45.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n' | netcat 127.0.0.1 8080
