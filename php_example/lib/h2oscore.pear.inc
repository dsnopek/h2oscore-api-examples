<?php

function _h2oscore_exception_error_handler($errno, $errstr, $errfile, $errline) {
  throw new Exception($errstr, $errno);
}
set_error_handler('_h2oscore_exception_error_handler');

try {
  include_once('HTTP/OAuth/Consumer.php');
}
catch (Exception $e) {
  print '<p>Unabled to find the "HTTP_OAuth" PEAR module.  See the README.txt with this example.</p>';
  exit;
}

restore_error_handler();

require_once('HTTP/OAuth/Consumer/Exception/InvalidResponse.php');
require_once('HTTP/OAuth/Consumer/Request.php');
require_once('h2oscore.common.inc');

session_start();

// reset state
if (!isset($_GET['oauth_token']) && $_SESSION['state'] == 1) {
  $_SESSION['state'] = 0;
}

// the logout "page"
if ($_GET['logout'] == 1) {
  $_SESSION['state'] = 0;
  unset($_SESSION['token']);
  unset($_SESSION['secret']);
}
// the login "page"
else if ($_GET['login'] == 1 || $_SESSION['state'] >= 1) {
  if ($_GET['login'] == 1) {
    $_SESSION['state'] = 0;
  }
  try {
	$oauth = new HTTP_OAuth_Consumer(CONSUMER_KEY, CONSUMER_SECRET);

    if (!isset($_GET['oauth_token']) && !$_SESSION['state']) {
      $oauth->getRequestToken(H2OSCORE_REQUEST_TOKEN_URL, CONSUMER_CALLBACK_URL);
      $_SESSION['secret'] = $oauth->getTokenSecret();
      $_SESSION['state'] = 1;

      header('Location: ' . $oauth->getAuthorizeUrl(H2OSCORE_AUTHORIZE_URL));
      exit;
    }
    else if ($_SESSION['state'] == 1) {
	  $oauth->setToken($_GET['oauth_token']);
	  $oauth->setTokenSecret($_SESSION['secret']);
      $oauth->getAccessToken(H2OSCORE_ACCESS_TOKEN_URL, $_GET['oauth_verifier']);

      $_SESSION['state'] = 2;
      $_SESSION['token'] = $oauth->getToken();
      $_SESSION['secret'] = $oauth->getTokenSecret();
    }

    $oauth->setToken($_SESSION['token']);
	$oauth->setTokenSecret($_SESSION['secret']);
  }
  catch (HTTP_OAuth_Consumer_Exception_InvalidResponse $e) {
    print_r($e);
  }
}

// Copied from HTTP_OAuth_Consumer::sendRequest()
// We had to fork this function so that we could set additional headers and pass non-urlencoded
// data to POST and PUT urls.
function _h2oscore_send_request($url, $additional = NULL, $method = 'POST', $headers = array()) {
  global $oauth;

  $req = clone $oauth->getOAuthConsumerRequest();

  $params = array(
      'oauth_consumer_key'     => $oauth->getKey(),
      'oauth_signature_method' => $oauth->getSignatureMethod()
  );
  if ($oauth->getToken()) {
      $params['oauth_token'] = $oauth->getToken();
  }
  if (is_array($additional)) {
    $params = array_merge($additional, $params);
  }
  elseif (!is_null($additional)) {
    $req->setBody($additional);
  }

  if (in_array($method, array('POST', 'PUT')) && !is_null($additional)) {
    $req->setAuthType(HTTP_OAuth_Consumer_Request::AUTH_HEADER);
  }

  $req->setUrl($url);
  $req->setMethod($method);
  $req->setSecrets($oauth->getSecrets());
  $req->setParameters($params);
  
  foreach ($headers as $hname => $hvalue) {
    $req->setHeader($hname, $hvalue);
  }

  return $req->send();
}

function h2oscore($verb, $resource, $data = NULL) {
  global $oauth;

  if (empty($oauth)) {
    return NULL;
  }

  $headers = array('Accept' => 'application/json');
  $verb = strtoupper($verb);

  if (in_array($verb, array('POST', 'PUT'))) {
    $headers['Content-Type'] = 'application/json';
  }

  if (substr($resource, 0, 7) != 'http://') {
    $resource = H2OSCORE_API_URL . '/' . $resource;
  }

  if (!is_null($data)) {
    if (in_array($verb, array('POST', 'PUT'))) {
      $data = json_encode($data);
    }
  }

  $response = _h2oscore_send_request($resource, $data, $verb, $headers);
  return json_decode($response->getResponse()->getBody());
}

// attempt to get the user data
$user = NULL;
try {
  if ($oauth) {
    $user = h2oscore('POST', 'info/getUser');
  }
}
catch(Exception $e) { }

