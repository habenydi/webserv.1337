#include "include.hpp"
#include <cassert>
#include <iostream>

void test_basic_response()
{
    std::cout << "Testing basic response creation...\n";

    HttpResponse response(HttpResponse::OK);
    response.setBody("Hello World");

    std::string result = response.toString();

    assert(result.find("HTTP/1.0 200 OK") != std::string::npos);
    assert(result.find("Content-Type: text/html") != std::string::npos);
    assert(result.find("Content-Length: 11") != std::string::npos);
    assert(result.find("Hello World") != std::string::npos);

    std::cout << "✓ Basic response test passed\n";
}

void test_get_request()
{
    std::cout << "\nTesting GET request handler...\n";

    HttpRequest request;
    request.method = "GET";
    request.path = "/";
    request.version = "HTTP/1.0";

    HttpResponse response = RequestHandler::handleRequest(request);
    std::string result = response.toString();

    std::cout << "GET Response:\n"
              << result << "\n";
    std::cout << "✓ GET request test completed\n";
}

void test_post_request()
{
    std::cout << "\nTesting POST request handler...\n";

    HttpRequest request;
    request.method = "POST";
    request.path = "/submit-form";
    request.version = "HTTP/1.0";
    request.body = "username=test&password=secret";

    HttpResponse response = RequestHandler::handleRequest(request);
    std::string result = response.toString();

    assert(result.find("200 OK") != std::string::npos);
    assert(result.find("POST Request Received") != std::string::npos);

    std::cout << "POST Response:\n"
              << result << "\n";
    std::cout << "✓ POST request test passed\n";
}

void test_invalid_version()
{
    std::cout << "\nTesting invalid HTTP version...\n";

    HttpRequest request;
    request.method = "GET";
    request.path = "/";
    request.version = "HTTP/2.0";

    HttpResponse response = RequestHandler::handleRequest(request);
    std::string result = response.toString();

    assert(result.find("400 Bad Request") != std::string::npos);

    std::cout << "✓ Invalid version test passed\n";
}

void test_method_not_allowed()
{
    std::cout << "\nTesting unsupported method...\n";

    HttpRequest request;
    request.method = "DELETE";
    request.path = "/test";
    request.version = "HTTP/1.0";

    HttpResponse response = RequestHandler::handleRequest(request);
    std::string result = response.toString();

    assert(result.find("405 Method Not Allowed") != std::string::npos);

    std::cout << "✓ Method not allowed test passed\n";
}

void test_custom_headers()
{
    std::cout << "\nTesting custom headers...\n";

    HttpResponse response(HttpResponse::OK);
    response.setHeader("X-Custom-Header", "test-value");
    response.setHeader("Cache-Control", "no-cache");
    response.setBody("Test body");

    std::string result = response.toString();

    assert(result.find("X-Custom-Header: test-value") != std::string::npos);
    assert(result.find("Cache-Control: no-cache") != std::string::npos);

    std::cout << "✓ Custom headers test passed\n";
}

void test_different_status_codes()
{
    std::cout << "\nTesting different status codes...\n";

    HttpResponse response404(HttpResponse::NOT_FOUND);
    std::string result404 = response404.toString();
    assert(result404.find("404 Not Found") != std::string::npos);

    HttpResponse response500(HttpResponse::INTERNAL_SERVER_ERROR);
    std::string result500 = response500.toString();
    assert(result500.find("500 Internal Server Error") != std::string::npos);

    std::cout << "✓ Status codes test passed\n";
}

int main()
{
    std::cout << "========================================\n";
    std::cout << "HTTP Response Code Tests\n";
    std::cout << "========================================\n\n";

    try
    {
        test_basic_response();
        test_get_request();
        test_post_request();
        test_invalid_version();
        test_method_not_allowed();
        test_custom_headers();
        test_different_status_codes();

        std::cout << "\n========================================\n";
        std::cout << "✓ All tests passed successfully!\n";
        std::cout << "========================================\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }

    return 0;
}