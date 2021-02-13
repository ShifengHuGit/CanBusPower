public function createStackString($tenancy_id, $user_id, $thumbprint, $oc_key_location_path, $key_passphrase,
$content_type, $content_length, $content_sha256)
{


$key_location = "file://$oc_key_location_path";
$key_id = "$tenancy_id/$user_id/$thumbprint";

$headers = "date (request-target) host";

// example: Thu, 05 Jan 2014 21:31:40 GMT
$date=gmdate("D, d M Y H:i:s T", time());
$method = 'put';
$request_target = urlencode("/" . $this->urlCreateMultipartUpload());
$region = $this->systemParameterService->findByName('oc_region');
$host = "objectstorage.$region.oraclecloud.com";

$signing_string = "date: $date\n(request-target): $method $request_target\nhost: $host";

// additional required headers for POST and PUT requests
if ($method == 'post' || $method == 'put') {

$headers = $headers . " content-type content-length";
$signing_string = $signing_string . "\ncontent-type: $content_type\ncontent-length: $content_length";
}

// echo "Signing string:\n$signing_string".PHP_EOL;

$signature = $this->sign_string($signing_string, $key_location, $key_passphrase);

$authorization_header = "Signature version=\"1\",keyId=\"$key_id\",algorithm=\"rsa-sha256\",headers=\"$headers\",signature=\"$signature\"";

return $authorization_header;
}

function sign_string($data, $key_path, $passphrase){
$pkeyid = openssl_pkey_get_private($key_path, $passphrase);
if (!$pkeyid) {
exit('Error reading private key');
}

openssl_sign($data, $signature, $pkeyid, OPENSSL_ALGO_SHA256);

// free the key from memory
openssl_free_key($pkeyid);

return base64_encode($signature);
}