use 5.14.0;
use warnings;

use OAuth::Lite::Consumer;
use JSON;
use Flea;
use Plack::Builder;

my $consumer = OAuth::Lite::Consumer->new(
    consumer_key       => 'is4vQoaFYBFT8hL3TKsqWK4v2hXHjGP2',
    consumer_secret    => 'ALjshvGHSiegPDgFY9eesrmdsRnWb8XN',
    site               => 'http://beta.h2oscore.com',
    realm              => 'http://beta.h2oscore.com',
    request_token_path => '/oauth/request_token',
    access_token_path  => '/oauth/access_token',
    authorize_path     => '/oauth/authorize',
);

sub decode {
    my ($res, $getkey) = @_;
    unless ($res->is_success) {
        if ($res->code == 400 || $res->code == 401) {
            my $header = $res->header('WWW-Authenticate');
            if ($header && $header =~ /^OAuth/) {
                # auth has expired, go get a new one
                http 302, location => $getkey;
            }
            else {
                # some other auth error, what could it be?
            }
        }
        # nothing makes sense any more, I'm going home
        die 'HTTP ' . $res->status_line;
    }
    return JSON::decode_json($res->decoded_content);
}

builder {
    enable 'HTTPExceptions';
    enable 'Session';
    bite {
        get '^/$' {
            file 'static/index.html';
        }

        get '^/myscore$' {
            my $req   = request(shift);
            my $ses   = $req->session;
            my $token = $ses->{access_token};
            unless ($token) {
                http 302, location => uri($req, '/getkey');
            }
            my $res = $consumer->request(
                method => 'GET',
                url    => 'http://beta.h2oscore.com/api/v1/address',
                params => { limit => 1, 'parameters[owned_by]' => 'me' },
                token  => $token,
            );
            
            my $gk    = uri($req, '/getkey');
            my $uri   = decode($res, $gk)->[0]->{uri};
            $res = $consumer->request(
                method  => 'POST',
                headers => ['Content-Type' => 'application/json'],
                content => '',
                url     => "$uri/getLatestScore",
                token   => $token,
            );
            my $score = decode($res, $gk)->{percentile};
            text "Your h2oscore is $score! Good job, maybe!";
        }

        get '^/getkey$' {
            my $req   = request(shift);
            my $ses   = $req->session;
            my $rt = $ses->{request_token} = $consumer->get_request_token(
                callback_url => uri($req, '/oauth/callback')->as_string,
            ) or die $consumer->errstr;
            my $auth_url = $consumer->url_to_authorize(token => $rt)
                or die $consumer->errstr;
            http 302, location => $auth_url;
        }

        get '^/oauth/callback$' {
            my $req = request(shift);
            my $ses = $req->session;
            my $token = $consumer->get_access_token(
                token    => $ses->{request_token},
                verifier => $req->param('oauth_verifier'),
            ) or die $consumer->errstr;
            $ses->{access_token} = $token;
            http 302, location => uri($req, '/myscore');
        }
    }
};

# vim: set ft=perl :
