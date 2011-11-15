
First things first, modify config.inc to include the correct
values for YOUR consumer key, secret and callback URL.

This example includes support for a couple different methods
of doing OAuth, depending on what you have available to you.

We can use the PEAR module HTTP_OAuth or the "oauth" PECL
module.  The PECL module is probably more performant and is
considered by the "de facto standard" by some.

However, if you are on shared hosting, installing a PECL module
might not be an option for you.  For that reason, the PEAR
module is probably portable to more environments.

Using the PEAR module
---------------------

Depends on the 'HTTP_OAuth' PEAR module:

  http://pear.php.net/package/HTTP_OAuth/download

Install it via the following command:

  pear install HTTP_OAuth-0.2.3

Then make sure that the 'lib/h2oscore.pear.inc' module is required
at the top of the index.php file (this is the default).

Using the PECL module
---------------------

Depends on the 'oauth' PECL module:

  http://php.net/manual/en/book.oauth.php

Install it via the following command:

  pecl oauth

Then modify your php.ini to include:

  extension = oauth.so
