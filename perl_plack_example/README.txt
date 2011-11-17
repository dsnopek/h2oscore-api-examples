
 1. First, edit app.psgi and replace the CONSUMER_KEY, CONSUMER_SECRET
    with the values for your application.

 2. Install dependencies by runing this command:

      sudo cpan -i Plack Plack::Middleware::Session OAuth::Lite JSON Flea

    ... choose the default whenever asked by simply pressing enter.

 3. Run the example with this command:

      plackup

 4. And then point your web-browser to port 5000 on your domain,
    possibly: http://localhost:5000

