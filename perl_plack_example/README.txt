
 1. First, edit app.psgi and replace the CONSUMER_KEY, CONSUMER_SECRET
    with the values for your application.

 2. Install dependencies:

	  # If you don't have 'cpanm' check out the 'cpanminus' instructions below.
      cpanm --sudo Plack Plack::Middleware::Session OAuth::Lite JSON Flea

	  # OR if you're brave, you can do it with cpan but it will ask you a
	  # lot of questions.  Choose the default whenever asked by simply
	  # pressing enter.
      sudo cpan -i Plack Plack::Middleware::Session OAuth::Lite JSON Flea

 3. Run the example with this command:

      plackup

 4. And then point your web-browser to port 5000 on your domain,
    possibly: http://localhost:5000

cpanminus
=========

cpanminus is a usually pain-free way of installing things from cpan. It is
itself installable with the bundled cpan command:

cpan App::cpanminus.

The cpan toolchain is showing its age. Give cpanminus a try, maybe. But before
that, consider using perlbrew as well...

things blowing up?
==================
Maybe your system is slightly tweaky, especially if you are using a
vendor-supplied perl. The least buggy way to get perl apps working in these
days of vendor-patched nonstandard perls is to compile your own perl to run
your applications with, and leave the system perl alone to continue running
the system's perl components.

perlbrew
========
Perlbrew compiles perls in your home directory and manages the various path
environment variables for you. Then you can have your own perl, without vendor
oddities, without messing with the system perl.

    cpan App::perlbrew
    <snip>

    # install the latest perl
    perlbrew install perl-5.14.2

    # switch to it (set up path, etc)
    perlbrew switch perl-5.14.2

    # install cpanminus for your brand new perl
    cpan App::cpanminus

    # install the cpan modules you wanted
    cpanm Plack OAuth::Lite JSON Flea

This should get you up and running with a minimum of fuss.

