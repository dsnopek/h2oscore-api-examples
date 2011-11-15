<?php

if (!class_exists('OAuth')) {
  print '<p>Unabled to find the "oauth" PECL module.  See the README.txt with this example.</p>';
  exit;
}

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
    $oauth = new OAuth(CONSUMER_KEY, CONSUMER_SECRET, OAUTH_SIG_METHOD_HMACSHA1, OAUTH_AUTH_TYPE_URI);
    $oauth->enableDebug();

    if (!isset($_GET['oauth_token']) && !$_SESSION['state']) {
      $request_token_info = $oauth->getRequestToken(H2OSCORE_REQUEST_TOKEN_URL, CONSUMER_CALLBACK_URL);
      $_SESSION['secret'] = $request_token_info['oauth_token_secret'];
      $_SESSION['state'] = 1;

      header('Location: ' . H2OSCORE_AUTHORIZE_URL . '?oauth_token=' . $request_token_info['oauth_token']);
      exit;
    }
    else if ($_SESSION['state'] == 1) {
      $oauth->setToken($_GET['oauth_token'], $_SESSION['secret']);
      $access_token_info = $oauth->getAccessToken(H2OSCORE_ACCESS_TOKEN_URL, NULL, $_GET['oauth_verifier']);

      $_SESSION['state'] = 2;
      $_SESSION['token'] = $access_token_info['oauth_token'];
      $_SESSION['secret'] = $access_token_info['oauth_token_secret'];
    }

    $oauth->setToken($_SESSION['token'], $_SESSION['secret']);
  }
  catch (OAuthException $e) {
    print_r($e);
  }
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

  $oauth->fetch($resource, $data, $verb, $headers);
  return json_decode($oauth->getLastResponse());
}

// attempt to get the user data
$user = NULL;
try {
  if ($oauth) {
    $user = h2oscore('POST', 'info/getUser');
  }
}
catch(Exception $e) { }

