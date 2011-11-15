#!/bin/bash

import oauth2 as oauth
import sys, urlparse
from urllib import urlencode

try:
    import simplejson as json
except ImportError:
    import json

# Replace these with the values from:
#   http://www.h2oscore.com/user/me/oauth/consumers
CONSUMER_KEY = 'xxx'
CONSUMER_SECRET = 'xxx'

# While on this page on h2oscore.com:
#   http://www.h2oscore.com/user/me/oauth/consumers
# Click 'authorize' for your app, and copy the key and
# secret from it.
TOKEN_KEY = 'xxx'
TOKEN_SECRET = 'xxx'

BASE_URL = 'http://www.h2oscore.com/api/v1'

class Proxy(object):
    def __init__(self, client, path):
        self.client = client

        if path[-1] == '/':
            path = path[:-1]
        self.path = path

    def request(self, verb, resource, data=None):
        headers = {}
        headers['Accept'] = 'application/json';

        if verb in ('POST', 'PUT'):
            headers['Content-Type'] = 'application/json'

        if self.path != '' and not resource.startswith('http://'):
            resource = self.path + '/' + resource

        if data is not None:
            if verb in ('POST', 'PUT'):
                data = json.dumps(data)
            else:
                resource += '?' + urlencode(data)
                data = None
        if data is None:
            data = ''
 
        resp, content = self.client.request(resource, verb, data, headers)
        if resp['status'] == '200':
            if content:
                try:
                    return json.loads(content)
                except Exception, e:
                    print content
                    raise
        else:
            raise Exception(resp['status'] + ' ' + content)

def main():
    consumer = oauth.Consumer(CONSUMER_KEY, CONSUMER_SECRET)
    token = oauth.Token(TOKEN_KEY, TOKEN_SECRET)

    client = oauth.Client(consumer, token)
    proxy = Proxy(client, BASE_URL)

    addresses = proxy.request('GET', 'address', {'limit': 10, 'parameters[city]': 'MILWAUKEE'})
    for address in addresses:
        print address

if __name__ == '__main__': main()

