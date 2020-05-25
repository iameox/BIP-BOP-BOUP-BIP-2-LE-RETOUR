#Usage : ./test.sh
#Port 8080 par défaut

# ========================= TESTS POUR L'UNICITE DES HEADERS=========================
# echo -en 'GET / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\nHost: www.example.com:8080\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 13\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\nHost: www.example.com:8080\r\nHost: www.example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 13\r\nContent-Length: 117\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"

# ========================= TESTS POUR LA SYNTAXE DE LA METHODE =========================
# echo -en 'GET / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'HEAD / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'POST / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'SUPERMETHODEDU47QUIEXISTEPAS / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"

# ========================= TESTS POUR POST ET CONTENT-LENGTH =========================
# echo -en 'POST / HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'POST / HTTP/1.1\r\nHost: www.example.com:8080\r\n\r\naquecoucoujesuisunbodysauvagesanslength' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'POST / HTTP/1.1\r\nHost: www.example.com:8080\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 13\r\n\r\nsay=Hi&to=Mom' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'POST / HTTP/1.1\r\nHost: www.example.com:8080\r\nContent-Length: 123456789123\r\n\r\nsay=Hi&to=Mom' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'POST / HTTP/1.1\r\nHost: www.example.com:8080\r\nContent-Length: 164643844\r\n\r\nsay=Hi&to=Mom' | netcat 127.0.0.1 8080; echo -e "\n"

# ========================= TESTS POUR LA VERSION=========================
# echo -en 'GET / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.0\r\nHost: www.toto.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\nHost: www.example.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/4.7\r\nHost: www.example.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"

# echo -en 'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\nHost: www.example.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /index.html HTTP/1.1\r\nHost: www.example.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /fichierquiexistepas.html HTTP/1.1\r\nHost: www.example.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\nHost: www.monsitedepeche.haha\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /particles.htm HTTP/1.1\r\nHost: www.supersitedu47.wow:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:45.0) Gecko/20100101 Firefox/45.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"

# ========================= TESTS POUR LA NORMALISATION D'URI =========================
# echo -en 'GET / HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /dir1/dir2/./dir31/../dir32/index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /%4C%27%41%72%64%C3%A8%63%68%65%20%65%73%74%20%75%6E%20%74%72%6F%75%20%70%61%75%6D%C3%A9%20%61%75%20%6D%69%6C%69%65%75%20%64%65%20%6E%75%6C%6C%65%20%70%61%72%74%20%21 HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /%2F%64%69%72%31%2F%64%69%72%32%2F%2E%2F%64%69%72%33%31/%2E%2E/%64%69%72%33%32%2F%69%6E%64%65%78%2E%68%74%6D%6C HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /ma/su%49%59per/url/./voiture/mon/su%2eper/sac/a/d%2f%94os/./././main/haha/je/te/hack/ HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /ma/su%49%59per/url/./../voiture/../../../mon/su%2eper/sac/a/d%2f%94os/./././../main/haha/je/te/hack/../../../../../../ HTTP/1.1\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"

# ========================= TESTS POUR LES TYPES MIME =========================
# echo -en 'GET /index.html HTTP/1.1\r\nHost: www.toto.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /index.jpeg HTTP/1.1\r\nHost: www.toto.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /index.png HTTP/1.1\r\nHost: www.toto.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /index.tif HTTP/1.1\r\nHost: www.toto.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
# echo -en 'GET /index.tiff HTTP/1.1\r\nHost: www.toto.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"

# ========================= TESTS POUR LA FERMETURE =========================
echo -en 'HEAD / HTTP/1.1\r\nHost: www.example.com:8080\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
echo -en 'HEAD / HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: keep-alive\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
echo -en 'HEAD / HTTP/1.1\r\nHost: www.example.com:8080\r\nConnection: close\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
echo -en 'HEAD / HTTP/1.0\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
echo -en 'HEAD / HTTP/1.0\r\nConnection: keep-alive\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"
echo -en 'HEAD / HTTP/1.0\r\nConnection: close\r\n\r\n' | netcat 127.0.0.1 8080; echo -e "\n"