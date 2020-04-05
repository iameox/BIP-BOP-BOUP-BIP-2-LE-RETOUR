declare -a j=1
declare -a k
for dir in ../tests/avances/*
do
  for i in {0..199}
  do
    k=1
    echo "Parsing de $dir/$i.txt :" >> result.txt

    for key in URI hier-part URI-reference absolute-URI relative-ref relative-part scheme authority userinfo host2 port IP-literal IPvFuture IPv6address h16 ls32 IPv4address dec-octet reg-name path path-abempty path-absolute path-noscheme path-rootless path-empty segment segment-nz segment-nz-nc pchar query fragment pct-encoded unreserved reserved gen-delims sub-delims language-range alphanum Language-Tag langtag language extlang script region variant extension singleton privateuse grandfathered irregular regular BWS Connection Content-Length HTTP-message HTTP-name HTTP-version Host OWS RWS TE Trailer Transfer-Encoding Upgrade Via absolute-form absolute-path asterisk-form authority-form chunk chunk-data chunk-ext chunk-ext-name chunk-ext-val chunk-size chunked-body comment connection-option ctext field-content field-name field-value field-vchar http-URI https-URI last-chunk message-body method obs-fold obs-text origin-form partial-URI protocol protocol-name protocol-version pseudonym qdtext quoted-pair quoted-string rank reason-phrase received-by received-protocol request-line request-target start-line status-code status-line t-codings t-ranking tchar token trailer-part transfer-coding transfer-extension transfer-parameter uri-host Accept Accept-Charset Accept-Encoding Accept-Language Allow Content-Encoding Content-Language Content-Location Content-Type Date Expect GMT HTTP-date IMF-fixdate Location Max-Forwards Referer Retry-After Server User-Agent Vary accept-ext accept-params asctime-date charset codings content-coding date1 date2 date3 day day-name day-name-l delay-seconds hour media-range media-type minute month obs-date parameter product product-version qvalue rfc850-date second subtype time-of-day type weight year ETag If-Match If-Modified-Since If-None-Match If-Unmodified-Since Last-Modified entity-tag etagc opaque-tag weak Accept-Ranges Content-Range If-Range Range acceptable-ranges byte-content-range byte-range byte-range-resp byte-range-set byte-range-spec byte-ranges-specifier bytes-unit complete-length first-byte-pos last-byte-pos other-content-range other-range-resp other-range-set other-range-unit other-ranges-specifier range-unit suffix-byte-range-spec suffix-length unsatisfied-range Age Cache-Control Expires Pragma Warning cache-directive delta-seconds extension-pragma pragma-directive warn-agent warn-code warn-date warn-text warning-value Proxy-Authenticate Proxy-Authorization WWW-Authenticate auth-param auth-scheme challenge credentials Authorization token68 Connection-header Content-Length-header Content-Type-header Trailer-header Transfer-Encoding-header Upgrade-header Via-header Age-header Expires-header Date-header Location-header Retry-After-header Vary-header Warning-header Cache-Control-header Expect-header Host-header Max-Forwards-header Pragma-header Range-header TE-header If-Match-header If-None-Match-header If-Modified-Since-header If-Unmodified-Since-header If-Range-header Accept-header Accept-Charset-header Accept-Encoding-header Accept-Language-header Authorization-header Proxy-Authorization-header Referer-header User-Agent-header cookie-pair cookie-name cookie-value cookie-octet Cookie-header cookie-string header-field ALPHA BIT CHAR CR CRLF CTL DIGIT DQUOTE HEXDIG HTAB LF LWSP OCTET SP VCHAR WSP _CHAR-VAL _NUM-VAL
    do
      echo -en "\e[0K\r($((100*j/8600))%) Parsing de $dir/$i.txt : Recherche $k/284 ($key)..."
      
      ./main $dir/$i.txt $key >> result.txt
      echo "" >> result.txt
      
      k=$((k+1))
    done;

    echo "------------------------------------" >> result.txt
    j=$((j+1))
  done
done

echo -e "\n Parsing effectué avec succès."
