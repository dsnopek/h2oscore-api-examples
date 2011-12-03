using System.Json;
using System.Web;
using System.Web.Mvc;
using Hammock;
using Hammock.Authentication.OAuth;
using Hammock.Web;

namespace c_sharp_example.Controllers
{
    public class IndexController : Controller
    {
        private readonly string _consumerKey = "PLACE KEY HERE";
        private readonly string _consumerSecret = "PLACE SECRET HERE";

        public ActionResult Index()
        {
            // if there is a value in the session
            // then this user has authenticated
            if (Session["accessToken"] != null)
            {
                var accessToken = (string)Session["accessToken"];
                var accessSecret = (string)Session["accessSecret"];

                // fecth the user information
                var credentials = new OAuthCredentials
                {
                    Type = OAuthType.AccessToken,
                    SignatureMethod = OAuthSignatureMethod.HmacSha1,
                    ParameterHandling = OAuthParameterHandling.HttpAuthorizationHeader,
                    ConsumerKey = _consumerKey,
                    ConsumerSecret = _consumerSecret,
                    Token = accessToken,
                    TokenSecret = accessSecret
                };

                var client = new RestClient()
                {
                    Authority = "http://www.h2oscore.com/api/v1",
                    Credentials = credentials
                };

                var request = new RestRequest
                {
                    Path = "/info/getUser",
                    Method = WebMethod.Post
                };

                RestResponse response = client.Request(request);
                var user = JsonValue.Parse(response.Content);

                credentials = new OAuthCredentials
                {
                    Type = OAuthType.AccessToken,
                    SignatureMethod = OAuthSignatureMethod.HmacSha1,
                    ParameterHandling = OAuthParameterHandling.HttpAuthorizationHeader,
                    ConsumerKey = _consumerKey,
                    ConsumerSecret = _consumerSecret,
                    Token = accessToken,
                    TokenSecret = accessSecret
                };

                client = new RestClient()
                {
                    Authority = "http://www.h2oscore.com/api/v1",
                    Credentials = credentials
                };
                
                request = new RestRequest
                {
                    Path = "/address",
                    Method = WebMethod.Get
                };
                request.AddParameter("parameters[owned_by]", "me");
                
                
                response = client.Request(request);
                var address = JsonValue.Parse(response.Content)[0];

                credentials = new OAuthCredentials
                {
                    Type = OAuthType.AccessToken,
                    SignatureMethod = OAuthSignatureMethod.HmacSha1,
                    ParameterHandling = OAuthParameterHandling.HttpAuthorizationHeader,
                    ConsumerKey = _consumerKey,
                    ConsumerSecret = _consumerSecret,
                    Token = accessToken,
                    TokenSecret = accessSecret
                };

                client = new RestClient()
                {
                    Authority = address.AsDynamic().uri.Value,
                    Credentials = credentials
                };

                request = new RestRequest
                {
                    Path = "/getLatestScore",
                    Method = WebMethod.Post
                };

                response = client.Request(request);
                var score = JsonValue.Parse(response.Content);

                ViewBag.user = user.AsDynamic();
                ViewBag.address = address.AsDynamic();
                ViewBag.score = score.AsDynamic();

                return View();
            }
            else
            {
                return View();
            }
        }

        public RedirectResult Login()
        {
            var credentials = new OAuthCredentials
            {
                Type = OAuthType.RequestToken,
                SignatureMethod = OAuthSignatureMethod.HmacSha1,
                ParameterHandling = OAuthParameterHandling.HttpAuthorizationHeader,
                ConsumerKey = _consumerKey,
                ConsumerSecret = _consumerSecret
            };

            var client = new RestClient
            {
                Authority = "http://www.h2oscore.com/oauth",
                Credentials = credentials
            };

            var request = new RestRequest
            {
                Path = "/request_token"
            };

            RestResponse response = client.Request(request);

            var collection = HttpUtility.ParseQueryString(response.Content);

            var requestSecret = collection["oauth_token_secret"];
            var requestKey = collection["oauth_token"];

            Session["requestSecret"] = requestSecret;

            return new RedirectResult("http://www.h2oscore.com/oauth/authorize?oauth_token=" + requestKey);
        }

        public RedirectResult Auth()
        {
            var requestToken = Request.QueryString["oauth_token"];
            var requestSecret = (string)Session["requestSecret"];
            var requestVerifier = Request.QueryString["oauth_verifier"];

            var credentials2 = new OAuthCredentials
            {
                Type = OAuthType.AccessToken,
                SignatureMethod = OAuthSignatureMethod.HmacSha1,
                ParameterHandling = OAuthParameterHandling.HttpAuthorizationHeader,
                ConsumerKey = _consumerKey,
                ConsumerSecret = _consumerSecret,
                Token = requestToken,
                TokenSecret = requestSecret,
                Verifier = requestVerifier
            };

            var client2 = new RestClient()
            {
                Authority = "http://www.h2oscore.com/oauth",
                Credentials = credentials2
            };

            var request2 = new RestRequest
            {
                Path = "/access_token"
            };

            RestResponse response2 = client2.Request(request2);
            
            var collection = HttpUtility.ParseQueryString(response2.Content);

            Session["accessToken"] = collection["oauth_token"];
            Session["accessSecret"] = collection["oauth_token_secret"];

            return new RedirectResult("/");
        }
    }
}
